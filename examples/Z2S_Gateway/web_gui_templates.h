#ifndef WEB_GUI_TEMPLATES_H_
#define WEB_GUI_TEMPLATES_H_

#include <supla/actions.h>
#include <supla/events.h>

enum ActionGUIState {

	VIEW_ACTION,
	NEW_ACTION,
	EDIT_ACTION,
	SAVE_ACTION,
	CANCEL_ACTION,
	REMOVE_ACTION,
};

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
	const uint32_t zigbee_attribute_value;
} zigbee_attribute_value_t;


typedef struct Tuya_datapoint_type_s {

	const char* Tuya_datapoint_type_name;
	const uint8_t Tuya_datapoint_type_id;
	const uint16_t Tuya_datapoint_type_length;
} Tuya_datapoint_type_t;


typedef struct Tuya_datapoint_desc_s {

  uint32_t    Tuya_datapoint_desc_id;
  uint32_t    z2s_device_desc_id;
  uint8_t     Tuya_datapoint_id;
  uint8_t     Tuya_datapoint_type;
  uint16_t    Tuya_datapoint_length;
  const char *Tuya_datapoint_name;
  const char *Tuya_datapoint_description;
} Tuya_datapoint_desc_t;


typedef struct zigbee_manufacturer_code_s {

	const uint16_t manufacturer_code;
	const char* manufacturer_name;
} zigbee_manufacturer_code_t;


typedef struct Supla_action_type_s {

	const Supla::Action	Supla_action_id;
	const char* Supla_action_name;
} Supla_action_type_t;


typedef struct Supla_event_type_s {

	const Supla::Event Supla_event_id;
	const char* Supla_event_name;
} Supla_event_type_t;


typedef struct Supla_condition_type_s {

	const Supla::Conditions Supla_condition_id;
	const char* Supla_condition_name;
} Supla_condition_type_t;

static constexpr char* GUI_MODE_OPTIONS[] PROGMEM = {

	"No GUI on start",
	"Minimal GUI (Gateway + Credentials)",
	"Standard GUI (Minimal + Devices + Channels)",
	"Extended GUI (Standard + Clusters&Attributes)",
	"Full GUI",
	"Developer GUI (Gateway + C&A + AD +TCC)"
};

static constexpr zigbee_cluster_t zigbee_clusters[] PROGMEM = {

	{ .zigbee_cluster_name = "BASIC",										
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_BASIC },

	{ .zigbee_cluster_name = "POWER CONFIG",						
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG },

	{ .zigbee_cluster_name = "IDENTIFY",								
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY },

	{ .zigbee_cluster_name = "GROUPS",									
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_GROUPS },

	{ .zigbee_cluster_name = "SCENES",									
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_SCENES },

	{ .zigbee_cluster_name = "ON/OFF",									
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF },

	{ .zigbee_cluster_name = "LEVEL CONTROL",						
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL },

	{ .zigbee_cluster_name = "MULTISTATE INPUT",				
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_MULTI_INPUT },

	{ .zigbee_cluster_name = "ANALOG INPUT",				
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT },

	{ .zigbee_cluster_name = "TIME",										
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_TIME },

	{ .zigbee_cluster_name = "POLL CONTROL",						
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL },

	{ .zigbee_cluster_name = "WINDOW COVERING",					
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING },

	{ .zigbee_cluster_name = "THERMOSTAT",							
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT },

	{ .zigbee_cluster_name = "THERMOSTAT UI",						
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG },	

	{ .zigbee_cluster_name = "COLOR CONTROL",						
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL },			

	{ .zigbee_cluster_name = "ILLUMINANCE MEASUREMENT",	
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT },

	{ .zigbee_cluster_name = "TEMPERATURE MEASUREMENT",	
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT },

	{ .zigbee_cluster_name = "PRESSURE MEASUREMENT",		
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT },

	{ .zigbee_cluster_name = "FLOW MEASUREMENT",				
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT },

	{ .zigbee_cluster_name = "HUMIDITY MEASUREMENT",		
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT },

	{ .zigbee_cluster_name = "OCCUPANCY SENSING",				
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING },

	{ .zigbee_cluster_name = "IAS ZONE",								
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE },

	{ .zigbee_cluster_name = "IAS WD",									
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_IAS_WD },

	{ .zigbee_cluster_name = "SIMPLE METERING",					
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_METERING },

	{ .zigbee_cluster_name = "ELECTRICAL MEASUREMENT",	
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT },
	
	{ .zigbee_cluster_name = "TUYA (0xE000)",						
		.zigbee_cluster_id = TUYA_PRIVATE_CLUSTER_0 },
	
	{ .zigbee_cluster_name = "TUYA (0xE001)",						
		.zigbee_cluster_id = TUYA_PRIVATE_CLUSTER_1 },
	
	{ .zigbee_cluster_name = "TUYA (0xEF00)",						
		.zigbee_cluster_id = TUYA_PRIVATE_CLUSTER_EF00 },
	
	{ .zigbee_cluster_name = "DEVELCO (0xFC03)",				
		.zigbee_cluster_id = DEVELCO_CUSTOM_CLUSTER },
	
	{ .zigbee_cluster_name = "SONOFF (0xFC11)",					
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER },
	
	{ .zigbee_cluster_name = "LUMI (0xFCC0)",						
		.zigbee_cluster_id = LUMI_CUSTOM_CLUSTER }
};

