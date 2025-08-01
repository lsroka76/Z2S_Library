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

static const zigbee_cluster_t zigbee_clusters[] PROGMEM = 
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
   {.zigbee_cluster_name = "THERMOSTAT UI",.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG},					
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

static const zigbee_attribute_t zigbee_attributes[] PROGMEM = {
//POWER CONFIG
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
  //ON OFF
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
  //LEVEL CONTROL
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "CURRENT LEVEL"
	},
  //POLL CONTROL
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
  //WINDOW COVERING
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
  //THERMOSTAT
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "LOCAL TEMPERATURE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S8,
		.zigbee_attribute_name = "TEMPERATURE CALIBRATION"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "HEATING SETPOINT"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "SYSTEM MODE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_THERMOSTAT_RUNNING_MODE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "RUNNING MODE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "TEMPERATURE UNITS"
	},
  //COLOR CONTROL
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_HUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "CURRENT HUE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_SATURATION_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "CURRENT SATURATION"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_X_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "CURRENT X"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_Y_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "CURRENT Y"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMPERATURE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "CURRENT Y"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_MODE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "COLOR MODE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_COLOR_CONTROL_OPTIONS_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BITMAP,
		.zigbee_attribute_name = "COLOR OPTIONS"
	},
  //ILLUINANCE/TEMPERATURE/PRESSURE/FLOW/HUMIDITY
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "ILLUMINANCE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MIN_MEASURED_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "MIN VALUE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MAX_MEASURED_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "MAX VALUE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "TEMPERATURE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "MIN VALUE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "MAX VALUE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "PRESSURE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "MIN VALUE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "MAX VALUE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_FLOW_MEASUREMENT_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "FLOW"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_FLOW_MEASUREMENT_MIN_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "MIN VALUE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_FLOW_MEASUREMENT_MAX_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "MAX VALUE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "HUMIDITY"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_MIN_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "MIN VALUE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_MAX_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "MAX VALUE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BITMAP,
		.zigbee_attribute_name = "OCCUPANCY"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "SENSOR TYPE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_BITMAP_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BITMAP,
		.zigbee_attribute_name = "SENSOR TYPE BITMAP"
	},
  //IAS ZONE
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "ZONE STATE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_16BIT_ENUM,
		.zigbee_attribute_name = "ZONE TYPE"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_16BITMAP,
		.zigbee_attribute_name = "ZONE STATUS"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONEID_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "ZONE ID"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_IAS_ZONE_NUMBER_OF_ZONE_SENSITIVITY_LEVELS_SUPPORTED_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "SENSITIVITY LEVELS"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_IAS_ZONE_CURRENT_ZONE_SENSITIVITY_LEVEL_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "SENSITIVITY LEVEL"
	},
  //IAS WD
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_IAS_WD_MAX_DURATION_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_WD, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "MAX DURATION"
	},
	{
		.zigbee_attribute_id  = 0x0001, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_WD, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "TUYA SIREN ALARM LIGHT"
	},
	{
		.zigbee_attribute_id  = 0x0002, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_WD, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "TUYA SIREN ALARM RADIO"
	},
	{
		.zigbee_attribute_id  = 0xF000, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_WD, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "PROMPT"
	},
  //METERING
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_METERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U48,
		.zigbee_attribute_name = "CURRENT SUMMATION"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_METERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM,
		.zigbee_attribute_name = "UNIT OF MEASURE"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_METERING_MULTIPLIER_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_METERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U24,
		.zigbee_attribute_name = "MULTIPLIER"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_METERING_DIVISOR_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_METERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U24,
		.zigbee_attribute_name = "DIVISOR"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_METERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BITMAP,
		.zigbee_attribute_name = "FORMATTING"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_METERING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BITMAP,
		.zigbee_attribute_name = "DEVICE TYPE"
	},
  //ELECTRICAL MESUREMENT
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "AC FREQUENCY"
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
		.zigbee_attribute_name = "ACTIVE POWER"
	 },
	 {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_POWER_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "REACTIVE POWER"
	 },
	 {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "APPARENT POWER"
	 },
	 {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_FACTOR_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S8,
		.zigbee_attribute_name = "POWER FACTOR"
	 },
	 {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_FACTOR_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S8,
		.zigbee_attribute_name = "POWER FACTOR"
	 },
	 {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_MULTIPLIER_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "AC VOLTAGE MULTIPLIER"
	 },
	 {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_DIVISOR_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "AC VOLTAGE DIVISOR"
	 },
	 {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_MULTIPLIER_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "AC CURRENT MULTIPLIER"
	 },
	 {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_DIVISOR_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "AC CURRENT DIVISOR"
	 },
	 {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_MULTIPLIER_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "AC POWER MULTIPLIER"
	 },
	 {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_DIVISOR_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "AC POWER DIVISOR"
	 },
};
	 
	static const zigbee_datatype_t zigbee_datatypes[] PROGMEM = 

	{{.zigbee_datatype_name = "NULL",.zigbee_datatype_size = 0x00,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_NULL},
	 {.zigbee_datatype_name = "BOOL",.zigbee_datatype_size = 0x01,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_BOOL},
	 {.zigbee_datatype_name = "8BITMAP",.zigbee_datatype_size = 0x01,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BITMAP},
	 {.zigbee_datatype_name = "16BITMAP",.zigbee_datatype_size = 0x02,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_16BITMAP},
	 {.zigbee_datatype_name = "U8",.zigbee_datatype_size = 0x01,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8},
	 {.zigbee_datatype_name = "U16",.zigbee_datatype_size = 0x02,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16},
	 {.zigbee_datatype_name = "U24",.zigbee_datatype_size = 0x03,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U24},
	 {.zigbee_datatype_name = "U32",.zigbee_datatype_size = 0x04,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U32},
   {.zigbee_datatype_name = "U48",.zigbee_datatype_size = 0x06,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U48},
	 {.zigbee_datatype_name = "S8",.zigbee_datatype_size = 0x01,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S8},
	 {.zigbee_datatype_name = "S16",.zigbee_datatype_size = 0x02,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16},
	 {.zigbee_datatype_name = "8ENUM",.zigbee_datatype_size = 0x01,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM},
	 {.zigbee_datatype_name = "16ENUM",.zigbee_datatype_size = 0x02,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_16BIT_ENUM},
	 {.zigbee_datatype_name = "OSTRING",.zigbee_datatype_size = 0x00,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_OCTET_STRING},
	 {.zigbee_datatype_name = "LSTRING",.zigbee_datatype_size = 0x00,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING},
	 {.zigbee_datatype_name = "ARRAY",.zigbee_datatype_size = 0x00,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_ARRAY},
	 {.zigbee_datatype_name = "SET",.zigbee_datatype_size = 0x00,.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_SET}
	};

static const zigbee_attribute_value_t zigbee_attribute_values [] PROGMEM = {

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