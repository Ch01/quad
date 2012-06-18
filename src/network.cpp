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

#include "network.h"

/* Intiliazes structures to be used by getaddrinfo(), binds  to a local port and
 * puts the open socket fd in variable sockfd for latter use. or
 * if type is for client connect to a remote port.
 */
void network::start(const char * host, const char * port, const short  type)
{
    int yes = 1;
    int rv;


    switch (type)
    {
    case SERVER:
    {
        memset(&hints,0,sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;


        if ((rv = getaddrinfo(NULL, port, &hints,&servinfo)) != 0)
        {
            std::cerr << gai_strerror(rv) << "\n";
            exit(1);
        }

        for (p = servinfo; p != NULL; p = p->ai_next)
        {
            sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
            if (sockfd == -1)
            {
                error("socket",strerror(errno));
                continue;
            }
            if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, &yes,p->ai_addrlen) == -1)
            {
                std::cerr << "ERORR: setsockopt()\n";
                exit(1);
            }
            if (bind(sockfd,p->ai_addr,p->ai_addrlen) == -1)
            {
                error("bind",strerror(errno));
                close(sockfd);
                continue;
            }

            break;

        }
        if (p != NULL)
        {
            int rv;
            if ((rv = listen(sockfd, 2)) == -1)
                system_error("listen");
            if (verbose)
                std::cout << "listening on port " << port << "...\n";
            size = sizeof(clientaddr);
            if ((newfd = accept(sockfd, (struct sockaddr *) &clientaddr, &size)) == -1)
                system_error("accept");
            sockfd = newfd;
            if (verbose)
            {
                char s[INET6_ADDRSTRLEN];

                inet_ntop(clientaddr.ss_family,(struct sockaddr *)
                          get_in_addr((struct sockaddr *)&clientaddr),
                          s,sizeof(s));
                std::cout << "Got a connection from '" << s << "'.\n";
            }
            protohandler.serverside(newfd);
        }
        if (p == NULL)
        {
            std::cerr << "Unable to host\n";
            exit(1);
        }
    }
    break;
    case CLIENT:
    {
        memset(&hints,0,sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;


        if ((rv = getaddrinfo(host, port, &hints,&servinfo)) != 0)
        {
            std::cerr << gai_strerror(rv) << "\n";
            exit(1);
        }

        for (p = servinfo; p != NULL; p = p->ai_next)
        {
            sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
            if (sockfd == -1)
            {
                error("socket",strerror(errno));
                continue;
            }
            if (connect(sockfd,p->ai_addr,p->ai_addrlen) == -1)
            {
                error("connect",strerror(errno));
                close(sockfd);
                continue;
            } break;

        }
        if (p == NULL)
        {
            std::cerr << "Unable to connect\n";
            exit(1);
        }
    }
    break;
    default:
        error("network","Type is not correct, valid types are SERVER=1 CLIENT=2");

    }

    freeaddrinfo(servinfo);
}
