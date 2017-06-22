/*
 * S/KEY v1.1b (skey.c)
 *
 * Authors:
 *          Neil M. Haller <nmh@thumper.bellcore.com>
 *          Philip R. Karn <karn@chicago.qualcomm.com>
 *          John S. Walden <jsw@thumper.bellcore.com>
 *          Scott Chasin <chasin@crimelab.com>
 *
 *
 * Stand-alone program for computing responses to S/Key challenges.
 * Takes the iteration count and seed as command line args, prompts
 * for the user's key, and produces both word and hex format responses.
 *
 * Usage example:
 *	>skey 88 ka9q2
 *	Enter password:
 *	OMEN US HORN OMIT BACK AHOY
 *	>
 */


#include <stdio.h>
#ifdef HAS_STD_LIB
#include <stdlib.h>
#else
#include <sys/types.h>
#endif
#include <string.h>
#include <fcntl.h>
#include "md4.h"
#include "skey.h"


int redirect;
char *readpass ();
void usage ();
void logging(char* path);

main (argc, argv)
  int argc;
  char *argv[];
{
  debug1("entering function:\n");
  debug3("Args: %d\n", argc);
  int n, cnt, i, pass = 0;
  char passwd[256], key[8], buf[33], *seed, *slash;
  dflag = 0;
  cnt = 1;

  while ((i = getopt (argc, argv, "l:n:p:dh")) != EOF)
  {
    switch (i)
    {
    case 'n':
      cnt = atoi (optarg);
      break;
    case 'p':
      strcpy (passwd, optarg);
      pass = 1;
      break;
    case 'd':
      dflag++;
      break;
    case 'h':
      usage("help: ");
      break;
    case 'l':
      logging(optarg);
      break;
    }
  }

  /* could be in the form <number>/<seed> */

  if (argc <= optind + 1)
  {
    /* look for / in it */
    if (argc <= optind)
    {
      usage (argv[0]);
      debug1("exiting function\n");
      debug2("return:%d\n", 1);
      return 1;
    }

    slash = strchr (argv[optind], '/');
    if (slash == NULL)
    {
      usage (argv[0]);
      return 1;
    }
    *slash++ = '\0';
    seed = slash;

    if ((n = atoi (argv[optind])) < 0)
    {
      printf ("%s not positive\n", argv[optind]);
      usage (argv[0]);
      debug1("exiting function\n");
      debug2("return:%d\n", 1);
      return 1;
    }
  }
  else
  {

    if ((n = atoi (argv[optind])) < 0)
    {
      printf ("%s not positive\n", argv[optind]);
      usage (argv[0]);
      debug1("exiting function\n");
      debug2("return:%d\n", 1);
      return 1;
    }
    seed = argv[++optind];
  }

  /* Get user's secret password */
  if (!pass)
  {
    printf ("Enter secret password: ");
    readpass (passwd, sizeof (passwd));
  }

  rip (passwd);

  /* Crunch seed and password into starting key */
  if (keycrunch (key, seed, passwd) != 0)
  {
    fprintf (stderr, "%s: key crunch failed\n", argv[0]);
    debug1("exiting function");
    debug2("return:%d", 1);
    return 1;
  }
  if (cnt == 1)
  {
    while (n-- != 0)
      f (key);
    printf ("%s\n", btoe (buf, key));
#ifdef	HEXIN
    printf ("%s\n", put8 (buf, key));
#endif
   }
  else
  {
    for (i = 0; i <= n - cnt; i++)
      f (key);
    for (; i <= n; i++)
    {
#ifdef	HEXIN
      printf ("%d: %-29s  %s\n", i, btoe (buf, key), put8 (buf, key));
#else
      printf ("%d: %-29s\n", i, btoe (buf, key));
#endif
      f (key);
    }
  }
  debug1("exiting function\n");
  debug2("return:%d\n", 0);
  return 0;
}

void
 usage (s)
  char *s;
{
  debug1("exiting function\n");
  debug3("args:%s\n", s);
  printf ("Usage: %s [-d][-l] <logfile>[-n count] [-p password ] <sequence #>[/] <key> \n", s);
  debug1("exiting function\n");
  debug2("return:%d\n", 0);
}

void logging(char *path)
{
  redirect = open(path, O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR );
  // redirects stderr into a logfile
  if(redirect < 0){
    fprintf(stderr, "invalid file\n");
  }
  dup2(redirect, STDERR_FILENO);
}
