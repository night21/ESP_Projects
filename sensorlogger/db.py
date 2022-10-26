import mariadb
import sys
from flask import current_app as app

class DbOps:
    def __init__(self):
        # Connect to MariaDB Platform
        try:
            self.conn = mariadb.connect(
                user=app.config['DB_USER'],
                password=app.config['DB_PASS'],
                host=app.config['DB_HOST'],
                port=3306,
                database="data_repository"

            )
        except mariadb.Error as e:
            print(f"Error connecting to MariaDB Platform: {e}")
            sys.exit(1)

    def get_data(self):
        cur = self.conn.cursor()
        cur.execute("select type, time, value from sensor_data")
        rows = cur.fetchall()
        r = []
        for row in rows:
            r.append({'type' : row[0], 'time' : row[1], 'value' : row[2]})
        #for (Type,Data) in cur:
        #    r = f"{Type} sensor with {Data}"
        cur.close();
        return r

    def insert_data(self, type, time, value):
        cur = self.conn.cursor()
        sql = "INSERT INTO sensor_data (type, time, value) VALUES (%s, %s, %s)"
        val = (type, time, value)
        cur.execute(sql, val)
        self.conn.commit()

    def close_conn(self):
        self.conn.close()
