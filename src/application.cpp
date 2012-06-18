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

#include "application.h"

/* Uploads a file to a remote computer */
void App::upload(const char *host,const char *port, const char *path)
{
    protocolhandler protohandler;
    // start the network for client
    net.start(host,port,CLIENT);
    struct fileinfo uploadfile;
    struct stat filestat;
    if (stat(path,&filestat) == -1)
        system_error("stat");

// set up file info
    uploadfile.blocksize = 16;
    uploadfile.packsize = 76;
    uploadfile.filesize = filestat.st_size;

//-- Find filename in path --//
    char filename[MAX_FILENAME];
    GetFileNameInPath(path,filename);
    strcpy(uploadfile.filename,filename);
    protohandler.clientside(net.getsockfd(),path,uploadfile);

}

/* Downloads a file from a remote computer */
void App::download(const char * lport)
{
    net.start(NULL,lport,SERVER);
}

