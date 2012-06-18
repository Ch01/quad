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

#ifndef NETWORK_H_
#define NETWORK_H_

#include "protocolhandler.h"
#include "global.h"

class network
{
    protocolhandler protohandler;
    int sockfd,newfd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage clientaddr;
    socklen_t size;
public:
    network() {}
    void start(const char * host, const char * port, const short  type);
    int getsockfd() {
        return sockfd;
    }
};
#endif

