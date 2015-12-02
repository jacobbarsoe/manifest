#!/bin/sh
DAEMON=/usr/bin/laptime
NAME=laptimer
DESC="laptimer app"
ARGS="-platform linuxfb"
PIDFILE=/var/run/laptime.pid

test -f $DAEMON || exit 0

set -e

case "$1" in
    start)
        echo -n "starting $DESC: $NAME... "
	insmod /lib/modules/gpio-event-drv.ko
	gpio-event 17:R:100 18:R:100
	start-stop-daemon -b -S -m -p $PIDFILE -a $DAEMON -- $ARGS
	echo "done."
	;;
    stop)
        echo -n "stopping $DESC: $NAME... "
	start-stop-daemon --signal=1 -K -p $PIDFILE
	echo "done."
	;;
    restart)
        echo -n "restarting $DESC: $NAME... "
 	$0 stop
	$0 start
	echo "done."
	;;
    *)
	echo "Usage: $0 {start|stop|restart}"
	exit 1
	;;
esac


