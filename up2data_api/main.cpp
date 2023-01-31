#include <QtCore>
#include <QtHttpServer>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>


/*static inline QString host(const QHttpServerRequest &request)
{
    return QString::fromLatin1(request.value("Host"));
}*/

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QHttpServer httpServer;
    httpServer.route("/", []() {
        return "Digite o nome de um ativo depois da barra. (/ativo/nomeAtivo)";
    });

    //Busca json do ativo no banco e retorna
    //(QString nomeAtivo, const QHttpServerRequest &request)
    httpServer.route("/ativo/", [] (QString nomeAtivo) {

       QDir::setCurrent("C:/up2data_api");
       QString scriptFile = QDir().currentPath() + "/pythonCode/retrieve.py";

        QFile pyFile(scriptFile);

        if(pyFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        {
            QTextStream fileContent(&pyFile);


            QString pyCode = "import pymongo\n";
            pyCode += "client = pymongo.MongoClient('latour.mboituva.app.br:3000')\n";

            pyCode += "mydb=client['WorldSecurity']\n";
            pyCode += "information = mydb.SecurityHistorical\n";

            pyCode += "col = mydb[\"SecurityHistorical\"]\n";

            pyCode += "jsonAtv = col.find_one({'TckrSymb': {'$exists': True}}, {'TckrSymb': '" + nomeAtivo.toUpper() + "'})\n";
            pyCode += "print(jsonAtv)\n";

            //Coloca o conteúdo dentro do arquivo
            fileContent << pyCode;

            pyFile.flush();
            pyFile.close();
        }
        else
        {
            qDebug() << "Não foi possível abrir o arquivo";
        }


        QStringList comandosPython = QStringList() << scriptFile;

        QProcess process;
        process.start ("C:/Python39/python.exe", comandosPython);

        process.waitForFinished();

        QJsonArray jsonParsed;

        if(!process.error())
        {
            QString json = process.readAllStandardOutput();

            QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());

            if(!doc.isNull())
            {
                if(doc.isArray())
                {
                    jsonParsed = doc.array();
                }
            }

        }
        else
        {
            QString erro = process.readAllStandardError();
            qDebug() << erro;
            //return QString(erro);
        }

        return QJsonArray(jsonParsed);
        //return QString("Ok it seems MOTHERSHOKER");
       });


    httpServer.afterRequest([](QHttpServerResponse &&resp) {
        resp.setHeader("Server", "Super server!");
        return std::move(resp);
    });


    const auto port = httpServer.listen(QHostAddress::Any);
    if (!port) {
        qDebug() << QCoreApplication::translate(
                "QHttpServerExample", "Server failed to listen on a port.");
        return 0;
    }

    qDebug() << QCoreApplication::translate(
            "QHttpServerExample", "Running on http://127.0.0.1:%1/ (Press CTRL+C to quit)").arg(port);

    return app.exec();
}
