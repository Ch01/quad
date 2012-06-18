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

#include "downloader.h"

downloader::downloader()
{
    pathname = NULL;
}

/* Open a file for writing, checks errors if any exit*/
void downloader::open(const char* filename)
{
    std::string path;
    path = outputdir;
    if ((path[path.size() -1] != '/'))
    {
        path += '/';
    }
    path+=filename;

    /* If the file already exist overwrite it */
    if (access (path.c_str(), F_OK) == 0)
    {
        // enable reading & writing
        if (chmod(path.c_str(),0666) == -1)
            system_error("chmod");
    }

// Try to open the file
    if ((fd = p_open(path.c_str(),O_WRONLY)) == -1)
    {
//The file might not exist, try and create it
        if (errno == 2)
        {
            if ((fd = p_open(path.c_str(),O_CREAT | O_WRONLY)) == -1)
                system_error("open");
// Enable reading & writing
            if (chmod(path.c_str(),0666) == -1)
                system_error("chmod");
        }
        else
            system_error("open");
    }
    /*allocate enough memory to fit the path name of this filename
    * store the file name in the newly allocated buffer*/
    pathname= new char [strlen(filename)+path.size()];
    strcpy(pathname,path.c_str());
}

/* Downloads data in chunks or so called blocksize. It output the chunk
 * into a file given that the file is already opened*/
void downloader::download(int sock,short bsize,long filesize)
{

    // Has the programmer made a mistake?
    if (pathname != NULL)
    {

        int written,blocksize,position,length,rb;
        position = 0; // offset position in the file
        // allocate enough buffer for the blocksize
        char * buffer = new char[bsize];


        int onepercent = (double) 1/100*filesize / 4096 + 1;
        onepercent *= 4096;
        int percentage = 1;
        int  i = 1;
        if (verbose)
        {
            init_curses();
            move(0,0);
            printw("0% Downloaded.");
            refresh();
        }

        while (position < filesize)
        {
            // This section makes sure the we copy the last few bytes.
            if (filesize-position < bsize)
            {
                blocksize = filesize-position;
                delete [] buffer;
                buffer = new char [blocksize];
            }
            else
                blocksize = bsize;

            length = blocksize;
            // receive  a chunk  & receive it all
            rb = recvall(sock,buffer,&length);
            switch (rb)
            {
            case -1:
            {
                term_curses();
                system_error("recv");
            }
            break;
            case 0:
            {
                term_curses();
                std::cerr << "Connection closed by client.\n";
                exit(0);
            }
            break;
            }
            // write the chunk to file
            if ((written = write(fd,buffer,blocksize)) == -1)
            {
                term_curses();
                system_error("write");
            }

            // update the position in the file
            position += written;
            if (verbose)
            {
                if (position == onepercent*i)
                {
                    move(0,0);
                    printw("%d% Downloaded.",percentage*i);
                    refresh();
                    i++;
                }

                if (position == filesize)
                {
                    move(0,0);
                    printw("100% Downloaded.");
                    refresh();
                    sleep(3);
                }
            }

        }
        // Done with file descriptor, close it for now.
        term_curses();
        close(fd);
        if (verbose)
        {
            std::cout << "Downloaded --> " << pathname << "\n";
            std::cout << "Size: " << filesize << "\n";
        }
    }

// Might be a programmer mistake
    else
    {
        cerr << "No path speciefied!\n";
        exit(1);
    }

}

downloader::~downloader()
{
    // clean up
    delete [] pathname;
}