static constexpr zigbee_datatype_t zigbee_datatypes[] PROGMEM = 

	{{ .zigbee_datatype_name = "NULL",			
		 .zigbee_datatype_size = 0x00,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_NULL },

	 { .zigbee_datatype_name = "BOOL",			
	   .zigbee_datatype_size = 0x01,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_BOOL },

	 { .zigbee_datatype_name = "8BITMAP",	
	 	 .zigbee_datatype_size = 0x01,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BITMAP },

	 { .zigbee_datatype_name = "16BITMAP",	
	 	 .zigbee_datatype_size = 0x02,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_16BITMAP },

	 { .zigbee_datatype_name = "U8",				
	  .zigbee_datatype_size = 0x01,	
		.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8 },
	 
	 { .zigbee_datatype_name = "U16",			
	   .zigbee_datatype_size = 0x02,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16 },

	 { .zigbee_datatype_name = "U24",			
	  .zigbee_datatype_size = 0x03,	
		.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U24 },

	 { .zigbee_datatype_name = "U32",			
	  .zigbee_datatype_size = 0x04,	
		.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U32 },

   { .zigbee_datatype_name = "U48",			
	  .zigbee_datatype_size = 0x06,	
		.zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U48 },

	 { .zigbee_datatype_name = "S8",				
	   .zigbee_datatype_size = 0x01,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S8 },

	 { .zigbee_datatype_name = "S16",			
	   .zigbee_datatype_size = 0x02,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16 },

	 { .zigbee_datatype_name = "8ENUM",		
	   .zigbee_datatype_size = 0x01,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM },

	 { .zigbee_datatype_name = "16ENUM",		
	   .zigbee_datatype_size = 0x02,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_16BIT_ENUM },

	 { .zigbee_datatype_name = "OSTRING",	
	   .zigbee_datatype_size = 0x00,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_OCTET_STRING },

	 { .zigbee_datatype_name = "LSTRING",	
	   .zigbee_datatype_size = 0x00,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING },

	 { .zigbee_datatype_name = "ARRAY",		
	   .zigbee_datatype_size = 0x00,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_ARRAY },

	 { .zigbee_datatype_name = "SET",			
	   .zigbee_datatype_size = 0x00,	
		 .zigbee_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_SET }
	};

static constexpr Tuya_datapoint_type_t Tuya_datapoint_types[] PROGMEM = 
	
	{{ .Tuya_datapoint_type_name = "RAW", 		
		 .Tuya_datapoint_type_id = 0x00, 
		 .Tuya_datapoint_type_length = 0x00 },
	
	 { .Tuya_datapoint_type_name = "BOOL", 	
	 	 .Tuya_datapoint_type_id = 0x01, 
		 .Tuya_datapoint_type_length = 0x01 },

	 { .Tuya_datapoint_type_name = "VALUE", 	
	 	 .Tuya_datapoint_type_id = 0x02, 
		 .Tuya_datapoint_type_length = 0x04 },

	 { .Tuya_datapoint_type_name = "STRING", 
	 	 .Tuya_datapoint_type_id = 0x03, 
		 .Tuya_datapoint_type_length = 0x00 },

	 { .Tuya_datapoint_type_name = "ENUM", 	
	 	 .Tuya_datapoint_type_id = 0x04, 
		 .Tuya_datapoint_type_length = 0x01 },

	 { .Tuya_datapoint_type_name = "BITMAP", 
	 	 .Tuya_datapoint_type_id = 0x05, 
		 .Tuya_datapoint_type_length = 0x01 }
	};

