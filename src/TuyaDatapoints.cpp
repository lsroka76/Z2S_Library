#include "TuyaDatapoints.h"

bool sendTuyaRequestCmdBool(ZigbeeGateway *gateway, 
                            zbg_device_params_t *device, 
                            uint8_t dp_id, 
                            bool dp_value,
                            bool cmd_sync) {

  Tuya_dp_zcl_payload_t Tuya_dp_zcl_payload;
        
  makeTuyaDPBool(Tuya_dp_zcl_payload, dp_id, dp_value ? 1 : 0);
  
  return gateway->sendCustomClusterCmd(device,
                                       TUYA_PRIVATE_CLUSTER_EF00, 
                                       TUYA_REQUEST_CMD, 
                                       ESP_ZB_ZCL_ATTR_TYPE_SET, 
                                       TUYA_DP_ZCL_PAYLOAD_8, 
                                       (uint8_t*)&Tuya_dp_zcl_payload, 
                                       cmd_sync);
}

bool sendTuyaRequestCmdEnum8(ZigbeeGateway *gateway, 
                             zbg_device_params_t *device, 
                             uint8_t dp_id, 
                             uint8_t dp_value,
                             bool cmd_sync) {

  Tuya_dp_zcl_payload_t Tuya_dp_zcl_payload;
        
  makeTuyaDPEnum8(Tuya_dp_zcl_payload, dp_id, dp_value);
  
  return  gateway->sendCustomClusterCmd(device,
                                        TUYA_PRIVATE_CLUSTER_EF00, 
                                        TUYA_REQUEST_CMD, 
                                        ESP_ZB_ZCL_ATTR_TYPE_SET, 
                                        TUYA_DP_ZCL_PAYLOAD_8, 
                                        (uint8_t*)&Tuya_dp_zcl_payload, 
                                        cmd_sync);
}

bool sendTuyaRequestCmdValue32(ZigbeeGateway *gateway, 
                               zbg_device_params_t *device,
                               uint8_t dp_id, 
                               uint32_t dp_value,
                               bool cmd_sync) {

  Tuya_dp_zcl_payload_t Tuya_dp_zcl_payload;
        
  makeTuyaDPValue32(Tuya_dp_zcl_payload, dp_id, dp_value);
  
  return  gateway->sendCustomClusterCmd(device,
                                        TUYA_PRIVATE_CLUSTER_EF00, 
                                        TUYA_REQUEST_CMD, 
                                        ESP_ZB_ZCL_ATTR_TYPE_SET, 
                                        TUYA_DP_ZCL_PAYLOAD_32, 
                                        (uint8_t*)&Tuya_dp_zcl_payload, 
                                        cmd_sync);
}

bool sendTuyaRequestCmdData(ZigbeeGateway *gateway, 
                            zbg_device_params_t *device,
                            uint8_t dp_id,
                            uint8_t dp_type, 
                            uint32_t dp_value,
                            bool cmd_sync) {

  switch (dp_type) {


    case TUYA_DP_TYPE_BOOL:

      return
        sendTuyaRequestCmdBool(gateway, device, dp_id, dp_value, cmd_sync); break;


    case TUYA_DP_TYPE_VALUE:

      return
        sendTuyaRequestCmdValue32(gateway, device, dp_id, dp_value, cmd_sync); break;


    case TUYA_DP_TYPE_ENUM:

      return
        sendTuyaRequestCmdEnum8(gateway, device, dp_id, dp_value, cmd_sync); break;


    default:

      log_e("unsupported Tuya datapoint %02x type %02x", dp_id, dp_type); break;
  }
  return false;
}
