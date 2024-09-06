#include "protocol.h"

#include <stdlib.h>
#include <string.h>

PDU* mkPDU(uint uiMSGLen)
{

    uint uiPDULen=sizeof(PDU)+uiMSGLen;
    PDU * ptr = (PDU*)malloc(uiPDULen);
    if(ptr==NULL) exit(-1);

    memset(ptr,0,uiPDULen); //将协议数据单元的每一个字节的值都初始化为0

    ptr->uiPDULen=uiPDULen;
    ptr->uiMSgLen=uiMSGLen;

    return ptr;
}
