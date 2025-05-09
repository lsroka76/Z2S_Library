//#include "Arduino.h"     
#include <stdint.h>
#include "send_default_resp.h"
#include <zboss_api.h>
#include <zboss_api_nwk.h>
#include <zboss_api_buf.h>
#include <zboss_api_af.h>
#include <zcl/zb_zcl_common.h>
#include <zboss_api_zcl.h>
    
void sendDefaultResponse(uint8_t command_id, uint8_t tsn, uint16_t short_addr, uint8_t endpoint, uint16_t profile, uint16_t cluster) {
    zb_bufid_t bufferDefaultResponse;
    zb_uint8_t *cmd_ptr;

    bufferDefaultResponse = (zb_bufid_t) zb_buf_get_out();
    
    cmd_ptr = (zb_uint8_t*) ZB_ZCL_START_PACKET(bufferDefaultResponse);

    ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL_EXT(cmd_ptr, ZB_TRUE, 0);
    
    ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, command_id);
    ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, ESP_OK);
    //*(cmd_ptr++) = command.id;
    //*(cmd_ptr++) = ESP_OK;

    ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(cmd_ptr, tsn, ZB_TRUE, 0x1002, ZB_ZCL_CMD_DEFAULT_RESP);
 
    ZB_ZCL_FINISH_PACKET((bufferDefaultResponse), cmd_ptr)                                                                \
    ZB_ZCL_SEND_COMMAND_SHORT((bufferDefaultResponse), short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, 
                              endpoint, 1, profile, cluster, NULL);
 
    zb_buf_free(bufferDefaultResponse);
}