/*
 * S/KEY v1.1b (skey.h)
 *
 * Authors:
 *          Neil M. Haller <nmh@thumper.bellcore.com>
 *          Philip R. Karn <karn@chicago.qualcomm.com>
 *          John S. Walden <jsw@thumper.bellcore.com>
 *
 * Modifications:
 *          Scott Chasin <chasin@crimelab.com>
 *
 * Main client header
 */
#include "config.h"
#if	defined(__TURBOC__) || defined(__STDC__) || defined(LATTICE)
#define	ANSIPROTO	1
#endif

int dflag; // debug level

#define debug1(fmt, ...) if(dflag > 0) fprintf(stderr,"%s ==> (%s:%d) --> " fmt, __FILE__,__FUNCTION__, __LINE__, ##__VA_ARGS__) // entering and exiting a function
#define debug2(fmt, ...) if(dflag > 1) fprintf(stderr,"%s ==> (%d) --> " fmt, __FILE__, __LINE__, ##__VA_ARGS__) // return
#define debug3(fmt, ...) if(dflag > 2) fprintf(stderr,"%s ==> (%d) --> " fmt, __FILE__, __LINE__, ##__VA_ARGS__) // arguments

#ifndef	__ARGS
#ifdef	ANSIPROTO
#define	__ARGS(x)	x
#else
#define	__ARGS(x)	()
#endif
#endif

#if defined(HAVE_TERMIO_H)
 	#include <termio.h>
	 	#ifdef HAVE_TERMIOS_H
 			#include <termios.h>
	 	#endif
 	#define TTYSTRUCT termio
 	#define stty(fd,buf) ioctl((fd),TCSETA,(buf))
  	#define gtty(fd,buf) ioctl((fd),TCGETA,(buf))
#elif defined(HAVE_TERMIOS_H)
 	#include <termios.h>
 	#define TTYSTRUCT termios
 	#define stty(fd,buf) ioctl((fd),TIOCSETA,(buf))
  	#define gtty(fd,buf) ioctl((fd),TIOCGETA,(buf))
#elif defined(HAVE_SGTTY_H)
 	#include <sgtty.h>
 	#define TTYSTRUCT sgttyb
 	#define stty(fd,buf) ioctl((fd),TIOCSETN,(buf))
  	#define gtty(fd,buf) ioctl((fd),TIOCGETP,(buf))
 	struct tchars chars;
#else
 	#error "NO TTY Support Available!"
#endif

struct TTYSTRUCT newtty;
struct TTYSTRUCT oldtty;



// #ifdef HAVE_TERMIOS_H
// # include <termios.h>
// # include <sys/ioctl.h>
// # define TTYSTRUCT termios
// # define stty(fd,buf) ioctl((fd),tcsetattr,(buf))
// # define gtty(fd,buf) ioctl((fd),tcgetattr,(buf))
// #else //otherwise it's just sgtty
// # include <sgtty.h>
// # define TTYSTRUCT sgttyb //let me recommit
// # define stty(fd,buf) ioctl((fd),TIOCSETN,(buf))
// # define gtty(fd,buf) ioctl((fd),TIOCGETP,(buf))
// #endif

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif
// any solaris thing, danny used shadow. use shadow
#ifdef HAVE_SHADOW_H
#ifdef WORDS_BIGENDIAN // its a sparky thing
#define setpriority(x,y,z)      z
#endif
#endif

#ifdef HAVE_LIBCRYPT
#include <crypt.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

/* Server-side data structure for reading keys file during login */
struct skey
{
  FILE *keyfile;
  char buf[256];
  char *logname;
  int n;
  char *seed;
  char *val;
  long recstart;		/* needed so reread of buffer is efficient */


};

/* Client-side structure for scanning data stream for challenge */
struct mc
{
  char buf[256];
  int skip;
  int cnt;
};

void f __ARGS ((char *x));
int keycrunch __ARGS ((char *result, char *seed, char *passwd));
char *btoe __ARGS ((char *engout, char *c));
char *put8 __ARGS ((char *out, char *s));
int etob __ARGS ((char *out, char *e));
void rip __ARGS ((char *buf));
int skeychallenge __ARGS ((struct skey * mp, char *name, char *ss));
int skeylookup __ARGS ((struct skey * mp, char *name));
int skeyverify __ARGS ((struct skey * mp, char *response));
