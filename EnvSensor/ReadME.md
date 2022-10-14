Hardware:
- NodeMCU 0.9
- MQ-7 CO sensor
- DHT22 temperature and humidity sensor

Purpose:
- collect data on a remote location and send that in an encoded message to a server (see sensorlogger folder)

Message contains:
- UTC date and time (format accepted by MariaDB)
- temperature in celsius
- humidity in %
- carbon monodxise in ppm

Message encoded with JWT