static constexpr zigbee_manufacturer_code_t zigbee_manufacturer_codes[] PROGMEM = {

	 { .manufacturer_code = PHILIPS_MANUFACTURER_CODE,	
		 .manufacturer_name = "PHILIPS" },

	 { .manufacturer_code = LUMI_MANUFACTURER_CODE,		
	 	 .manufacturer_name = "LUMI" },

	 { .manufacturer_code = DEVELCO_MANUFACTURER_CODE,	
	 	 .manufacturer_name = "DEVELCO" },

	 { .manufacturer_code = 0x1286,										
	 	 .manufacturer_name = "SHENZHEN_COOLKIT_TECHNOLOGY_CO_LTD" }
	};

static constexpr Supla_action_type_t Supla_actions [] PROGMEM = {
	
	{ .Supla_action_id = Supla::TURN_ON,
		.Supla_action_name = "TURN ON" },

	{ .Supla_action_id = Supla::TURN_OFF,
		.Supla_action_name = "TURN OFF" },

	{ .Supla_action_id = Supla::TOGGLE,
		.Supla_action_name = "TOGGLE" },

	{ .Supla_action_id = Supla::SET,
		.Supla_action_name = "SET" },

	{ .Supla_action_id = Supla::CLEAR,
		.Supla_action_name = "CLEAR" },

	{ .Supla_action_id = Supla::OPEN,
		.Supla_action_name = "OPEN" },

	{ .Supla_action_id = Supla::CLOSE,
		.Supla_action_name = "CLOSE" },

	{ .Supla_action_id = Supla::STOP,
		.Supla_action_name = "STOP" },

	{ .Supla_action_id = Supla::BRIGHTEN_ALL,
		.Supla_action_name = "BRIGHTEN ALL" },

	{ .Supla_action_id = Supla::DIM_ALL,
		.Supla_action_name = "DIM ALL" },

	{ .Supla_action_id = Supla::ENTER_CONFIG_MODE,
		.Supla_action_name = "ENTER CONFIG MODE" },

	{ .Supla_action_id = Supla::SOFT_RESTART,
		.Supla_action_name = "SOFT RESTART" },

	{ .Supla_action_id = Supla::INCREASE_TEMPERATURE,
		.Supla_action_name = "INCREASE TEMPERATURE" },

	{ .Supla_action_id = Supla::DECREASE_TEMPERATURE,
		.Supla_action_name = "DECREASE TEMPERATURE" },

	{ .Supla_action_id = Supla::TOGGLE_OFF_MANUAL_WEEKLY_SCHEDULE_MODES,
		.Supla_action_name = "TOGGLE_OFF/MANUAL/WEEKLY SCHEDULE MODES" },

	{ .Supla_action_id = Supla::SWITCH_TO_MANUAL_MODE,
		.Supla_action_name = "SWITCH TO MANUAL MODE" },

	{ .Supla_action_id = Supla::SWITCH_TO_WEEKLY_SCHEDULE_MODE,
		.Supla_action_name = "SWITCH TO WEEKLY SCHEDULE MODE" }
};

static constexpr Supla_event_type_t Supla_events [] PROGMEM = {
		
	{ .Supla_event_id = Supla::ON_CHANGE,
		.Supla_event_name =  "ON CHANGE" },

	{ .Supla_event_id = Supla::ON_CLICK_1,
		.Supla_event_name =  "ON CLICK 1" },

	{ .Supla_event_id = Supla::ON_TURN_ON,
		.Supla_event_name =  "ON TURN ON" },

	{ .Supla_event_id = Supla::ON_TURN_OFF,
		.Supla_event_name =  "ON TURN OFF" },

	{ .Supla_event_id = Supla::ON_OPEN,
		.Supla_event_name =  "ON VALVE OPEN" },

	{ .Supla_event_id = Supla::ON_CLOSE,
		.Supla_event_name =  "ON VALVE CLOSE" },

	{ .Supla_event_id = Supla::ON_HVAC_HEATING,
		.Supla_event_name =  "ON HVAC HEATING" },

	{ .Supla_event_id = Supla::ON_HVAC_STANDBY,
		.Supla_event_name =  "ON HVAC STANDBY" },

	{ .Supla_event_id = Supla::ON_HVAC_WEEKLY_SCHEDULE_ENABLED,
		.Supla_event_name =  "ON HVAC WEEKLY SCHEDULE ENABLED" },

	{ .Supla_event_id = Supla::ON_HVAC_WEEKLY_SCHEDULE_DISABLED,
		.Supla_event_name =  "ON HVAC WEEKLY SCHEDULE DISABLED" },

	{ .Supla_event_id = Supla::ON_HVAC_MODE_OFF,
		.Supla_event_name =  "ON HVAC MODE OFF" },

	{ .Supla_event_id = Supla::ON_HVAC_MODE_HEAT,
		.Supla_event_name =  "ON HVAC MODE HEAT" }
};

