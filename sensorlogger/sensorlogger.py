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
app.config['SECRET'] = "<Secret>"
app.config['DB_HOST'] = "<HOST>"
app.config['DB_USER'] = "<user>"
app.config['DB_PASS'] = "<pass>"
api = Api(app)

@app.route("/home")
def index():
    line_labels=[]
    line_values=[]
    d = DbOps()
    r = d.get_data()
    d.close_conn();
    for row in r:
        if (row['type'] == 'temperature'):
            line_labels.append(row['time'])
            line_values.append(row['value'])
    return render_template('line_chart.html', title='Temperature', min=-20, max=40, labels=line_labels, values=line_values)

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
