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


//NOTE: Must be included in every source file

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <cstring>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <curses.h>

#define MAX_FILENAME 256
#define CLIENT 1
#define SERVER 2
//-- very Simple ftp protocol ---//
#define ACK 1
#define UPLOAD 2
#define READY 3
#define BADPROTOCOL 0

/* Used to hold file details */
struct fileinfo
{
    short packsize;
    char filename[MAX_FILENAME];
    long filesize;
    short blocksize;
};

extern const char * program_name;
extern const char * outputdir;
extern const char * inputdir;
extern int verbose;

/* This function sends all data that a simple
 * call to send() might not send,
 *It handles all send() errors correctly*/
template <typename P>
int sendall(int s, P *buf, int *len)
{
    int total = 0; // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;
    while (total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) {
            break;
        }
        total += n;
        bytesleft -= n;
    }
    *len = total; // return number actually sent here
    return n==-1?-1:0; // return -1 on failure, 0 on success
}

/* This function is the same as the function above accept it's
 * job is to recieve data, it recv all data that simple call to recv()
 * might have not received. It also handles all recv() errors correctly*/
template <typename P>
int recvall(int s, P * buf,int *len)
{
// the workbuffer for storing currently received buffer
    char workbuffer[*len];
// Holds the number of received bytes */
    int total = 0;
    // Holds the number of sent bytes
    int n;
    //holds the number of bytes left to received
    int bytesleft = *len;
    while (total < *len)
    {
        // recv and append to workbuffer
        n = recv(s,workbuffer+total,bytesleft,0);
        if (n== -1 || n == 0) {
            break;
        }
	
        // increment total by the number of received bytes
        total += n;
        bytesleft -= n;
    }
    // Copy workbuffer to to buf
    memcpy(buf,workbuffer,sizeof(workbuffer));
    switch(n)
    {
      case -1:
	return -1;
      break;
      case 0:
	return 0;
      break;
      default:
	return total;
    }
}

extern void system_error (const char* operation);
extern void error (const char* cause, const char* message);
extern void GetFileNameInPath(const char * srcpath,char *dest);
extern void *get_in_addr(struct sockaddr *sa);
// - Reference to int "open(const char *pathname, int flags);" -//
extern int (*p_open)(const char *a,int b,...);
extern void init_curses();
extern void term_curses();
#endif
