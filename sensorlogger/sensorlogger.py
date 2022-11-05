from datetime import datetime, timedelta
from encodings import utf_8
import jwt
import json
from urllib import request
from flask import Flask, request, jsonify, render_template
from data import sensorData
from db import DbOps
from flask_restful import Api,Resource
from datetime import datetime
from datetime import timedelta
import pytz

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
    r = d.get_data(600)
    d.close_conn()
    localTZ = pytz.timezone('Europe/Budapest')
    local_datetime = datetime.now()
    for row in r:
        if (row['type'] == 'temperature'):
            db_datetime = row['time']
            if (db_datetime.year == 1970):
                    local_datetime = local_datetime + timedelta(minutes=5)
            else:
                    local_datetime = localTZ.localize(db_datetime)    
            line_labels.append(local_datetime)
            temperature_values.append(row['value'])
        if (row['type'] == 'humidity'):
            humidity_values.append(row['value'])
        if (row['type'] == 'co'):
            co_values.append(row['value'])

    current_values = {'curr_temp': temperature_values[len(temperature_values) - 1],
    'curr_hum': humidity_values[len(humidity_values) - 1],
    'curr_co': co_values[len(co_values) - 1]}

    return render_template('line_chart.html', title='Environmental Data', max=100, labels=line_labels, temperatures=temperature_values, humidities = humidity_values, cos=co_values, **current_values)


def test(self):
    line_labels=[]
    temperature_values=[]
    humidity_values = []
    co_values = []
    d = DbOps()
    r = d.get_data(0)
    d.close_conn()
    skiptill = len(r) // 300
    localTZ = pytz.timezone('Europe/Budapest')
    count = 1
    local_datetime = datetime.now()
    for row in r:
        if (row['type'] == 'temperature'):
            count = count + 1
            if (count % skiptill == 0):
                db_datetime = row['time']
                if (db_datetime.year == 1970):
                        local_datetime = local_datetime + timedelta(minutes=5)
                else:
                        local_datetime = localTZ.localize(db_datetime)    
                line_labels.append(local_datetime)
                temperature_values.append(row['value'])
            curr_temp = row['value']
        if (row['type'] == 'humidity'):
            if (count % skiptill == 0):
                humidity_values.append(row['value'])
            curr_hum = row['value']
        if (row['type'] == 'co'):
            if (count % skiptill == 0):
                co_values.append(row['value'])
            curr_co = row['value']

    current_values = {'curr_temp': curr_temp,
    'curr_hum': curr_hum,
    'curr_co': curr_co}
    return render_template('line_chart.html', title='Environmental Data', max=100, labels=line_labels, temperatures=temperature_values, humidities = humidity_values, cos=co_values, **current_values)


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

api.add_resource(Home, "/")

if __name__ == "__main__":
    app.run()
