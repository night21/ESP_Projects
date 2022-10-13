class sensorData:
    def __init__(self, type, data):
        self.type = type
        self.data = data

    def __str__(self):
        return f"{self.type} sensor sent {self.data}"