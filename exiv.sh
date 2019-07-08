#!/bin/bash

echo "usage : $0 [directory to copy] [files]"
for i in ${*:2}; do
	exiv2 -p a $i 2> /dev/null |
	gawk '$1 ~ /Exif.GPSInfo.GPSL.+/ {print $4,$5,$6}
		  $1 == "Exif.Image.DateTime" {print $4, $5}' |
	{
		read day time;
		day=`echo $day | tr ":" "-"`;
		time=`echo $time | tr ":" "-"`;
		echo $day $time
		if [ ! -d ${1}/$day ]; then mkdir ${1}/$day; mkdir ${1}/${day}/thumbnail; fi
		fname="${1}/${day}/${time}.${i##*.}"	
		cp $i $fname
		exiv2 -et $fname
		mv ${1}/${day}/${time}-thumb.jpg ${1}/${day}/thumbnail/

		read ns;
		read -a pos1;
		lat=`echo "${pos1[0]%deg} + ${pos1[1]%\'} / 60 + ${pos1[2]%\"} / 3600" | bc -l`
		read ew;
		read -a pos1;
		lng=`echo "${pos1[0]%deg} + ${pos1[1]%\'} / 60 + ${pos1[2]%\"} / 3600" | bc -l`
		if [ $ns == "South" ]; then lat=-$lat; fi
		if [ $ew == "West" ]; then lng=-$lng; fi
		exiv2 mo -c "$lat $lng" $fname;
	}
done

