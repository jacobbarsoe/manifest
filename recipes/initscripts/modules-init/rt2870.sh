#the following sequence avoids errors in dmesg
sleep 5
modprobe crc-ccitt
modprobe cfg80211
modprobe mac80211
modprobe rt2x00lib
modprobe rt2800lib
modprobe rt2x00usb
modprobe rt2800usb