sensorlogger service runs uwsgi
local user and group access
copy file to /etc/systemd/system

sudo sensorlogger.service /etc/systemd/system/.


Nginx host the app publicly
sudo cp sensorlogger /etc/nginx/sites-available/sensorlogger
sudo ln -s /etc/nginx/sites-available/sensorlogger /etc/nginx/sites-enabled/
