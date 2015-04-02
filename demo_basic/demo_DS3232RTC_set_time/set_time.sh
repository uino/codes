# use +3600 in the summer

CURRENT_TIME=`date +%s`
echo "#define CURRENT_TIME ${CURRENT_TIME}+3600" > local_time.h
ino build && ino upload && ino serial
