import mariadb
import sys
import json

class DbOps:
    def __init__(self):
        # Connect to MariaDB Platform
        try:
            self.conn = mariadb.connect(
                user="<user>",
                password="<pass>",
                host="localhost",
                port=3306,
                database="data_repository"

            )
        except mariadb.Error as e:
            print(f"Error connecting to MariaDB Platform: {e}")
            sys.exit(1)

    def get_data(self):
        cur = self.conn.cursor()
        cur.execute("select Type,Data from sensor_data")
        rows = cur.fetchall()
        r = []
        for row in rows:
            r.append({'type' : row[0], 'data' : row[1]})
        #for (Type,Data) in cur:
        #    r = f"{Type} sensor with {Data}"
        cur.close();
        return r

    def insert_data(self, sType, sData):
        cur = self.conn.cursor()
        sql = "INSERT INTO sensor_data (Type, Data) VALUES (%s, %s)"
        val = (sType, sData)
        cur.execute(sql, val)
        self.conn.commit()

    def close_conn(self):
        self.conn.close()
