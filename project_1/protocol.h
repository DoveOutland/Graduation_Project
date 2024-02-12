#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdint.h>
#define MAX_LEN 256

typedef struct
{
    char frame_header;       /*帧头*/
    char length;             /*长度*/
    uint8_t HighDataBit;        /*数据高位*/
    uint8_t LowDataBit;         /*数据低位*/
    char frame_total;        /*帧尾*/
}Sensor_Data;

#endif // PROTOCOL_H
