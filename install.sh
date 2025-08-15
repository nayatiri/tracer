sudo pacman -S gpsd gpspipe jq grep
sudo systemctl start gpsd
sudo systemctl enable gpsd
sudo systemctl status gpsd
sudo gpsd /dev/ttyUSB0 -F /var/run/gpsd.sock
