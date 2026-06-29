#ifndef SRC_SUPLA_Z2S_CUSTOM_ACTIONS_EVENTS_H_
#define SRC_SUPLA_Z2S_CUSTOM_ACTIONS_EVENTS_H_

#include <supla/actions.h>
#include <supla/events.h>

#define Z2S_SUPLA_ACTION_FIRST_ACTION                                         0x4000

#define Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_ON                                 0x4000
#define Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_OFF                                0x4001
#define Z2S_SUPLA_ACTION_DEVICE_STATUS_CHANGE                                 0x4010

#define Z2S_SUPLA_ACTION_START_TIMER                                          0x4020
#define Z2S_SUPLA_ACTION_STOP_TIMER                                           0x4021
#define Z2S_SUPLA_ACTION_TURN_ON_LED_WHITE                                    0x4030
#define Z2S_SUPLA_ACTION_TURN_OFF_LED                                         0x4031

#define Z2S_SUPLA_ACTION_OPEN_ZIGBEE_NETWORK                                  0x4100
#define Z2S_SUPLA_ACTION_CLOSE_ZIGBEE_NETWORK                                 0x4101
#define Z2S_SUPLA_ACTION_SET_FORCE_BIND_DEVICE                                0x4105
#define Z2S_SUPLA_ACTION_CLEAR_FORCE_BIND_DEVICE                              0x4106

#define Z2S_SUPLA_ACTION_START_GUI_MINIMAL                                    0x4200
#define Z2S_SUPLA_ACTION_START_GUI_STANDARD                                   0x4201

#define Z2S_SUPLA_ACTION_DIM_W                                                0x4500
#define Z2S_SUPLA_ACTION_DIM_W_5                                              0x4501
#define Z2S_SUPLA_ACTION_DIM_W_15                                             0x4502
#define Z2S_SUPLA_ACTION_DIM_W_20                                             0x4503

#define Z2S_SUPLA_ACTION_TURN_ON_MAX_TEMPERATURE                              0x5000
#define Z2S_SUPLA_ACTION_TURN_ON_MIN_TEMPERATURE                              0x5001

#define Z2S_SUPLA_ACTION_RESEND_RELAY_STATE                                   0x5100
#define Z2S_SUPLA_ACTION_SET_RELAY_STATE_DISABLED                             0x5101
#define Z2S_SUPLA_ACTION_RESYNC_REMOTE_RELAY                                  0x5102
#define Z2S_SUPLA_ACTION_COPY_ON_STATE                                        0x5105
#define Z2S_SUPLA_ACTION_COPY_OFF_STATE                                       0x5106

#define Z2S_SUPLA_ACTION_TURN_OFF_RELAY_5_SECONDS                             0x5110

/***********************************************************************************/

#define Z2S_SUPLA_ACTION_CORE_ACTIONS_FIRST_ACTION                            0x6000

