#!/bin/bash
lsmod | grep gpio_event || insmod /lib/modules/gpio_event_drv.ko
gpio-event 17:R:100 
gpio-event 18:R:100

LAPTIME1_REL=0
LAPTIME1_ABS_OLD=0
LAPTIME1_REL_BEST=100000000000
LAPTIME2_ABS_OLD=0
LAPTIME2_REL_BEST=100000000000
 
cat /dev/gpio-event | while read sample; 
do
    if [ ! -z "`echo $sample | grep ^17`"  ]; then
	LAPTIME1_ABS=`echo $sample | cut -d " " -f 3 | tr -d \.` 
	let LAPTIME1_REL=LAPTIME1_ABS-LAPTIME1_ABS_OLD
	echo "1: " $LAPTIME1_REL
	let LAPTIME1_ABS_OLD=LAPTIME1_ABS
    fi

    if [ ! -z "`echo $sample | grep ^18`"  ]; then
	LAPTIME2_ABS=`echo $sample | cut -d " " -f 3 | tr -d \.` 
	let LAPTIME2_REL=LAPTIME2_ABS-LAPTIME2_ABS_OLD
	echo "2: " $LAPTIME2_REL
	let LAPTIME2_ABS_OLD=LAPTIME2_ABS
    fi
done
