from encodings import utf_8
import jwt
import json
from urllib import request
from flask import Flask, request, jsonify
from data import sensorData
from db import DbOps
from flask_restful import Api,Resource

app = Flask(__name__)
app.config['SECRET'] = "<Secret>"
app.config['DB_HOST'] = "<HOST>"
app.config['DB_USER'] = "<user>"
app.config['DB_PASS'] = "<pass>"
api = Api(app)

class Home(Resource):
    def get(self):
        return "Hello this is a test application"

    def post(self):
        header = request.headers.get('Content-Type')
        if (header == 'text/html'):
            data = request.data
            print(data)
            payload = jwt.decode(data, app.config['SECRET'], ["HS256"])
            

            d=DbOps()
            performedTime = payload['time']
            d.insert_data('temperature', performedTime, payload['temperature'])
            d.insert_data('humidity', performedTime, payload['humidity'])
            d.insert_data('co', performedTime, payload['co'])
            d.close_conn()
        else:
            return 'Content-Type not supported!'

class SensorData(Resource):
    def get(self):
        d = DbOps()
        r = d.get_data()
        d.close_conn();
        #d = sensorData('test', 'data')
        return jsonify(r)
        

api.add_resource(Home, "/")
api.add_resource(SensorData, "/sensor")

if __name__ == "__main__":
    app.run(host="0.0.0.0",port=8080)
