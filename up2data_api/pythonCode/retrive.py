import pymongo

client = pymongo.MongoClient('latour.mboituva.app.br:3000')
mydb=client['WorldSecurity']
information = mydb.SecurityHistorical

col = mydb["SecurityHistorical"]

findOne = col.find_one()
print(findOne)

'''Return Only Some Fields
The second parameter of the find() method is an object describing which fields to include in the result.

This parameter is optional, and if omitted, all fields will be included in the result.

Example
Return only the names and addresses, not the _ids:'''

for x in col.find({},{ "_id": 0, "name": 1, "address": 1 }):
  print(x)



#Neste caso eu checo se existe a chave virtual_machines, depois eu especifíco qual chave e qual o valor da chave eu quero
#Ele retorna o json inteiro que preciso
col.find_one({'Virtual_Machines': {'$exists': True}}, {'_id':0})