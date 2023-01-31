import pymongo
client = pymongo.MongoClient('latour.mboituva.app.br:3000')
mydb=client['WorldSecurity']
information = mydb.SecurityHistorical
col = mydb["SecurityHistorical"]
jsonAtv = col.find_one({'TckrSymb': {'$exists': True}}, {'TckrSymb': 'BGIM22C030000'})
print(jsonAtv)
