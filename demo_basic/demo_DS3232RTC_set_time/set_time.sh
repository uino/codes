CURRENT_TIME=`date +%s`
echo "#define CURRENT_TIME ${CURRENT_TIME}" > local_time.h
ino build && ino upload && ino serial
