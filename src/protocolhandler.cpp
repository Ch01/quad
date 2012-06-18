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

#include "protocolhandler.h"
#include "network.h"
#include "downloader.h"
#include "uploader.h"

/* Resposible for the server side protocol*/
void protocolhandler::serverside(int sock)
{
    struct fileinfo downloadfile_info,temp;
    int length = sizeof(downloadfile_info);
    int rv;

    while ((rv = recv(sock,&command,sizeof(command),0)) != -1 || rv != 0)
    {
        command = ntohs(command);
        if (command == ACK)
        {
            if (send(sock,&command,sizeof(command),0) == -1)
                system_error("send");
        }
        else if (command == UPLOAD)
        {
            command = htons(ACK);
            if (send(sock,&command,sizeof(command),0) == -1)
                system_error("send");
            command = htons(READY);
            if (send(sock,&command,sizeof(command),0) == -1)
                system_error("send");
            if ((rv = recv(sock,&command,sizeof(command),0)) != -1 || rv != 0)
            {
                command = ntohs(command);
                switch (command)
                {
                case ACK:
                {
                    rv = recvall(sock,&temp,&length);
                    if (rv == -1 || rv == 0)
                        break;
                    downloadfile_info.packsize = ntohs(temp.packsize);
                    strcpy(downloadfile_info.filename,temp.filename);
                    downloadfile_info.filesize = ntohl(temp.filesize);
                    downloadfile_info.blocksize = ntohs(temp.blocksize);
                    downloader filedownloader;
                    filedownloader.open(downloadfile_info.filename);
                    filedownloader.download(sock,downloadfile_info.blocksize,downloadfile_info.filesize);
                    exit(0);
                }
                break;
                default :
                {
                    std::cerr << "Error: Client protocol not understood!\n";
                }
                }

            }
        }
        else if (command == READY)
        {
            if (send(sock,&command,sizeof(command),0) == -1)
                system_error("send");
        }
        else
        {
            std::cerr << "Error: Client protocol not understood\n";
            exit(1);
        }
    }
    if (rv == 0)
    {
        std::cerr << "Connection closed by client.\n";
        exit(0);
    }
    if (rv == -1)
        system_error("recv");

}

/* Resposible for the client side protocol */
void protocolhandler::clientside(int sock,const char *path,fileinfo uploadfile)
{
    int rv;
    uploader uploadd;
    command = htons(UPLOAD);
    if (send(sock,&command,sizeof(command),0) == -1)
        system_error("send");
    rv = recv(sock,&command,sizeof(command),0);
    switch (rv)
    {
    case -1:
        system_error("recv");
        break;
    case 0:
    {
        std::cerr << "Connection closed by host.\n";
        exit(0);
    }
    break;
    }

    command = ntohs(command);


    if (command == ACK)
    {
        rv = recv(sock,&command,sizeof(command),0);
        switch (rv)
        {
        case -1:
            system_error("recv");
            break;
        case 0:
        {
            std::cerr << "Connection closed by host.\n";
            exit(0);
        }
        break;
        }
        command = ntohs(command);
        if (command == READY)
        {
            command = htons(ACK);
            if (send(sock,&command,sizeof(command),0) == -1)
                system_error("send");

            int length = sizeof(uploadfile);
            uploadfile.blocksize = htons(uploadfile.blocksize);
            uploadfile.filesize = htonl(uploadfile.filesize);
            uploadfile.blocksize = htons(uploadfile.blocksize);
            if (sendall(sock,&uploadfile,&length) == -1)
                system_error("send");
            uploadd.open(path);
            uploadd.upload(sock,ntohs(uploadfile.blocksize), ntohl(uploadfile.filesize));
        }
        else
        {
            cerr << "Error: Host might be using a different protocol.\n";
            exit(1);
        }

    }
    else
    {
        cerr << "Error: Host might be using a different protocol.\n";
        exit(1);
    }
}