static constexpr Supla_condition_type_t Supla_conditions [] PROGMEM = {
	
	{ .Supla_condition_id = Supla::ON_LESS,
		.Supla_condition_name =  "ON LESS" },

	{ .Supla_condition_id = Supla::ON_LESS_EQ,
		.Supla_condition_name =  "ON LESS EQ" },

	{ .Supla_condition_id = Supla::ON_GREATER,
		.Supla_condition_name =  "ON GREATER" },

	{ .Supla_condition_id = Supla::ON_GREATER_EQ,
		.Supla_condition_name =  "ON GREATER EQ" },

	{ .Supla_condition_id = Supla::ON_BETWEEN,
		.Supla_condition_name =  "ON BETWEEN" },

	{ .Supla_condition_id = Supla::ON_BETWEEN_EQ,
		.Supla_condition_name =  "ON BETWEEN EQ" },

	{ .Supla_condition_id = Supla::ON_EQUAL,
		.Supla_condition_name =  "ON EQUAL" }
};

static constexpr zigbee_attribute_t zigbee_attributes[] PROGMEM = {

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
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_RUNNING_STATE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_16BITMAP,
		.zigbee_attribute_name = "RUNNING STATE"
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
		.zigbee_attribute_name = "TEMPERATURE [°C/x100]"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "MIN VALUE [°C/x100]"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "MAX VALUE [°C/x100]"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "PRESSURE [kPa/x10]"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "MIN VALUE [kPa/x10]"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "MAX VALUE [kPa/x10]"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_FLOW_MEASUREMENT_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "FLOW [m³/h/x10]"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_FLOW_MEASUREMENT_MIN_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "MIN VALUE [m³/h/x10]"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_FLOW_MEASUREMENT_MAX_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "MAX VALUE [m³/h/x10]"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "HUMIDITY [%/x100]"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_MIN_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "MIN VALUE [%/x100]"
	},
  {
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_MAX_VALUE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "MAX VALUE [%/x100]"
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

	{
		.zigbee_attribute_id  = 0x0020, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "SNZB-03P MOTION TIMEOUT [5-60s]"
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
		.zigbee_attribute_name = "CURRENT SUMMATION [kWh]"
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
		.zigbee_attribute_name = "AC FREQUENCY [Hz]"
	},
	{
		.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "RMS VOLTAGE [V]"
	},
	{	.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID, 
	  .zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "RMS CURRENT [A]"
	},
	{
	.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, 
	.zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
	.zigbee_attribute_name = "ACTIVE POWER [W]"
	},
	{
	.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_POWER_ID, 
	.zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
	.zigbee_attribute_name = "REACTIVE POWER [W]"
	},
	{
	.zigbee_attribute_id  = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_ID, 
	.zigbee_attribute_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
	.zigbee_attribute_name = "APPARENT POWER [W]"
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
	
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_CHILD_LOCK_ID, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_BOOL,
	.zigbee_attribute_name = "CHILD LOCK (TRVZB)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_TURBO_MODE_ID, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
	.zigbee_attribute_name = "TURBO MODE (ZBMINI, ZBMICRO)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_OPEN_WINDOW_ID, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_BOOL,
	.zigbee_attribute_name = "OPEN WINDOW (TRVZB)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_FROST_PROTECTION_TEMPERATURE_ID, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
	.zigbee_attribute_name = "FROST PROTECTION [4°C...35°C/0.5°C/x100] (TRVZB)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_IDLE_STEPS_ID, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
	.zigbee_attribute_name = "IDLE STEPS (TRVZB)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_CLOSING_STEPS_ID, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
	.zigbee_attribute_name = "CLOSING STEPS (TRVZB)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_VALVE_OPENING_LIMIT_VOLTAGE, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
	.zigbee_attribute_name = "VALVE OPENING LIMIT VOLTAGE [mV] (TRVZB)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_VALVE_CLOSING_LIMIT_VOLTAGE, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
	.zigbee_attribute_name = "VALVE CLOSING LIMIT VOLTAGE [mV] (TRVZB)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_VALVE_MOTOR_RUNNING_VOLTAGE, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
	.zigbee_attribute_name = "VALVE MOTOR RUNNING VOLTAGE [mV] (TRVZB)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_VALVE_OPENING_DEGREE, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
	.zigbee_attribute_name = "VALVE OPENING DEGREE [0-100%] (TRVZB)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_VALVE_CLOSING_DEGREE, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
	.zigbee_attribute_name = "VALVE CLOSING DEGREE [0-100%] (TRVZB)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_EXTERNAL_TEMPERATURE_INPUT, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
	.zigbee_attribute_name = "EXTERNAL TEMPERATURE INPUT (TRVZB)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_TEMPERATURE_SENSOR_SELECT, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
	.zigbee_attribute_name = "TEMPERATURE SENSOR SELECT (TRVZB)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_TEMPERATURE_ACCURACY, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
	.zigbee_attribute_name = "TEMPERATURE ACCURACY [-1°C...-0.2°C/0.2°C/x100] (TRVZB 1.3.0)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_AC_CURRENT_ID, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U32,
	.zigbee_attribute_name = "AC CURRENT"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_AC_VOLTAGE_ID, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U32,
	.zigbee_attribute_name = "AC VOLTAGE"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_AC_POWER_ID, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U32,
	.zigbee_attribute_name = "AC ACTIVE POWER"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_AC_ENERGY_TODAY_ID, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U32,
	.zigbee_attribute_name = "ENERGY [kWh] (TODAY)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_AC_ENERGY_MONTH_ID, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U32,
	.zigbee_attribute_name = "ENERGY [kWh] (MONTH)"
	},
	{
	.zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_AC_ENERGY_YESTERDAY_ID, 
	.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
	.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U32,
	.zigbee_attribute_name = "ENERGY [kWh] (YESTERDAY)"	
	},

	{ .zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_OUTLET_CONTROL_PROTECT_ID, 
		.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U8,
		.zigbee_attribute_name = "OUTLET CONTROL PROTECT"	},
	
	{ .zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_TEMPERATURE_UNITS_ID, 
		.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_U16,
		.zigbee_attribute_name = "TEMPERATURE UNIT °C/°F"	},

	{ .zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_TEMPERATURE_CALIBRATION_ID, 
		.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "TEMPERATURE CALIBRATION [-50°C..50°C/0.1°C/x100]" },

	{ .zigbee_attribute_id  = SONOFF_CUSTOM_CLUSTER_HUMIDITY_CALIBRATION_ID, 
		.zigbee_attribute_cluster_id = SONOFF_CUSTOM_CLUSTER, 
		.zigbee_attribute_datatype_id = ESP_ZB_ZCL_ATTR_TYPE_S16,
		.zigbee_attribute_name = "HUMIDITY CALIBRATION [-50°C..50°C/0.1°C/x100]" },
};
	 