#define Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON                                  0x6000
#define Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_WITHOUT_TIMER                    0x6001
#define Z2S_SUPLA_ACTION_CORE_ACTION_TURN_OFF                                 0x6002
#define Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE                                   0x6003
#define Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_WITH_POSTPONED_COMM               0x6004
#define Z2S_SUPLA_ACTION_CORE_ACTION_SET                                      0x6005
#define Z2S_SUPLA_ACTION_CORE_ACTION_CLEAR                                    0x6006
#define Z2S_SUPLA_ACTION_CORE_ACTION_OPEN                                     0x6007
#define Z2S_SUPLA_ACTION_CORE_ACTION_CLOSE                                    0x6008
#define Z2S_SUPLA_ACTION_CORE_ACTION_STOP                                     0x6009
#define Z2S_SUPLA_ACTION_CORE_ACTION_OPEN_OR_STOP                             0x600A
#define Z2S_SUPLA_ACTION_CORE_ACTION_CLOSE_OR_STOP                            0x600B
#define Z2S_SUPLA_ACTION_CORE_ACTION_COMFORT_UP_POSITION                      0x600C
#define Z2S_SUPLA_ACTION_CORE_ACTION_COMFORT_DOWN_POSITION                    0x600D
#define Z2S_SUPLA_ACTION_CORE_ACTION_STEP_BY_STEP                             0x600E
#define Z2S_SUPLA_ACTION_CORE_ACTION_MOVE_UP                                  0x600F
#define Z2S_SUPLA_ACTION_CORE_ACTION_MOVE_DOWN                                0x6010
#define Z2S_SUPLA_ACTION_CORE_ACTION_MOVE_UP_OR_STOP                          0x6011
#define Z2S_SUPLA_ACTION_CORE_ACTION_MOVE_DOWN_OR_STOP                        0x6012
#define Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_MOVE_UP_OR_STOP          0x6013
#define Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_MOVE_DOWN_OR_STOP        0x6014
#define Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_MOVE_UP                  0x6015
#define Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_MOVE_DOWN                0x6016
#define Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_COMFORT_UP               0x6017
#define Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_COMFORT_DOWN             0x6018
#define Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_ALL                             0x6019
#define Z2S_SUPLA_ACTION_CORE_ACTION_DIM_ALL                                  0x601A
#define Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_R                               0x601B
#define Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_G                               0x601C
#define Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_B                               0x601D
#define Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_W                               0x601E
#define Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_RGB                             0x601F
#define Z2S_SUPLA_ACTION_CORE_ACTION_DIM_R                                    0x6020
#define Z2S_SUPLA_ACTION_CORE_ACTION_DIM_G                                    0x6021
#define Z2S_SUPLA_ACTION_CORE_ACTION_DIM_B                                    0x6022
#define Z2S_SUPLA_ACTION_CORE_ACTION_DIM_W                                    0x6023
#define Z2S_SUPLA_ACTION_CORE_ACTION_DIM_RGB                                  0x6024
#define Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_RGB                              0x6025
#define Z2S_SUPLA_ACTION_CORE_ACTION_TURN_OFF_RGB                             0x6026
#define Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_RGB                               0x6027
#define Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_W                                0x6028
#define Z2S_SUPLA_ACTION_CORE_ACTION_TURN_OFF_W                               0x6029
#define Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_W                                 0x602A
#define Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_RGB_DIMMED                       0x602B
#define Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_W_DIMMED                         0x602C
#define Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_ALL_DIMMED                       0x602D
#define Z2S_SUPLA_ACTION_CORE_ACTION_ITERATE_DIM_RGB                          0x602E
#define Z2S_SUPLA_ACTION_CORE_ACTION_ITERATE_DIM_W                            0x602F
#define Z2S_SUPLA_ACTION_CORE_ACTION_ITERATE_DIM_ALL                          0x6030
#define Z2S_SUPLA_ACTION_CORE_ACTION_RESET                                    0x6031
#define Z2S_SUPLA_ACTION_CORE_ACTION_INCREMENT                                0x6032
#define Z2S_SUPLA_ACTION_CORE_ACTION_DECREMENT                                0x6033
#define Z2S_SUPLA_ACTION_CORE_ACTION_ENABLE                                   0x6034
#define Z2S_SUPLA_ACTION_CORE_ACTION_DISABLE                                  0x6035
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TURN_ON                          0x6036
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TURN_OFF                         0x6037
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x1                        0x6038
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x2                        0x6039
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x3                        0x603A
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x4                        0x603B
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x5                        0x603C
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_HOLD                             0x603D
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x1                   0x603E
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x2                   0x603F
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x3                   0x6040
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x4                   0x6041
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x5                   0x6042
#define Z2S_SUPLA_ACTION_CORE_ACTION_ENTER_CONFIG_MODE                        0x6043
#define Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_CONFIG_MODE                       0x6044
#define Z2S_SUPLA_ACTION_CORE_ACTION_RESET_TO_FACTORY_SETTINGS                0x6045
#define Z2S_SUPLA_ACTION_CORE_ACTION_SOFT_RESTART                             0x6046
#define Z2S_SUPLA_ACTION_CORE_ACTION_START_LOCAL_WEB_SERVER                   0x6047
#define Z2S_SUPLA_ACTION_CORE_ACTION_STOP_LOCAL_WEB_SERVER                    0x6048
#define Z2S_SUPLA_ACTION_CORE_ACTION_CHECK_SW_UPDATE                          0x6049
#define Z2S_SUPLA_ACTION_CORE_ACTION_LEAVE_CONFIG_MODE_AND_RESET              0x604A
#define Z2S_SUPLA_ACTION_CORE_ACTION_ENTER_CONFIG_MODE_OR_RESET_TO_FACTORY    0x604B
#define Z2S_SUPLA_ACTION_CORE_ACTION_TARE_SCALES                              0x604C
#define Z2S_SUPLA_ACTION_CORE_ACTION_VOLUME_UP                                0x604D
#define Z2S_SUPLA_ACTION_CORE_ACTION_VOLUME_DOWN                              0x604E
#define Z2S_SUPLA_ACTION_CORE_ACTION_MUTE_SOUND_ALARM                         0x604F
#define Z2S_SUPLA_ACTION_CORE_ACTION_ENABLE_EXTERNAL_SOUND_ALARM              0x6050
#define Z2S_SUPLA_ACTION_CORE_ACTION_DISABLE_EXTERNAL_SOUND_ALARM             0x6051
#define Z2S_SUPLA_ACTION_CORE_ACTION_INCREASE_TEMPERATURE                     0x6052
#define Z2S_SUPLA_ACTION_CORE_ACTION_DECREASE_TEMPERATURE                     0x6053
#define Z2S_SUPLA_ACTION_CORE_ACTION_INCREASE_HEATING_TEMPERATURE             0x6054
#define Z2S_SUPLA_ACTION_CORE_ACTION_DECREASE_HEATING_TEMPERATURE             0x6055
#define Z2S_SUPLA_ACTION_CORE_ACTION_INCREASE_COOLING_TEMPERATURE             0x6056
#define Z2S_SUPLA_ACTION_CORE_ACTION_DECREASE_COOLING_TEMPERATURE             0x6057
#define Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_MANUAL_MODE                    0x6058
#define Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_WEEKLY_SCHEDULE_MODE           0x6059
#define Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_MANUAL_MODE_HEAT               0x605A
#define Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_MANUAL_MODE_COOL               0x605B
#define Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_MANUAL_MODE_HEAT_COOL          0x605C
#define Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_MANUAL_WEEKLY_SCHEDULE_MODES      0x605D
#define Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_OFF_MANUAL_WEEKLY_SCHEDULE_MODES  0x605E
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_1_START                         0x605F
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_1_STOP                          0x6060
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_2_START                         0x6061
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_2_STOP                          0x6062
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_3_START                         0x6063
#define Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_3_STOP                          0x6064

