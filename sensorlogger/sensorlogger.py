from datetime import datetime
from encodings import utf_8
import jwt
import json
from urllib import request
from flask import Flask, request, jsonify, render_template
from data import sensorData
from db import DbOps
from flask_restful import Api,Resource

app = Flask(__name__)
app.config.from_pyfile('Config/config.py')
api = Api(app)

@app.route("/home")
def index():
    line_labels=[]
    temperature_values=[]
    humidity_values = []
    co_values = []
    d = DbOps()
    r = d.get_data()
    d.close_conn()
    skiptill = len(r) // 300
    count = 1
    for row in r:
        if (row['type'] == 'temperature'):
            count = count + 1
            if (count % skiptill == 0):
                line_labels.append(row['time'])
                temperature_values.append(row['value'])
        if (row['type'] == 'humidity'):
            if (count % skiptill == 0):
                humidity_values.append(row['value'])
        if (row['type'] == 'co'):
            if (count % skiptill == 0):
                co_values.append(row['value'])
    return render_template('line_chart.html', title='Environmental Data', max=100, labels=line_labels, temperatures=temperature_values, humidities = humidity_values, cos=co_values)

class Home(Resource):
    def post(self):
        header = request.headers.get('Content-Type')
        if (header == 'text/html'):
            data = request.data
            #print(data)
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
        return jsonify(r)
        

api.add_resource(Home, "/")
api.add_resource(SensorData, "/sensor")

if __name__ == "__main__":
    app.run()
