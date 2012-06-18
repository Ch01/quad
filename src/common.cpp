/* Copyright (C) 2012 Chol Nhial <ch01.cout@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <curses.h>

// Variables that are used in some of the classes
const char * program_name;
const char * outputdir;
const char * inputdir;
int verbose;

// - Reference to int "open(const char *pathname, int flags);" -//
int (*p_open)(const char *a,int b,...) = open;

//----------------------------------------------//


/* Prints error messages to stderr in a format &
 * retrun 1 to the system_error */

void error (const char* cause, const char* message)
{
    /* Print an error message to stderr.  */
    fprintf (stderr, "%s: error: (%s) %s\n", program_name, cause, message);
    /* End the program.  */
    exit (1);
}

/* This function calls error() to print a message in a
 * sort of format*/
void system_error (const char* operation)
{
    /* Generate an error message for errno.  */
    error (operation, strerror (errno));
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/* Searches for the file a path. When the
 * file name has been succefully located it's
 * stored in dest*/
void GetFileNameInPath(const char * srcpath,char *dest)
{

    bool found  = false;

// scan for '/'
    for (int s = 0; s < strlen(srcpath); s++)
    {
        if (srcpath[s] == '/')
        {
            found = true;
            break;
        }
    }
    /* '/' was not found that indicates the
     * path name is the file name, so copy
     * it to 'dest' and return*/
    if (found == false)
    {
        strcpy(dest,srcpath);
        return;
    }

    /* '/' was found in the 'srcpath' string now
     * locate the name of the file in this path*/
    int counter = 0;
    int o = 0;
    int i = strlen(srcpath);
    while (srcpath[i] != '/')
    {
        counter++;
        i--;
    }
    counter--;
    for (int pos = strlen(srcpath) - counter; pos < strlen(srcpath); pos++)
        dest[o++] = srcpath[pos];
    dest[o] = 0;
}

void init_curses()
{
    if (initscr() == NULL)
        system_error("initscr");
}

void term_curses()
{
    endwin();
}
