/* S/KEY v1.1b (skeysubr.c)
 *
 * Authors:
 *          Neil M. Haller <nmh@thumper.bellcore.com>
 *          Philip R. Karn <karn@chicago.qualcomm.com>
 *          John S. Walden <jsw@thumper.bellcore.com>
 *
 * Modifications:
 *          Scott Chasin <chasin@crimelab.com>
 *
 * S/KEY misc routines.
 */
#include "config.h"
#include <stdio.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#include <sys/types.h>
#endif

#include <string.h>
#include <signal.h>

#ifdef stty
# undef stty
#endif

#ifdef gtty
# undef gtty
#endif
#include "skey.h"

// #ifdef HAVE_TERMIOS_H
// struct termios newtty;
// struct termios oldtty;
// #else
// struct sgttyb newtty;
// struct sgttyb oldtty;
// struct tchars chars;
// #endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif



#define SIGTYPE void

SIGTYPE trapped();

#include "md4.h"

//#if (defined(__MSDOS__) || defined(MPU8086) || defined(MPU8080) \
    || defined(vax) || defined (MIPSEL))
//#define	LITTLE_ENDIAN
//#endif

/* Crunch a key:
 * concatenate the seed and the password, run through MD4 and
 * collapse to 64 bits. This is defined as the user's starting key.
 */
int
keycrunch(result,seed,passwd)
char *result;	/* 8-byte result */
char *seed;	/* Seed, any length */
char *passwd;	/* Password, any length */
{
	debug1("Entering function\n");
  debug3("Args:%s, %s, %s\n", result, seed, passwd);
	char *buf;
	MDstruct md;
	unsigned int buflen;
#ifdef WORDS_BIGENDIAN
	int i;
	register long tmp;
#endif

	buflen = strlen(seed) + strlen(passwd);
	if ((buf = (char *)malloc(buflen+1)) == NULL)
		return -1;
	strcpy(buf,seed);
	strcat(buf,passwd);

	/* Crunch the key through MD4 */
	sevenbit(buf);
	MDbegin(&md);
	MDupdate(&md,(unsigned char *)buf,8*buflen);

	free(buf);

	/* Fold result from 128 to 64 bits */
	md.buffer[0] ^= md.buffer[2];
	md.buffer[1] ^= md.buffer[3];

#ifndef	WORDS_BIGENDIAN
	/* Only works on byte-addressed little-endian machines!! */
	memcpy(result,(char *)md.buffer,8);
#else
	/* Default (but slow) code that will convert to
	 * little-endian byte ordering on any machine
	 */
	for (i=0; i<2; i++) {
		tmp = md.buffer[i];
		*result++ = tmp;
		tmp >>= 8;
		*result++ = tmp;
		tmp >>= 8;
		*result++ = tmp;
		tmp >>= 8;
		*result++ = tmp;
	}
#endif
debug1("exiting function\n");
debug2("return:%d\n", 0);
	return 0;
}

/* The one-way function f(). Takes 8 bytes and returns 8 bytes in place */
void f (x)
char *x;
{
	MDstruct md;
#ifdef	WORDS_BIGENDIAN
	register long tmp;
#endif

	MDbegin(&md);
	MDupdate(&md,(unsigned char *)x,64);

	/* Fold 128 to 64 bits */
	md.buffer[0] ^= md.buffer[2];
	md.buffer[1] ^= md.buffer[3];

#ifndef	WORDS_BIGENDIAN
	/* Only works on byte-addressed little-endian machines!! */
	memcpy(x,(char *)md.buffer,8);

#else
	/* Default (but slow) code that will convert to
	 * little-endian byte ordering on any machine
	 */
	tmp = md.buffer[0];
	*x++ = tmp;
	tmp >>= 8;
	*x++ = tmp;
	tmp >>= 8;
	*x++ = tmp;
	tmp >>= 8;
	*x++ = tmp;

	tmp = md.buffer[1];
	*x++ = tmp;
	tmp >>= 8;
	*x++ = tmp;
	tmp >>= 8;
	*x++ = tmp;
	tmp >>= 8;
	*x = tmp;
#endif

}

/* Strip trailing cr/lf from a line of text */
void rip (buf)
char *buf;
{
	debug1("entering function\n");
	debug3("Args:%s\n", buf);
	char *cp;

	if((cp = strchr(buf,'\r')) != NULL)
		*cp = '\0';

	if((cp = strchr(buf,'\n')) != NULL)
		*cp = '\0';
		debug1("exiting function\n");
    debug2("return:%d\n", 0);
}

char *readpass (buf,n)
char *buf;
int n;
{
	debug1("entering function\n");
	debug3("Args:%s, %d\n", buf, n);
#ifndef USE_ECHO
    set_term ();
    echo_off ();
#endif

    if(fgets(buf, n, stdin) == NULL){
			printf("fgets returned null");
		}

    rip (buf);

    printf ("\n\n");
    sevenbit (buf);

#ifndef USE_ECHO
    unset_term ();
#endif
		debug1("exiting function\n");
		debug2("return:%s\n", buf);
    return buf;
}

set_term ()
{
    gtty (fileno(stdin), &newtty);
    gtty (fileno(stdin), &oldtty);
    signal (SIGINT, trapped);
}

echo_off ()
{

#ifdef HAVE_TERMIOS_H
    newtty.c_lflag &= ~(ICANON | ECHO | ECHONL);
#else
    newtty.sg_flags |= CBREAK;
    newtty.sg_flags &= ~ECHO;
#endif

#ifdef HAVE_TERMIOS_H
    newtty.c_cc[VMIN] = 1;
    newtty.c_cc[VTIME] = 0;
    newtty.c_cc[VINTR] = 3;
#else
    ioctl(fileno(stdin), TIOCGETC, &chars);
    chars.t_intrc = 3;
    ioctl(fileno(stdin), TIOCSETC, &chars);
#endif

    stty (fileno (stdin), &newtty);
}

unset_term ()
{
    stty (fileno (stdin), &oldtty);

#ifndef HAVE_TERMIOS_H
    ioctl(fileno(stdin), TIOCSETC, &chars);
#endif
}

void trapped()
 {
  signal (SIGINT, trapped);
  printf ("^C\n");
  unset_term ();
  exit (-1);
 }

/* removebackspaced over charaters from the string */
backspace(buf)
char *buf;
{
	char bs = 0x8;
	char *cp = buf;
	char *out = buf;

	while(*cp){
		if( *cp == bs ) {
			if(out == buf){
				cp++;
				continue;
			}
			else {
			  cp++;
			  out--;
			}
		}
		else {
			*out++ = *cp++;
		}

	}
	*out = '\0';

}

/* sevenbit ()
 *
 * Make sure line is all seven bits.
 */

sevenbit (s)
char *s;
{
	debug1("Entering function\n");
  debug3("Args:%s\n", s);
   while (*s) {
     *s = 0x7f & ( *s);
     s++;
   }
	 debug1("exiting function\n");
   debug2("return:%d\n", 0);
}