static constexpr zigbee_attribute_value_t zigbee_attribute_values [] PROGMEM = {

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
		.zigbee_attribute_id = 0x8002,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
		.zigbee_attribute_value_name = "OFF",
		.zigbee_attribute_value = 0
	},
	{
		.zigbee_attribute_id = 0x8002,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
		.zigbee_attribute_value_name = "ON",
		.zigbee_attribute_value = 1
	},
	{
		.zigbee_attribute_id = 0x8002,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
		.zigbee_attribute_value_name = "RESTORE",
		.zigbee_attribute_value = 2
	},
	{
		.zigbee_attribute_id = 0x8001,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
		.zigbee_attribute_value_name = "OFF",
		.zigbee_attribute_value = 0
	},
	{
		.zigbee_attribute_id = 0x8001,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
		.zigbee_attribute_value_name = "ON WHEN SWITCH ON",
		.zigbee_attribute_value = 1
	},
	{
		.zigbee_attribute_id = 0x8001,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
		.zigbee_attribute_value_name = "ON WHEN SWITCH OFF",
		.zigbee_attribute_value = 2
	},
	{
		.zigbee_attribute_id = 0x5000,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
		.zigbee_attribute_value_name = "OFF",
		.zigbee_attribute_value = 0
	},
	{
		.zigbee_attribute_id = 0x5000,
		.zigbee_cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
		.zigbee_attribute_value_name = "ON",
		.zigbee_attribute_value = 1
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
	},
	{
		.zigbee_attribute_id = SONOFF_CUSTOM_CLUSTER_CHILD_LOCK_ID,
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER,
		.zigbee_attribute_value_name = "OFF",
		.zigbee_attribute_value = 0x00
	},
	{
		.zigbee_attribute_id = SONOFF_CUSTOM_CLUSTER_CHILD_LOCK_ID,
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER,
		.zigbee_attribute_value_name = "ON",
		.zigbee_attribute_value = 0x01
	},
	{
		.zigbee_attribute_id = SONOFF_CUSTOM_CLUSTER_TURBO_MODE_ID,
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER,
		.zigbee_attribute_value_name = "OFF",
		.zigbee_attribute_value = 0x09
	},
	{
		.zigbee_attribute_id = SONOFF_CUSTOM_CLUSTER_TURBO_MODE_ID,
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER,
		.zigbee_attribute_value_name = "ON",
		.zigbee_attribute_value = 0x14
	},
	{
		.zigbee_attribute_id = SONOFF_CUSTOM_CLUSTER_OPEN_WINDOW_ID,
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER,
		.zigbee_attribute_value_name = "OFF",
		.zigbee_attribute_value = 0x00
	},
	{
		.zigbee_attribute_id = SONOFF_CUSTOM_CLUSTER_OPEN_WINDOW_ID,
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER,
		.zigbee_attribute_value_name = "ON",
		.zigbee_attribute_value = 0x01
	},
	{
		.zigbee_attribute_id = SONOFF_CUSTOM_CLUSTER_TEMPERATURE_SENSOR_SELECT,
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER,
		.zigbee_attribute_value_name = "INTERNAL",
		.zigbee_attribute_value = 0x00
	},
	{
		.zigbee_attribute_id = SONOFF_CUSTOM_CLUSTER_TEMPERATURE_SENSOR_SELECT,
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER,
		.zigbee_attribute_value_name = "EXTERNAL",
		.zigbee_attribute_value = 0x01
	},
	{
		.zigbee_attribute_id = SONOFF_CUSTOM_CLUSTER_OUTLET_CONTROL_PROTECT_ID,
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER,
		.zigbee_attribute_value_name = "OFF",
		.zigbee_attribute_value = 0x00
	},
	{
		.zigbee_attribute_id = SONOFF_CUSTOM_CLUSTER_OUTLET_CONTROL_PROTECT_ID,
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER,
		.zigbee_attribute_value_name = "ON",
		.zigbee_attribute_value = 0x01
	},

	{
		.zigbee_attribute_id = SONOFF_CUSTOM_CLUSTER_TEMPERATURE_UNITS_ID,
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER,
		.zigbee_attribute_value_name = "CELCIUS",
		.zigbee_attribute_value = 0x00
	},
	{
		.zigbee_attribute_id = SONOFF_CUSTOM_CLUSTER_TEMPERATURE_UNITS_ID,
		.zigbee_cluster_id = SONOFF_CUSTOM_CLUSTER,
		.zigbee_attribute_value_name = "FAHRENHEIT",
		.zigbee_attribute_value = 0x01
	}
};

static const Tuya_datapoint_desc_t Tuya_datapoints[] PROGMEM = {

  { .Tuya_datapoint_desc_id = 1,
    .z2s_device_desc_id 	 			= Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR,
    .Tuya_datapoint_id 		 			= 0x09,
    .Tuya_datapoint_type 				= 0x04,
    .Tuya_datapoint_length 			= 0x01,
    .Tuya_datapoint_name 	 			= "Sensitivity",
    .Tuya_datapoint_description = "0 (low), 1 (medium), 2 (high)" }
};

#endif //WEB_GUI_TEMPLATES_H_