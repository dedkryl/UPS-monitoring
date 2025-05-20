mkdir /opt/ups
cp ups_service /opt/ups/ups_service
cp Config.json /opt/ups/Config.json
chmod +x ups.service
cp ups.service /opt/ups/
cp ups.service /etc/systemd/system/ups.service
sudo systemctl enable ups.service

