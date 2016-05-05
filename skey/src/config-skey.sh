rm -f config.h
rm -f Makefile
#stdlib
MYCC=""
INSTPATH=""
#checking for the compiler
while getopts "C:" opt; do
  case $opt in
    C)
      MYCC=$OPTARG
      shift `(OPTIND-1)`
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done
# first test the compiler
echo 'main(){}' > cmptest.c
if [ "$MYCC" = "" ];
then
  CCLIST="gcc cc"
  for c in $CCLIST; do
    if $c cmptest.c > /dev/null 2>&1; then
      MYCC="$c"
      break;
    fi
  done
  if test -z "$MYCC"; then
    echo 'cannot find any working C compiler'
    exit 1
  fi
else # then, they specified a compiler, so i'll test that compiler
  if $MYCC cmptest.c >/dev/null 2>&1; then
    echo 'defined compiler works'
  else
    echo 'defined compiler doesnt work'
    exit 1
  fi
fi
rm -f cmptest.c
#i have a legit
#checking for instpath, a necessary argument
if [ $# = '1' ]; then
  INSTPATH=$1
  echo $INSTPATH
else
  echo "Invalid number of options: $#"
fi
NAME=`uname`
echo '#define SI_HOSTNAME "'$NAME'"'>> config.h
#test for sys/stat.h
if test -f /usr/include/stdlib.h
then
  echo '#define HAS_SYS_STAT' >> config.h
  echo /usr/include/sys/stat.h ' was found.'
else
  echo /usr/include/sys/stat.h' was not found'
fi
#test for stdlib
if test -f /usr/include/stdlib.h
then
  echo '#define HAS_STD_LIB' >> config.h
  echo /usr/include/stdlib.h ' was found.'
else
  echo /usr/include/stdlib.h ' was not found'
fi
#test for sys/types
if test -f /usr/include/sys/types.h
then
  echo '#define HAS_SYS_TYPES' >> config.h
  echo /usr/include/sys/types.h 'was found.'
else
  echo /usr/include/sys/types.h 'was not found.'
fi
#test for ctype.h
if test -f /usr/include/ctype.h >> config.h
then
  echo '#define HAS_CTYPE' >> config.h
  echo /usr/include/ctype.h 'was found'
else
  echo /usr/include/ctype.h 'was not found'
fi
if test -f /usr/include/fcntl.h; then
  echo '#define HAS_FCNTL' >> config.h
  echo /usr/include/fcntl.h 'was found'
else
  echo /usr/include/fcntl.h 'was not found'
fi
#test for getopt
if test -f /usr/include/getopt.h; then
  echo '#define HAS_GETOPT' >> config.h
  echo /usr/include/getopt.h ' was found'
else
  echo /usr/include/getopt.h ' was not found.'
fi
# test for unistd, shadow, sys/systeminfo,
if test -f /usr/include/unistd.h
then
  echo '#define HAS_UNISTD' >> config.h
  echo /usr/include/unistd.h 'was found'
  echo 'dynamically altering makefile via sed'
  sed "s/@crypt@/-lcrypt/g" < makefile.tmpl >> tmpmk
else
  echo /usr/include/unistd.h 'was not found'
fi
# test for shadow
if test -f /usr/include/shadow.h
then
  echo '#define HAS_SHADOW' >> config.h
  echo /usr/include/shadow.h 'was found'
else
  echo /usr/include/shadow.h 'was not found'
fi
# test for sys/systeminfo
if test -f /usr/include/sys/systeminfo.h
then
  echo '#define HAS_SYSTEMINFO' >> config.h
  echo /usr/include/sys/systeminfo.h 'was found'
else
  echo /usr/include/sys/systeminfo.h 'was not found'
fi
#test for termios
echo '#include <termios.h>' >> termtest.c
echo '#include <sys/ioctl.h>' >> termtest.c
echo 'main(){ioctl(0, TIOCGETA, 0);}' >> termtest.c
echo '#include <termios.h>' >> termtest2.c
echo '#include <sys/ioctl.h>' >> termtest2.c
echo 'main(){ioctl(0, TCGETS, 0);}' >> termtest2.c
if test -f /usr/include/termios.h
then
  echo '#define HAS_TERMIOS' >> config.h
  echo /usr/include/termios.h 'was found.'
  if gcc termtest.c > /dev/null 2>&1 ; then
    echo '#define USES_TIOCGETA' >> config.h
    echo 'uses TIOCGETA'
  else
    echo 'doesnt use TIOCGETA'
    echo 'its normal'
  fi
else
  echo /usr/include/termios.h 'was not found. using sgtty'
fi
rm -f termtest2.c
rm -f termtest.c
#little endian
echo 'int main(){unsigned int n = 1;char *c = (char *)&n;printf("%d", c[0]);return 0;}' >> myendian.c
gcc -w myendian.c -o myendian
    if  [ `./myendian` = '1' ] ;
    then
      echo 'is Little Endian'
      echo '#ifdef LITTLE_ENDIAN' >> config.h
      echo '#undef LITTLE_ENDIAN' >> config.h
      echo '#endif' >> config.h
      echo '#define LITTLE_ENDIAN' >> config.h
    else
      echo 'is big endian'
      echo '#ifdef BIG_ENDIAN' >> config.h
      echo '#undef BIG_ENDIAN' >> config.h
      echo '#endif' >> config.h
      echo '#define BIG_ENDIAN' >> config.h
    fi
rm -f myendian myendian.c
# test for libcrypt, include it if it exists
if test -f /usr/include/crypt.h
then
  echo 'found crypt';
  echo '#define HAS_CRYPT' >> config.h
  echo 'dynamically altering makefile via sed'
  if test -f tmpmk;
  then sed "s/@crypt@/-lcrypt/g" < tmpmk >> tmpmk2
  else
    sed "s/@crypt@/-lcrypt/g" < makefile.tmpl >> tmpmk2
  fi
else
  echo 'did not find crypt';
  echo 'dynamically altering makefile via sed'
  if test -f tmpmk;
  then sed "s/@crypt@//g" < tmpmk >> tmpmk2
  else
    sed "s/@crypt@//g" < makefile.tmpl >> tmpmk2
  fi
fi
# if libcrypt doesnt exist, must dynamically alter the makefile
sed "s/@MYCC@/$MYCC/g" < tmpmk2 >> tmpmk3
sed "s:@INSTPATH@:$INSTPATH:g" < tmpmk3 >> Makefile
#clean all temps
rm -f a.out
rm -f tmpmk
rm -f tmpmk2
rm -f tmpmk3
echo 'all done!'
