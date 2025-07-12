#ifndef WEB_GUI_TEMPLATES_H_
#define WEB_GUI_TEMPLATES_H_

typedef struct zigbee_cluster_s {
	const char* zigbee_cluster_name;
	const uint16_t zigbee_cluster_id;
} zigbee_cluster_t;

typedef struct zigbee_attribute_s {
	const uint16_t zigbee_attribute_id;
	const uint16_t zigbee_attribute_cluster_id;
	const uint8_t zigbee_attribute_datatype_id;
	const char* zigbee_attribute_name;
} zigbee_attribute_t;

typedef struct zigbee_datatype_s {
	const char* zigbee_datatype_name;
	const uint8_t zigbee_datatype_size;
	const uint8_t zigbee_datatype_id;
} zigbee_datatype_t;

typedef struct zigbee_attribute_value_s {
	const uint16_t zigbee_attribute_id;
	const uint16_t zigbee_cluster_id;
	const char* zigbee_attribute_value_name;
	uint32_t zigbee_attribute_value;
} zigbee_attribute_value_t;

static const zigbee_cluster_t zigbee_clusters[] = 
	{{.zigbee_cluster_name = "BASIC",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_BASIC},
	 {.zigbee_cluster_name = "POWER CONFIG",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG},
	 {.zigbee_cluster_name = "IDENTIFY",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY},
	 {.zigbee_cluster_name = "GROUPS",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_GROUPS},
	 {.zigbee_cluster_name = "SCENES",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_SCENES},
	 {.zigbee_cluster_name = "ON/OFF",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF},
	 {.zigbee_cluster_name = "LEVEL CONTROL",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL},
	 {.zigbee_cluster_name = "TIME",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_TIME},
	 {.zigbee_cluster_name = "POLL CONTROLL",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL},
	 {.zigbee_cluster_name = "WINDOW COVERING",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING},
	 {.zigbee_cluster_name = "THERMOSTAT",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT},					
	 {.zigbee_cluster_name = "COLOR CONTROL",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL},																	
	 {.zigbee_cluster_name = "ILLUMINANCE MEASUREMENT",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT},
	 {.zigbee_cluster_name = "TEMPERATURE MEASUREMENT",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT},
	 {.zigbee_cluster_name = "PRESSURE MEASUREMENT",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT},
	 {.zigbee_cluster_name = "FLOW MEASUREMENT",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT},
	 {.zigbee_cluster_name = "HUMIDITY MEASUREMENT",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT},
	 {.zigbee_cluster_name = "OCCUPANCY SENSING",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING},
	 {.zigbee_cluster_name = "IAS ZONE",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE},
	 {.zigbee_cluster_name = "IAS WD",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_WD},
	 {.zigbee_cluster_name = "SIMPLE METERING",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_METERING},
	 {.zigbee_cluster_name = "ELECTRICAL MEASUREMENT",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT},
	 {.zigbee_cluster_name = "TUYA E000",.zigbee_cluster_id = 0xE000},
	 {.zigbee_cluster_name = "TUYA E001",.zigbee_cluster_id = 0xE001},
	 {.zigbee_cluster_name = "TUYA EF00",.zigbee_cluster_id = 0xEF00},
	 {.zigbee_cluster_name = "SONOFF FC11",.zigbee_cluster_id = 0xFC11}};

