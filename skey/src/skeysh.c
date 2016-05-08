/* S/KEY v1.1b (skeysh.c)
 *
 * Authors:
 *          Neil M. Haller <nmh@thumper.bellcore.com>
 *          Philip R. Karn <karn@chicago.qualcomm.com>
 *          John S. Walden <jsw@thumper.bellcore.com>
 *          Scott Chasin <chasin@crimelab.com>
 *
 * S/Key authentication shell
 */

#include <stdio.h>
#include <string.h>
#include <pwd.h>

char userbuf[16] = "USER=";
char homebuf[128] = "HOME=";
char shellbuf[128] = "SHELL=";
char pathbuf[128] = "PATH=:/usr/ucb:/bin:/usr/bin";
char *cleanenv[] = { userbuf, homebuf, shellbuf, pathbuf, 0, 0 };
char *shell = "/bin/csh";

#ifndef HAVE_GETENV
char *getenv();
#endif
extern char **environ;
struct passwd *pwd;

main (argc, argv)
  int argc;
  char *argv[];
{
  int stat, pid;
  char user [8];

  if ((pwd = getpwuid(getuid())) == NULL) {
      fprintf(stderr, "Who are you?\n");
      return 1;
  }

  strcpy(user, pwd->pw_name);

  if ((pwd = getpwnam(user)) == NULL) {
      fprintf(stderr, "Unknown login: %s\n", user);
      return 1;
  }

  stat = skey_haskey (user);

  if (stat == 1) {
     fprintf(stderr,"keysh: no entry for user %s.\n", user);
     return -1;
  }

  if (stat == -1) {
     fprintf(stderr, "keysh: could not open key file.\n");
     return -1;
  }

  if (skey_authenticate (user) == -1) {
      printf ("Invalid response.\n");
      return -1;
  }

  if (setgid(pwd->pw_gid) < 0) {
      perror("keysh: setgid");
      return 3;
  }

  if (initgroups(user, pwd->pw_gid)) {
      fprintf(stderr, "keysh: initgroups failed\n");
      return 4;
  }

  if (setuid(pwd->pw_uid) < 0) {
      perror("keysh: setuid");
      return 5;
  }

  cleanenv[4] = getenv("TERM");
  environ = cleanenv;

  setenv("USER", pwd->pw_name, userbuf);
  setenv("SHELL", shell, shellbuf);
  setenv("HOME", pwd->pw_dir, homebuf);

  if (chdir(pwd->pw_dir) < 0) {
      fprintf(stderr, "No directory\n");
      return 6;
  }

  execv (shell, argv);
  fprintf(stderr, "No shell\n");
  return 7;
}

#ifndef HAVE_SETENV
setenv (ename, eval, buf)
char *ename, *eval, *buf;
{
  register char *cp, *dp;
  register char **ep = environ;

  /*
   * this assumes an environment variable "ename" already exists
   */
   while (dp = *ep++) {
       for (cp = ename; *cp == *dp && *cp; cp++, dp++)
            continue;
       if (*cp == 0 && (*dp == '=' || *dp == 0)) {
            strcat(buf, eval);
            *--ep = buf;
            return;
       }
   }
}
#endif

#ifndef HAVE_GETENV
char *getenv(ename)
char *ename;
{
  register char *cp, *dp;
  register char **ep = environ;

  while (dp = *ep++) {
         for (cp = ename; *cp == *dp && *cp; cp++, dp++)
              continue;
         if (*cp == 0 && (*dp == '=' || *dp == 0))
              return (*--ep);
  }
  return ((char *)0);
}
#endif
