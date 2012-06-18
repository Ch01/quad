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

#include "uploader.h"

uploader::uploader()
{
    pathname = NULL;
}
/* Opens a file for reading reports errors if any*/
void uploader::open(const char* path)
{
    if ((fd = p_open(path, O_RDONLY)) == -1)
        system_error("open");
    pathname = new char [strlen(path)+1];
    strcpy(pathname,path);
}

/* Uploads a file to a remote computer in chunks(blocksize) given
 * by the fact that there's an open file descriptor to read from. */
void uploader::upload(int sock,short bsize,long filesize)
{
// Did the programmer not open a file
    if (pathname != NULL)
    {
        int Read,blocksize,position,length;
        position = 0;
        char * buffer = new char[bsize];
        int onepercent = (double) 1/100*filesize / 4096 + 1;
        onepercent *= 4096;
        int percentage = 1;
        int  i = 1;
        if (verbose)
        {
            init_curses();
            move(0,0);
            printw("0% Uploaded.");
            refresh();
        }
        while (position < filesize)
        {
            if (filesize-position < bsize)
            {
                blocksize = filesize-position;
                delete [] buffer;
                buffer = new char [blocksize];
            }
            else
                blocksize = bsize;
            length = blocksize;
            if ((Read = read(fd,buffer,blocksize)) == -1)
            {
                term_curses();
                system_error("read");
            }
            if ((sendall(sock,buffer,&length)) == -1)
            {
                term_curses();
                system_error("send");
            }

            position += Read;
            if (verbose)
            {
                if (position == onepercent*i)
                {
                    move(0,0);
                    printw("%d% Uploaded.",percentage*i);
                    refresh();
                    i++;
                }


                if (position == filesize)
                {
                    move(0,0);
                    printw("100% Uploaded.");
                    refresh();
                    sleep(3);
                }
            }
            off_t seeker = position;
            lseek(fd,seeker,0);
        }
        term_curses();
        close(fd);
        if (verbose)
        {
            cout << "Succesfully uploaded --> " << pathname << "\n";
            cout << "size: " << filesize << "\n";
        }


        // Big programmer mistake
    } else {
        cerr << "No path speciefied!.\n";
        exit(1);
    }
}

uploader::~uploader()
{
// cleaning up
    delete [] pathname;
}