static const zigbee_attribute_t zigbee_attributes[] = {

	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID, 
		.zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "BATTERY VOLTAGE"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "BATTERY PERCENTAGE"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_BOOL,
		.zigbee_attribute_name = "ON/OFF STATE"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ON_OFF_ON_TIME, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "ON TIME"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ON_OFF_OFF_WAIT_TIME, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "OFF WAIT TIME"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "STARTUP ON/OFF"
	},
	{
		.zigbee_attribute_id  = 0x8002, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "TUYA STARTUP ON/OFF"
	},
	{
		.zigbee_attribute_id  = 0x8001, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "TUYA INDICATOR STATUS"
	},
	{
		.zigbee_attribute_id  = 0x5000, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "TUYA BACKLIGHT"
	},
	{
		.zigbee_attribute_id  = 0x0, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_NULL,
		.zigbee_attribute_name = "OFF (CMD)"
	},
	{
		.zigbee_attribute_id  = 0x1, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_NULL,
		.zigbee_attribute_name = "ON (CMD)"
	},
	{
		.zigbee_attribute_id  = 0x2, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_NULL,
		.zigbee_attribute_name = "TOGGLE (CMD)"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U32,
		.zigbee_attribute_name = "CURRENT LEVEL"
	},
	{
		.zigbee_attribute_id  = 0x0000, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U32,
		.zigbee_attribute_name = "CHECK-IN INTERVAL"
	},
	{
		.zigbee_attribute_id  = 0x0001, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U32,
		.zigbee_attribute_name = "LONG POLL INTERVAL"
	},
	{
		.zigbee_attribute_id  = 0x0002, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "SHORT POLL INTERVAL"
	},
	{
		.zigbee_attribute_id  = 0x0003, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "FAST POLL TIMEOUT"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_LIFT_PERCENTAGE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "LIFT CURRENT POSITION (%)"
	},
	{
		.zigbee_attribute_id  = 0xF000, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "TUYA CURTAIN SWITCH"
	},
	{
		.zigbee_attribute_id  = 0xF001, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "TUYA CALIBRATION MODE"
	},
	{
		.zigbee_attribute_id  = 0xF002, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "TUYA MOTOR"
	},
	{
		.zigbee_attribute_id  = 0xF003, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "TUYA CALIBRATION TIME (S)"
	},
	{
		.zigbee_attribute_id  = 0x00, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_NULL,
		.zigbee_attribute_name = "OPEN (CMD)"
	},
	{
		.zigbee_attribute_id  = 0x01, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_NULL,
		.zigbee_attribute_name = "CLOSE (CMD)"
	},
	{
		.zigbee_attribute_id  = 0x02, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_NULL,
		.zigbee_attribute_name = "STOP (CMD)"
	},
	{
		.zigbee_attribute_id  = 0x03, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_NULL,
		.zigbee_attribute_name = "CONTINUE (CMD)"
	},
	{
		.zigbee_attribute_id  = 0x05, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "MOVE TO POSITION (%)"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "RMS VOLTAGE"
	},
	{	.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "RMS CURRENT"
	 },
	 {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "RMS ACTIVE POWER"
	 },
};
	 
	static const zigbee_datatype_t zigbee_datatypes[] = 

	{{.zigbee_datatype_name = "NULL",.zigbee_datatype_size = 0x00,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_NULL},
	 {.zigbee_datatype_name = "BOOL",.zigbee_datatype_size = 0x01,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_BOOL},
	 {.zigbee_datatype_name = "8BITMAP",.zigbee_datatype_size = 0x01,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BITMAP},
	 {.zigbee_datatype_name = "16BITMAP",.zigbee_datatype_size = 0x02,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_16BITMAP},
	 {.zigbee_datatype_name = "U8",.zigbee_datatype_size = 0x01,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8},
	 {.zigbee_datatype_name = "U16",.zigbee_datatype_size = 0x02,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16},
	 {.zigbee_datatype_name = "U32",.zigbee_datatype_size = 0x02,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U32},
	 {.zigbee_datatype_name = "S8",.zigbee_datatype_size = 0x01,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S8},
	 {.zigbee_datatype_name = "S16",.zigbee_datatype_size = 0x02,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16},
	 {.zigbee_datatype_name = "8ENUM",.zigbee_datatype_size = 0x01,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM},
	 {.zigbee_datatype_name = "16ENUM",.zigbee_datatype_size = 0x02,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_16BIT_ENUM},
	 {.zigbee_datatype_name = "OSTRING",.zigbee_datatype_size = 0x00,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_OCTET_STRING},
	 {.zigbee_datatype_name = "LSTRING",.zigbee_datatype_size = 0x00,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING},
	 {.zigbee_datatype_name = "ARRAY",.zigbee_datatype_size = 0x00,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_ARRAY},
	 {.zigbee_datatype_name = "SET",.zigbee_datatype_size = 0x00,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_SET}
	};

static const zigbee_attribute_value_t zigbee_attribute_values [] = {

	{
		.zigbee_attribute_id = ESP_ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
		.zigbee_attribute_value_name = "OFF",
		.zigbee_attribute_value = 0
	},
	{
		.zigbee_attribute_id = ESP_ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
		.zigbee_attribute_value_name = "ON",
		.zigbee_attribute_value = 1
	},
	{
		.zigbee_attribute_id = ESP_ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
		.zigbee_attribute_value_name = "TOGGLE",
		.zigbee_attribute_value = 2
	},
	{
		.zigbee_attribute_id = ESP_ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
		.zigbee_attribute_value_name = "RESTORE",
		.zigbee_attribute_value = 0xFF
	},
	{
		.zigbee_attribute_id = 0xF001,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,
		.zigbee_attribute_value_name = "START",
		.zigbee_attribute_value = 0x00
	},
	{
		.zigbee_attribute_id = 0xF001,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,
		.zigbee_attribute_value_name = "STOP",
		.zigbee_attribute_value = 0x01
	},
	{
		.zigbee_attribute_id = 0xF002,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,
		.zigbee_attribute_value_name = "DEFAULT",
		.zigbee_attribute_value = 0x00
	},
	{
		.zigbee_attribute_id = 0xF002,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,
		.zigbee_attribute_value_name = "REVERSE",
		.zigbee_attribute_value = 0x01
	}
};

#endif //WEB_GUI_TEMPLATES_H_