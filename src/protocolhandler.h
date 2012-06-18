#ifndef PROTOCOLHANDLER_H_
#define PROTOCOLHANDLER_H_


#include "global.h"

class protocolhandler
{
    short command;
public:
    void serverside(int sock);
    void clientside(int sock,const char * path,fileinfo uploadfile);
};
#endif