#define Z2S_SUPLA_ACTION_CORE_ACTIONS_LAST_ACTION                             0x6065

/***********************************************************************************/

#define Z2S_SUPLA_ACTION_PUSHOVER_FIRST_ACTION                                0x8000
//256 subactions
#define Z2S_SUPLA_ACTION_PUSHOVER_LAST_ACTION                                 0x8100 

/***********************************************************************************/

#define Z2S_SUPLA_EVENT_FIRST_EVENT                                           0x4000

#define Z2S_SUPLA_EVENT_ON_SUPLA_INITIALIZED                                  0x4009
#define Z2S_SUPLA_EVENT_ON_SUPLA_REGISTERED_AND_READY                         0x4010
#define Z2S_SUPLA_EVENT_ON_ZIGBEE_STARTED                                     0x4020
#define Z2S_SUPLA_EVENT_ON_ZIGBEE_OPEN_NETWORK                                0x4021
#define Z2S_SUPLA_EVENT_ON_ZIGBEE_CLOSE_NETWORK                               0x4022

#define Z2S_SUPLA_EVENT_ON_GUI_STARTED                                        0x4030
#define Z2S_SUPLA_EVENT_ON_GUI_NOT_STARTED                                    0x4031

#define Z2S_SUPLA_EVENT_ON_EVERY_5_SECONDS                                    0x4100
#define Z2S_SUPLA_EVENT_ON_EVERY_30_SECONDS                                   0x4101
#define Z2S_SUPLA_EVENT_ON_EVERY_60_SECONDS                                   0x4102
#define Z2S_SUPLA_EVENT_ON_EVERY_24_HOURS                                     0x4150

#define Z2S_SUPLA_EVENT_ON_GUI_MODE_SELECT                                    0x4200

#define Z2S_SUPLA_EVENT_ON_COPY_ON_STATE                                      0x4300
#define Z2S_SUPLA_EVENT_ON_COPY_OFF_STATE                                     0x4301

#define Z2S_SUPLA_EVENT_CORE_EVENT_FIRST_EVENT                                0x6000

