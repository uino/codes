# give as argument the folder on which to apply the script

TARGET=$1
if [ -z "$TARGET" ]; then
  echo "please specify target folder"
  exit
fi
cd $TARGET


mycompile() {
  FOLDER=`basename "$1"`
  cd $1
  echo "================> Compiling ${FOLDER}"
  if [ ! -L lib ];
  then
     ln -s ../../libraries lib
  fi
  if [ ! -d src ];
  then
    mkdir src
  fi
  if [ ! -f src/sketch.ino ];
  then
    echo "#include \"../${FOLDER}.ino\"" > src/sketch.ino
  fi
  # ino clean
  ino build
  if [ $? -ne 0 ]; then
    exit
  fi
  cd ..
}

for FOLDER in */; do
  mycompile ${FOLDER}
done

exit


