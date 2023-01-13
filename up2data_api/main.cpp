#include <QtCore>
#include <QtHttpServer>
#include <QProcess>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QHttpServer httpServer;
    httpServer.route("/", []() {
        return "Digite o nome de um ativo depois da barra. (/ativo/nomeAtivo)";
    });

    //Busca json do ativo no banco e retorna
    httpServer.route("/ativo/", [] (const QString nomeAtivo) {
        QString scriptFile = QCoreApplication::applicationDirPath() + "../../pythonCode/retrieve.py";

        QFile pyFile(scriptFile);

        if(pyFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        {
            QTextStream fileContent(&pyFile);


            QString pyCode = "import pymongo";
            pyCode += "client = pymongo.MongoClient('latour.mboituva.app.br:3000')";

            pyCode += "mydb=client['WorldSecurity']";
            pyCode += "information = mydb.SecurityHistorical";

            pyCode += "col = mydb[\"SecurityHistorical\"]";

            pyCode += "jsonAtv = col.find_one({'TckrSymb': {'$exists': True}}, {'TckrSymb':" + nomeAtivo.toUpper() + "})";
            pyCode += "print(jsonAtv)";

            //Coloca o conteúdo dentro do arquivo
            fileContent << pyCode;

            pyFile.flush();
            pyFile.close();
        }
        else
        {
            qDebug() << "Não foi possível abrir o arquivo";
        }


        QStringList comandosPython = QStringList() << "python.exe" << scriptFile;

        QProcess process;
        process.start ("python", comandosPython);

        if(!process.error())
        {
            QString json = process.readAllStandardOutput();
            qDebug() << json;
        }
        else
        {
            QString erro = process.readAllStandardError();
            qDebug() << erro;
        }

           //return QString("User %1").arg(id);
        process.waitForFinished();

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