#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_PRESS                                   0x6000
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_RELEASE                                 0x6001
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CHANGE                                  0x6002
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_HOLD                                    0x6003
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_1                                 0x6004
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_2                                 0x6005
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_3                                 0x6006
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_4                                 0x6007
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_5                                 0x6008
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_6                                 0x6009
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_7                                 0x600A
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_8                                 0x600B
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_9                                 0x600C
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_10                                0x600D
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CRAZY_CLICKER                           0x600E
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_SEQUENCE_MATCH                          0x600F
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_SEQUENCE_DOESNT_MATCH                   0x6010
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_TURN_ON                                 0x6011
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_TURN_OFF                                0x6012
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_SECONDARY_CHANNEL_CHANGE                0x6013
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_0                            0x6014
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_1                            0x6015
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_2                            0x6016
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_3                            0x6017
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_4                            0x6018
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_5                            0x6019
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_6                            0x601A
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_7                            0x601B
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_8                            0x601C
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_9                            0x601D
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_10                           0x601E
#define Z2S_SUPLA_EVENT_CORE_EVENT_CONDITIONAL_ON_PRESS                       0x601F
#define Z2S_SUPLA_EVENT_CORE_EVENT_CONDITIONAL_ON_RELEASE                     0x6020
#define Z2S_SUPLA_EVENT_CORE_EVENT_CONDITIONAL_ON_CHANGE                      0x6021
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_HOLD_RELEASE                            0x6022
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_DEVICE_STATUS_CHANGE                    0x6023
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_IDENTIFY                                0x6024
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_1                                 0x6025
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_2                                 0x6026
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_3                                 0x6027
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_4                                 0x6028
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_5                                 0x6029
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_6                                 0x602A
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_7                                 0x602B
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_8                                 0x602C
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_9                                 0x602D
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_10                                0x602E
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_DIMMER_TURN_ON                          0x602F
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_DIMMER_TURN_OFF                         0x6030
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_DIMMER_BRIGHTNESS_CHANGE                0x6031
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_COLOR_TURN_ON                           0x6032
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_COLOR_TURN_OFF                          0x6033
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_COLOR_BRIGHTNESS_CHANGE                 0x6034
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_RED_TURN_ON                             0x6035
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_RED_TURN_OFF                            0x6036
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_RED_CHANGE                              0x6037
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_GREEN_TURN_ON                           0x6038
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_GREEN_TURN_OFF                          0x6039
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_GREEN_CHANGE                            0x603A
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_BLUE_TURN_ON                            0x603B
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_BLUE_TURN_OFF                           0x603C
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_BLUE_CHANGE                             0x603D
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_WHITE_TEMPERATURE_CHANGE                0x603E
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CHANNEL_OFFLINE                         0x603F
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CHANNEL_ONLINE                          0x6040
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CHANNEL_FUNCTION_CHANGE                 0x6041
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_HEATING                            0x6042
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_COOLING                            0x6043
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_STANDBY                            0x6044
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_WEEKLY_SCHEDULE_ENABLED            0x6045
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_WEEKLY_SCHEDULE_DISABLED           0x6046
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_MODE_OFF                           0x6047
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_MODE_HEAT                          0x6048
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_MODE_COOL                          0x6049
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_MODE_HEAT_COOL                     0x604A
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_ERROR                                   0x604B
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_ERROR_CLEAR                             0x604C
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_ALARM_ACTIVE                  0x604D
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_ALARM_INACTIVE                0x604E
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_WARNING_ACTIVE                0x604F
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_WARNING_INACTIVE              0x6050
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_INVALID_SENSOR_STATE_ACTIVE   0x6051
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_INVALID_SENSOR_STATE_INACTIVE 0x6052
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SOUND_ALARM_ACTIVE            0x6053
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SOUND_ALARM_INACTIVE          0x6054
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SENSOR_OFFLINE_ACTIVE         0x6055
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SENSOR_OFFLINE_INACTIVE       0x6056
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SOUND_ALARM_MUTED             0x6057
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_OPEN                                    0x6058
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLOSE                                   0x6059
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_FLOODING_ACTIVE                         0x605A
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_FLOODING_INACTIVE                       0x605B
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_VALVE_MANUALLY_CLOSED_ACTIVE            0x605C
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_VALVE_MANUALLY_CLOSED_INACTIVE          0x605D
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_MOTOR_PROBLEM_ACTIVE                    0x605E
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_MOTOR_PROBLEM_INACTIVE                  0x605F
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_SUBDEVICE_PAIRING_START                 0x6060
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_SUBDEVICE_PAIRING_END                   0x6061
#define Z2S_SUPLA_EVENT_CORE_EVENT_ON_IMPULSE                                 0x6062

#define Z2S_SUPLA_EVENT_CORE_EVENTS_LAST_EVENT                                0x6063

/***********************************************************************************/

Supla::Action convertZ2SActionToSuplaAction(uint32_t z2s_core_supla_action);
uint32_t convertSuplaActionToZ2SAction(Supla::Action supla_action);

Supla::Event convertZ2SEventToSuplaEvent(uint32_t core_supla_event);
uint32_t convertSuplaEventToZ2SEvent(Supla::Event supla_event);

#endif //

