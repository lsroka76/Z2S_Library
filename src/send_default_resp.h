//#pragma once
//#ifdef __cplusplus
//extern "C" {
//#endif
#ifndef _SEND_DEFAULT_RESP_H
#define _SEND_DEFAULT_RESP_H 1

#ifdef __cplusplus
extern "C" {
#endif
    
void sendDefaultResponse(uint8_t command_id, uint8_t tsn, uint16_t short_addr, uint8_t endpoint, uint16_t profile, uint16_t cluster);


#ifdef __cplusplus
}
#endif

#endif
//#ifdef __cplusplus
//}
//#endif