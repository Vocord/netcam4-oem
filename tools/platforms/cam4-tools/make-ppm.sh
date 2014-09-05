for i in $1*2336x1744.8bit.raw ; do echo -e "P5\n2336 1744\n255" >$i.ppm ; cat $i >>$i.ppm ; done
