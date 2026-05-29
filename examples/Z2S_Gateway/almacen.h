/*if (millis() - printTime > 10000) {
    //Zigbee.scanNetworks();
    if (zbGateway.getGatewayDevices().size() > 0 ) {
      if (esp_zb_is_started()) {//&& esp_zb_lock_acquire(portMAX_DELAY)) {
        zbg_device_params_t *gt_device = zbGateway.getGatewayDevices().front();
	      log_i("short address before 0x%x",gt_device->short_addr);
        gt_device->short_addr = esp_zb_address_short_by_ieee(gt_device->ieee_addr);
        log_i("short address after 0x%x",gt_device->short_addr);
        if (counter == 0) {          
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x65;
          tuya_dp_data[3] = 0x01;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x01;
          
          //zbGateway.sendCustomClusterCmd(gt_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, 7, tuya_dp_data);
        }
        //if (counter == 1) {          
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x66;
          tuya_dp_data[3] = 0x02;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x04;
          tuya_dp_data[6] = 0x00;
          tuya_dp_data[7] = 0x00;
          tuya_dp_data[8] = 0x00;
          tuya_dp_data[9] = 0x00;
          //zbGateway.sendCustomClusterCmd(gt_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, 10, tuya_dp_data);
        //}
        if (counter == 2) {          
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x65;
          tuya_dp_data[3] = 0x01;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x00;
          
          //zbGateway.sendCustomClusterCmd(gt_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, 7, tuya_dp_data);
        }
        if (counter == 3) {          
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x6C;
          tuya_dp_data[3] = 0x01;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x01;
          
          //zbGateway.sendCustomClusterCmd(gt_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, 7, tuya_dp_data);
        }
        if (counter == 4) {          
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x6C;
          tuya_dp_data[3] = 0x01;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x02;
          
          //zbGateway.sendCustomClusterCmd(gt_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, 7, tuya_dp_data);
        }
        counter++; if(counter > 4) counter = 0;
        //zbGateway.sendAttributeWrite(gt_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
          //                          ESP_ZB_ZCL_ATTR_TYPE_U64,8, gt_device->ieee_addr);
        //zbGateway.sendIASzoneEnrollResponseCmd(gt_device, ESP_ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_CODE_SUCCESS, 120);
        //gt_device->short_addr = 0xFFFF;
        //zbGateway.sendAttributeRead(gt_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, true);
        //zbGateway.setClusterReporting(  gt_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
          //                 ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 0, 60, 1, true);
      }
   //esp_zb_lock_release();
  
    }
    printTime = millis();
  }*/

  /*if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 
                      0xF000/*ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_CAPABILITIES_ID, true))*/
                   // log_i("F0 0x%x, type 0x%x", *(uint16_t *)zbGateway.getReadAttrLastResult()->data.value, zbGateway.getReadAttrLastResult()->data.type);*/
                    //write_mask_16 = 0x01;
                    //zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF000, ESP_ZB_ZCL_ATTR_TYPE_U16, 2, &write_mask_16);
                    //write_mask = 1;
                    //zbGateway.sendCustomClusterCmd(joined_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask, true);                    
//zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, true);
                  //zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID, true);
                  //zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, true);


//zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
                    //                            0x0021, //ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID, 
                      //                          ESP_ZB_ZCL_ATTR_TYPE_U8, 0, 4*60*60, 1, true);

//*/
                  //esp_zb_ieee_addr_t addr;
                  //esp_zb_get_long_address(addr);
                  //zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
                  //     ESP_ZB_ZCL_ATTR_TYPE_U64,sizeof(esp_zb_ieee_addr_t),&addr);
                  //delay(200);
                  //zbGateway.sendIASzoneEnrollResponseCmd(joined_device, ESP_ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_CODE_SUCCESS, 120);
                  //delay(200);
                  //zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID, true);

