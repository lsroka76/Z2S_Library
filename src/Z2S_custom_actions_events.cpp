#include "Z2S_custom_actions_events.h"

Supla::Action convertZ2SActionToSuplaAction(uint32_t z2s_core_supla_action) {

  switch (z2s_core_supla_action) {
    
    
    case Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON:
      return Supla::Action::TURN_ON;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_WITHOUT_TIMER:
      return Supla::Action::TURN_ON_WITHOUT_TIMER;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TURN_OFF:
      return Supla::Action::TURN_OFF;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE:
      return Supla::Action::TOGGLE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_WITH_POSTPONED_COMM:
      return Supla::Action::TOGGLE_WITH_POSTPONED_COMM;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SET:
      return Supla::Action::SET;


    case Z2S_SUPLA_ACTION_CORE_ACTION_CLEAR:
      return Supla::Action::CLEAR;


    case Z2S_SUPLA_ACTION_CORE_ACTION_OPEN:
      return Supla::Action::OPEN;


    case Z2S_SUPLA_ACTION_CORE_ACTION_CLOSE:
      return Supla::Action::CLOSE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_STOP:
      return Supla::Action::STOP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_OPEN_OR_STOP:
      return Supla::Action::OPEN_OR_STOP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_CLOSE_OR_STOP:
      return Supla::Action::CLOSE_OR_STOP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_COMFORT_UP_POSITION:
      return Supla::Action::COMFORT_UP_POSITION;


    case Z2S_SUPLA_ACTION_CORE_ACTION_COMFORT_DOWN_POSITION:
      return Supla::Action::COMFORT_DOWN_POSITION;


    case Z2S_SUPLA_ACTION_CORE_ACTION_STEP_BY_STEP:
      return Supla::Action::STEP_BY_STEP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_MOVE_UP:
      return Supla::Action::MOVE_UP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_MOVE_DOWN:
      return Supla::Action::MOVE_DOWN;


    case Z2S_SUPLA_ACTION_CORE_ACTION_MOVE_UP_OR_STOP:
      return Supla::Action::MOVE_UP_OR_STOP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_MOVE_DOWN_OR_STOP:
      return Supla::Action::MOVE_DOWN_OR_STOP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_MOVE_UP_OR_STOP:
      return Supla::Action::INTERNAL_BUTTON_MOVE_UP_OR_STOP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_MOVE_DOWN_OR_STOP:
      return Supla::Action::INTERNAL_BUTTON_MOVE_DOWN_OR_STOP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_MOVE_UP:
      return Supla::Action::INTERNAL_BUTTON_MOVE_UP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_MOVE_DOWN:
      return Supla::Action::INTERNAL_BUTTON_MOVE_DOWN;


    case Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_COMFORT_UP:
      return Supla::Action::INTERNAL_BUTTON_COMFORT_UP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_COMFORT_DOWN:
      return Supla::Action::INTERNAL_BUTTON_COMFORT_DOWN;


    case Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_ALL:
      return Supla::Action::BRIGHTEN_ALL;


    case Z2S_SUPLA_ACTION_CORE_ACTION_DIM_ALL:
      return Supla::Action::DIM_ALL;


    case Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_R:
      return Supla::Action::BRIGHTEN_R;


    case Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_G:
      return Supla::Action::BRIGHTEN_G;


    case Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_B:
      return Supla::Action::BRIGHTEN_B;


    case Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_W:
      return Supla::Action::BRIGHTEN_W;


    case Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_RGB:
      return Supla::Action::BRIGHTEN_RGB;


    case Z2S_SUPLA_ACTION_CORE_ACTION_DIM_R:
      return Supla::Action::DIM_R;


    case Z2S_SUPLA_ACTION_CORE_ACTION_DIM_G:
      return Supla::Action::DIM_G;


    case Z2S_SUPLA_ACTION_CORE_ACTION_DIM_B:
      return Supla::Action::DIM_B;


    case Z2S_SUPLA_ACTION_CORE_ACTION_DIM_W:
      return Supla::Action::DIM_W;


    case Z2S_SUPLA_ACTION_CORE_ACTION_DIM_RGB:
      return Supla::Action::DIM_RGB;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_RGB:
      return Supla::Action::TURN_ON_RGB;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TURN_OFF_RGB:
      return Supla::Action::TURN_OFF_RGB;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_RGB:
      return Supla::Action::TOGGLE_RGB;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_W:
      return Supla::Action::TURN_ON_W;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TURN_OFF_W:
      return Supla::Action::TURN_OFF_W;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_W:
      return Supla::Action::TOGGLE_W;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_RGB_DIMMED:
      return Supla::Action::TURN_ON_RGB_DIMMED;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_W_DIMMED:
      return Supla::Action::TURN_ON_W_DIMMED;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_ALL_DIMMED:
      return Supla::Action::TURN_ON_ALL_DIMMED;


    case Z2S_SUPLA_ACTION_CORE_ACTION_ITERATE_DIM_RGB:
      return Supla::Action::ITERATE_DIM_RGB;


    case Z2S_SUPLA_ACTION_CORE_ACTION_ITERATE_DIM_W:
      return Supla::Action::ITERATE_DIM_W;


    case Z2S_SUPLA_ACTION_CORE_ACTION_ITERATE_DIM_ALL:
      return Supla::Action::ITERATE_DIM_ALL;


    case Z2S_SUPLA_ACTION_CORE_ACTION_RESET:
      return Supla::Action::RESET;


    case Z2S_SUPLA_ACTION_CORE_ACTION_INCREMENT:
      return Supla::Action::INCREMENT;


    case Z2S_SUPLA_ACTION_CORE_ACTION_DECREMENT:
      return Supla::Action::DECREMENT;


    case Z2S_SUPLA_ACTION_CORE_ACTION_ENABLE:
      return Supla::Action::ENABLE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_DISABLE:
      return Supla::Action::DISABLE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TURN_ON:
      return Supla::Action::SEND_AT_TURN_ON;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TURN_OFF:
      return Supla::Action::SEND_AT_TURN_OFF;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x1:
      return Supla::Action::SEND_AT_TOGGLE_x1;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x2:
      return Supla::Action::SEND_AT_TOGGLE_x2;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x3:
      return Supla::Action::SEND_AT_TOGGLE_x3;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x4:
      return Supla::Action::SEND_AT_TOGGLE_x4;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x5:
      return Supla::Action::SEND_AT_TOGGLE_x5;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_HOLD:
      return Supla::Action::SEND_AT_HOLD;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x1:
      return Supla::Action::SEND_AT_SHORT_PRESS_x1;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x2:
      return Supla::Action::SEND_AT_SHORT_PRESS_x2;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x3:
      return Supla::Action::SEND_AT_SHORT_PRESS_x3;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x4:
      return Supla::Action::SEND_AT_SHORT_PRESS_x4;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x5:
      return Supla::Action::SEND_AT_SHORT_PRESS_x5;


    case Z2S_SUPLA_ACTION_CORE_ACTION_ENTER_CONFIG_MODE:
      return Supla::Action::ENTER_CONFIG_MODE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_CONFIG_MODE:
      return Supla::Action::TOGGLE_CONFIG_MODE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_RESET_TO_FACTORY_SETTINGS:
      return Supla::Action::RESET_TO_FACTORY_SETTINGS;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SOFT_RESTART:
      return Supla::Action::SOFT_RESTART;


    case Z2S_SUPLA_ACTION_CORE_ACTION_START_LOCAL_WEB_SERVER:
      return Supla::Action::START_LOCAL_WEB_SERVER;


    case Z2S_SUPLA_ACTION_CORE_ACTION_STOP_LOCAL_WEB_SERVER:
      return Supla::Action::STOP_LOCAL_WEB_SERVER;


    case Z2S_SUPLA_ACTION_CORE_ACTION_CHECK_SW_UPDATE:
      return Supla::Action::CHECK_SW_UPDATE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_LEAVE_CONFIG_MODE_AND_RESET:
      return Supla::Action::LEAVE_CONFIG_MODE_AND_RESET;


    case Z2S_SUPLA_ACTION_CORE_ACTION_ENTER_CONFIG_MODE_OR_RESET_TO_FACTORY:
      return Supla::Action::ENTER_CONFIG_MODE_OR_RESET_TO_FACTORY;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TARE_SCALES:
      return Supla::Action::TARE_SCALES;


    case Z2S_SUPLA_ACTION_CORE_ACTION_VOLUME_UP:
      return Supla::Action::VOLUME_UP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_VOLUME_DOWN:
      return Supla::Action::VOLUME_DOWN;


    case Z2S_SUPLA_ACTION_CORE_ACTION_MUTE_SOUND_ALARM:
      return Supla::Action::MUTE_SOUND_ALARM;


    case Z2S_SUPLA_ACTION_CORE_ACTION_ENABLE_EXTERNAL_SOUND_ALARM:
      return Supla::Action::ENABLE_EXTERNAL_SOUND_ALARM;


    case Z2S_SUPLA_ACTION_CORE_ACTION_DISABLE_EXTERNAL_SOUND_ALARM:
      return Supla::Action::DISABLE_EXTERNAL_SOUND_ALARM;


    case Z2S_SUPLA_ACTION_CORE_ACTION_INCREASE_TEMPERATURE:
      return Supla::Action::INCREASE_TEMPERATURE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_DECREASE_TEMPERATURE:
      return Supla::Action::DECREASE_TEMPERATURE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_INCREASE_HEATING_TEMPERATURE:
      return Supla::Action::INCREASE_HEATING_TEMPERATURE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_DECREASE_HEATING_TEMPERATURE:
      return Supla::Action::DECREASE_HEATING_TEMPERATURE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_INCREASE_COOLING_TEMPERATURE:
      return Supla::Action::INCREASE_COOLING_TEMPERATURE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_DECREASE_COOLING_TEMPERATURE:
      return Supla::Action::DECREASE_COOLING_TEMPERATURE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_MANUAL_MODE:
      return Supla::Action::SWITCH_TO_MANUAL_MODE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_WEEKLY_SCHEDULE_MODE:
      return Supla::Action::SWITCH_TO_WEEKLY_SCHEDULE_MODE;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_MANUAL_MODE_HEAT:
      return Supla::Action::SWITCH_TO_MANUAL_MODE_HEAT;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_MANUAL_MODE_COOL:
      return Supla::Action::SWITCH_TO_MANUAL_MODE_COOL;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_MANUAL_MODE_HEAT_COOL:
      return Supla::Action::SWITCH_TO_MANUAL_MODE_HEAT_COOL;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_MANUAL_WEEKLY_SCHEDULE_MODES:
      return Supla::Action::TOGGLE_MANUAL_WEEKLY_SCHEDULE_MODES;


    case Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_OFF_MANUAL_WEEKLY_SCHEDULE_MODES:
      return Supla::Action::TOGGLE_OFF_MANUAL_WEEKLY_SCHEDULE_MODES;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_1_START:
      return Supla::Action::SEQUENCE_1_START;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_1_STOP:
      return Supla::Action::SEQUENCE_1_STOP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_2_START:
      return Supla::Action::SEQUENCE_2_START;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_2_STOP:
      return Supla::Action::SEQUENCE_2_STOP;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_3_START:
      return Supla::Action::SEQUENCE_3_START;


    case Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_3_STOP:
      return Supla::Action::SEQUENCE_3_STOP;


    default:
      return (Supla::Action)z2s_core_supla_action;
  }
}
  

uint32_t convertSuplaActionToZ2SAction(Supla::Action supla_action) {

  switch (supla_action) {


    //Supla::Action::TURN_ON
    case (Supla::Action)0:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON;


    //Supla::Action::TURN_ON_WITHOUT_TIMER
    case (Supla::Action)1:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_WITHOUT_TIMER;


    //Supla::Action::TURN_OFF
    case (Supla::Action)2:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TURN_OFF;


    //Supla::Action::TOGGLE
    case (Supla::Action)3:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE;


    //Supla::Action::TOGGLE_WITH_POSTPONED_COMM
    case (Supla::Action)4:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_WITH_POSTPONED_COMM;


    //Supla::Action::SET
    case (Supla::Action)5:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SET;


    //Supla::Action::CLEAR
    case (Supla::Action)6:
      return Z2S_SUPLA_ACTION_CORE_ACTION_CLEAR;


    //Supla::Action::OPEN
    case (Supla::Action)7:
      return Z2S_SUPLA_ACTION_CORE_ACTION_OPEN;


    //Supla::Action::CLOSE
    case (Supla::Action)8:
      return Z2S_SUPLA_ACTION_CORE_ACTION_CLOSE;


    //Supla::Action::STOP
    case (Supla::Action)9:
      return Z2S_SUPLA_ACTION_CORE_ACTION_STOP;


    //Supla::Action::OPEN_OR_STOP
    case (Supla::Action)10:
      return Z2S_SUPLA_ACTION_CORE_ACTION_OPEN_OR_STOP;


    //Supla::Action::CLOSE_OR_STOP
    case (Supla::Action)11:
      return Z2S_SUPLA_ACTION_CORE_ACTION_CLOSE_OR_STOP;


    //Supla::Action::COMFORT_UP_POSITION
    case (Supla::Action)12:
      return Z2S_SUPLA_ACTION_CORE_ACTION_COMFORT_UP_POSITION;


    //Supla::Action::COMFORT_DOWN_POSITION
    case (Supla::Action)13:
      return Z2S_SUPLA_ACTION_CORE_ACTION_COMFORT_DOWN_POSITION;


    //Supla::Action::STEP_BY_STEP
    case (Supla::Action)14:
      return Z2S_SUPLA_ACTION_CORE_ACTION_STEP_BY_STEP;


    //Supla::Action::MOVE_UP
    case (Supla::Action)15:
      return Z2S_SUPLA_ACTION_CORE_ACTION_MOVE_UP;


    //Supla::Action::MOVE_DOWN
    case (Supla::Action)16:
      return Z2S_SUPLA_ACTION_CORE_ACTION_MOVE_DOWN;


    //Supla::Action::MOVE_UP_OR_STOP
    case (Supla::Action)17:
      return Z2S_SUPLA_ACTION_CORE_ACTION_MOVE_UP_OR_STOP;


    //Supla::Action::MOVE_DOWN_OR_STOP
    case (Supla::Action)18:
      return Z2S_SUPLA_ACTION_CORE_ACTION_MOVE_DOWN_OR_STOP;


    //Supla::Action::INTERNAL_BUTTON_MOVE_UP_OR_STOP
    case (Supla::Action)19:
      return Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_MOVE_UP_OR_STOP;


    //Supla::Action::INTERNAL_BUTTON_MOVE_DOWN_OR_STOP
    case (Supla::Action)20:
      return Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_MOVE_DOWN_OR_STOP;


    //Supla::Action::INTERNAL_BUTTON_MOVE_UP
    case (Supla::Action)21:
      return Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_MOVE_UP;


    //Supla::Action::INTERNAL_BUTTON_MOVE_DOWN
    case (Supla::Action)22:
      return Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_MOVE_DOWN;


    //Supla::Action::INTERNAL_BUTTON_COMFORT_UP
    case (Supla::Action)23:
      return Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_COMFORT_UP;


    //Supla::Action::INTERNAL_BUTTON_COMFORT_DOWN
    case (Supla::Action)24:
      return Z2S_SUPLA_ACTION_CORE_ACTION_INTERNAL_BUTTON_COMFORT_DOWN;


    //Supla::Action::BRIGHTEN_ALL
    case (Supla::Action)25:
      return Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_ALL;


    //Supla::Action::DIM_ALL
    case (Supla::Action)26:
      return Z2S_SUPLA_ACTION_CORE_ACTION_DIM_ALL;


    //Supla::Action::BRIGHTEN_R
    case (Supla::Action)27:
      return Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_R;


    //Supla::Action::BRIGHTEN_G
    case (Supla::Action)28:
      return Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_G;


    //Supla::Action::BRIGHTEN_B
    case (Supla::Action)29:
      return Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_B;


    //Supla::Action::BRIGHTEN_W
    case (Supla::Action)30:
      return Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_W;


    //Supla::Action::BRIGHTEN_RGB
    case (Supla::Action)31:
      return Z2S_SUPLA_ACTION_CORE_ACTION_BRIGHTEN_RGB;


    //Supla::Action::DIM_R
    case (Supla::Action)32:
      return Z2S_SUPLA_ACTION_CORE_ACTION_DIM_R;


    //Supla::Action::DIM_G
    case (Supla::Action)33:
      return Z2S_SUPLA_ACTION_CORE_ACTION_DIM_G;


    //Supla::Action::DIM_B
    case (Supla::Action)34:
      return Z2S_SUPLA_ACTION_CORE_ACTION_DIM_B;


    //Supla::Action::DIM_W
    case (Supla::Action)35:
      return Z2S_SUPLA_ACTION_CORE_ACTION_DIM_W;


    //Supla::Action::DIM_RGB
    case (Supla::Action)36:
      return Z2S_SUPLA_ACTION_CORE_ACTION_DIM_RGB;


    //Supla::Action::TURN_ON_RGB
    case (Supla::Action)37:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_RGB;


    //Supla::Action::TURN_OFF_RGB
    case (Supla::Action)38:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TURN_OFF_RGB;


    //Supla::Action::TOGGLE_RGB
    case (Supla::Action)39:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_RGB;


    //Supla::Action::TURN_ON_W
    case (Supla::Action)40:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_W;


    //Supla::Action::TURN_OFF_W
    case (Supla::Action)41:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TURN_OFF_W;


    //Supla::Action::TOGGLE_W
    case (Supla::Action)42:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_W;


    //Supla::Action::TURN_ON_RGB_DIMMED
    case (Supla::Action)43:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_RGB_DIMMED;


    //Supla::Action::TURN_ON_W_DIMMED
    case (Supla::Action)44:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_W_DIMMED;


    //Supla::Action::TURN_ON_ALL_DIMMED
    case (Supla::Action)45:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TURN_ON_ALL_DIMMED;


    //Supla::Action::ITERATE_DIM_RGB
    case (Supla::Action)46:
      return Z2S_SUPLA_ACTION_CORE_ACTION_ITERATE_DIM_RGB;


    //Supla::Action::ITERATE_DIM_W
    case (Supla::Action)47:
      return Z2S_SUPLA_ACTION_CORE_ACTION_ITERATE_DIM_W;


    //Supla::Action::ITERATE_DIM_ALL
    case (Supla::Action)48:
      return Z2S_SUPLA_ACTION_CORE_ACTION_ITERATE_DIM_ALL;


    //Supla::Action::RESET
    case (Supla::Action)49:
      return Z2S_SUPLA_ACTION_CORE_ACTION_RESET;


    //Supla::Action::INCREMENT
    case (Supla::Action)50:
      return Z2S_SUPLA_ACTION_CORE_ACTION_INCREMENT;


    //Supla::Action::DECREMENT
    case (Supla::Action)51:
      return Z2S_SUPLA_ACTION_CORE_ACTION_DECREMENT;


    //Supla::Action::ENABLE
    case (Supla::Action)52:
      return Z2S_SUPLA_ACTION_CORE_ACTION_ENABLE;


    //Supla::Action::DISABLE
    case (Supla::Action)53:
      return Z2S_SUPLA_ACTION_CORE_ACTION_DISABLE;


    //Supla::Action::SEND_AT_TURN_ON
    case (Supla::Action)54:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TURN_ON;


    //Supla::Action::SEND_AT_TURN_OFF
    case (Supla::Action)55:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TURN_OFF;


    //Supla::Action::SEND_AT_TOGGLE_x1
    case (Supla::Action)56:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x1;


    //Supla::Action::SEND_AT_TOGGLE_x2
    case (Supla::Action)57:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x2;


    //Supla::Action::SEND_AT_TOGGLE_x3
    case (Supla::Action)58:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x3;


    //Supla::Action::SEND_AT_TOGGLE_x4
    case (Supla::Action)59:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x4;


    //Supla::Action::SEND_AT_TOGGLE_x5
    case (Supla::Action)60:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_TOGGLE_x5;


    //Supla::Action::SEND_AT_HOLD
    case (Supla::Action)61:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_HOLD;


    //Supla::Action::SEND_AT_SHORT_PRESS_x1
    case (Supla::Action)62:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x1;


    //Supla::Action::SEND_AT_SHORT_PRESS_x2
    case (Supla::Action)63:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x2;


    //Supla::Action::SEND_AT_SHORT_PRESS_x3
    case (Supla::Action)64:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x3;


    //Supla::Action::SEND_AT_SHORT_PRESS_x4
    case (Supla::Action)65:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x4;


    //Supla::Action::SEND_AT_SHORT_PRESS_x5
    case (Supla::Action)66:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEND_AT_SHORT_PRESS_x5;


    //Supla::Action::ENTER_CONFIG_MODE
    case (Supla::Action)67:
      return Z2S_SUPLA_ACTION_CORE_ACTION_ENTER_CONFIG_MODE;


    //Supla::Action::TOGGLE_CONFIG_MODE
    case (Supla::Action)68:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_CONFIG_MODE;


    //Supla::Action::RESET_TO_FACTORY_SETTINGS
    case (Supla::Action)69:
      return Z2S_SUPLA_ACTION_CORE_ACTION_RESET_TO_FACTORY_SETTINGS;


    //Supla::Action::SOFT_RESTART
    case (Supla::Action)70:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SOFT_RESTART;


    //Supla::Action::START_LOCAL_WEB_SERVER
    case (Supla::Action)71:
      return Z2S_SUPLA_ACTION_CORE_ACTION_START_LOCAL_WEB_SERVER;


    //Supla::Action::STOP_LOCAL_WEB_SERVER
    case (Supla::Action)72:
      return Z2S_SUPLA_ACTION_CORE_ACTION_STOP_LOCAL_WEB_SERVER;


    //Supla::Action::CHECK_SW_UPDATE
    case (Supla::Action)73:
      return Z2S_SUPLA_ACTION_CORE_ACTION_CHECK_SW_UPDATE;


    //Supla::Action::LEAVE_CONFIG_MODE_AND_RESET
    case (Supla::Action)74:
      return Z2S_SUPLA_ACTION_CORE_ACTION_LEAVE_CONFIG_MODE_AND_RESET;


    //Supla::Action::ENTER_CONFIG_MODE_OR_RESET_TO_FACTORY
    case (Supla::Action)75:
      return Z2S_SUPLA_ACTION_CORE_ACTION_ENTER_CONFIG_MODE_OR_RESET_TO_FACTORY;


    //Supla::Action::TARE_SCALES
    case (Supla::Action)76:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TARE_SCALES;


    //Supla::Action::VOLUME_UP
    case (Supla::Action)77:
      return Z2S_SUPLA_ACTION_CORE_ACTION_VOLUME_UP;


    //Supla::Action::VOLUME_DOWN
    case (Supla::Action)78:
      return Z2S_SUPLA_ACTION_CORE_ACTION_VOLUME_DOWN;


    //Supla::Action::MUTE_SOUND_ALARM
    case (Supla::Action)79:
      return Z2S_SUPLA_ACTION_CORE_ACTION_MUTE_SOUND_ALARM;


    //Supla::Action::ENABLE_EXTERNAL_SOUND_ALARM
    case (Supla::Action)80:
      return Z2S_SUPLA_ACTION_CORE_ACTION_ENABLE_EXTERNAL_SOUND_ALARM;


    //Supla::Action::DISABLE_EXTERNAL_SOUND_ALARM
    case (Supla::Action)81:
      return Z2S_SUPLA_ACTION_CORE_ACTION_DISABLE_EXTERNAL_SOUND_ALARM;


    //Supla::Action::INCREASE_TEMPERATURE
    case (Supla::Action)82:
      return Z2S_SUPLA_ACTION_CORE_ACTION_INCREASE_TEMPERATURE;


    //Supla::Action::DECREASE_TEMPERATURE
    case (Supla::Action)83:
      return Z2S_SUPLA_ACTION_CORE_ACTION_DECREASE_TEMPERATURE;


    //Supla::Action::INCREASE_HEATING_TEMPERATURE
    case (Supla::Action)84:
      return Z2S_SUPLA_ACTION_CORE_ACTION_INCREASE_HEATING_TEMPERATURE;


    //Supla::Action::DECREASE_HEATING_TEMPERATURE
    case (Supla::Action)85:
      return Z2S_SUPLA_ACTION_CORE_ACTION_DECREASE_HEATING_TEMPERATURE;


    //Supla::Action::INCREASE_COOLING_TEMPERATURE
    case (Supla::Action)86:
      return Z2S_SUPLA_ACTION_CORE_ACTION_INCREASE_COOLING_TEMPERATURE;


    //Supla::Action::DECREASE_COOLING_TEMPERATURE
    case (Supla::Action)87:
      return Z2S_SUPLA_ACTION_CORE_ACTION_DECREASE_COOLING_TEMPERATURE;


    //Supla::Action::SWITCH_TO_MANUAL_MODE
    case (Supla::Action)88:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_MANUAL_MODE;


    //Supla::Action::SWITCH_TO_WEEKLY_SCHEDULE_MODE
    case (Supla::Action)89:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_WEEKLY_SCHEDULE_MODE;


    //Supla::Action::SWITCH_TO_MANUAL_MODE_HEAT
    case (Supla::Action)90:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_MANUAL_MODE_HEAT;


    //Supla::Action::SWITCH_TO_MANUAL_MODE_COOL
    case (Supla::Action)91:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_MANUAL_MODE_COOL;


    //Supla::Action::SWITCH_TO_MANUAL_MODE_HEAT_COOL
    case (Supla::Action)92:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SWITCH_TO_MANUAL_MODE_HEAT_COOL;


    //Supla::Action::TOGGLE_MANUAL_WEEKLY_SCHEDULE_MODES
    case (Supla::Action)93:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_MANUAL_WEEKLY_SCHEDULE_MODES;


    //Supla::Action::TOGGLE_OFF_MANUAL_WEEKLY_SCHEDULE_MODES
    case (Supla::Action)94:
      return Z2S_SUPLA_ACTION_CORE_ACTION_TOGGLE_OFF_MANUAL_WEEKLY_SCHEDULE_MODES;


    //Supla::Action::SEQUENCE_1_START
    case (Supla::Action)95:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_1_START;


    //Supla::Action::SEQUENCE_1_STOP
    case (Supla::Action)96:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_1_STOP;


    //Supla::Action::SEQUENCE_2_START
    case (Supla::Action)97:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_2_START;


    //Supla::Action::SEQUENCE_2_STOP
    case (Supla::Action)98:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_2_STOP;


    //Supla::Action::SEQUENCE_3_START
    case (Supla::Action)99:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_3_START;


    //Supla::Action::SEQUENCE_3_STOP
    case (Supla::Action)100:
      return Z2S_SUPLA_ACTION_CORE_ACTION_SEQUENCE_3_STOP;


    default:
      return 0;
  }
}
  
Supla::Event convertZ2SEventToSuplaEvent(uint32_t core_supla_event) {

  switch (core_supla_event) {
    
    
    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_PRESS:
      return Supla::Event::ON_PRESS;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_RELEASE:
      return Supla::Event::ON_RELEASE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CHANGE:
      return Supla::Event::ON_CHANGE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_HOLD:
      return Supla::Event::ON_HOLD;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_1:
      return Supla::Event::ON_CLICK_1;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_2:
      return Supla::Event::ON_CLICK_2;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_3:
      return Supla::Event::ON_CLICK_3;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_4:
      return Supla::Event::ON_CLICK_4;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_5:
      return Supla::Event::ON_CLICK_5;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_6:
      return Supla::Event::ON_CLICK_6;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_7:
      return Supla::Event::ON_CLICK_7;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_8:
      return Supla::Event::ON_CLICK_8;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_9:
      return Supla::Event::ON_CLICK_9;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_10:
      return Supla::Event::ON_CLICK_10;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CRAZY_CLICKER:
      return Supla::Event::ON_CRAZY_CLICKER;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_SEQUENCE_MATCH:
      return Supla::Event::ON_SEQUENCE_MATCH;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_SEQUENCE_DOESNT_MATCH:
      return Supla::Event::ON_SEQUENCE_DOESNT_MATCH;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_TURN_ON:
      return Supla::Event::ON_TURN_ON;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_TURN_OFF:
      return Supla::Event::ON_TURN_OFF;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_SECONDARY_CHANNEL_CHANGE:
      return Supla::Event::ON_SECONDARY_CHANNEL_CHANGE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_0:
      return Supla::Event::ON_LONG_CLICK_0;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_1:
      return Supla::Event::ON_LONG_CLICK_1;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_2:
      return Supla::Event::ON_LONG_CLICK_2;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_3:
      return Supla::Event::ON_LONG_CLICK_3;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_4:
      return Supla::Event::ON_LONG_CLICK_4;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_5:
      return Supla::Event::ON_LONG_CLICK_5;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_6:
      return Supla::Event::ON_LONG_CLICK_6;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_7:
      return Supla::Event::ON_LONG_CLICK_7;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_8:
      return Supla::Event::ON_LONG_CLICK_8;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_9:
      return Supla::Event::ON_LONG_CLICK_9;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_10:
      return Supla::Event::ON_LONG_CLICK_10;


    case Z2S_SUPLA_EVENT_CORE_EVENT_CONDITIONAL_ON_PRESS:
      return Supla::Event::CONDITIONAL_ON_PRESS;


    case Z2S_SUPLA_EVENT_CORE_EVENT_CONDITIONAL_ON_RELEASE:
      return Supla::Event::CONDITIONAL_ON_RELEASE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_CONDITIONAL_ON_CHANGE:
      return Supla::Event::CONDITIONAL_ON_CHANGE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_HOLD_RELEASE:
      return Supla::Event::ON_HOLD_RELEASE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_DEVICE_STATUS_CHANGE:
      return Supla::Event::ON_DEVICE_STATUS_CHANGE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_IDENTIFY:
      return Supla::Event::ON_IDENTIFY;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_1:
      return Supla::Event::ON_EVENT_1;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_2:
      return Supla::Event::ON_EVENT_2;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_3:
      return Supla::Event::ON_EVENT_3;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_4:
      return Supla::Event::ON_EVENT_4;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_5:
      return Supla::Event::ON_EVENT_5;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_6:
      return Supla::Event::ON_EVENT_6;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_7:
      return Supla::Event::ON_EVENT_7;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_8:
      return Supla::Event::ON_EVENT_8;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_9:
      return Supla::Event::ON_EVENT_9;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_10:
      return Supla::Event::ON_EVENT_10;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_DIMMER_TURN_ON:
      return Supla::Event::ON_DIMMER_TURN_ON;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_DIMMER_TURN_OFF:
      return Supla::Event::ON_DIMMER_TURN_OFF;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_DIMMER_BRIGHTNESS_CHANGE:
      return Supla::Event::ON_DIMMER_BRIGHTNESS_CHANGE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_COLOR_TURN_ON:
      return Supla::Event::ON_COLOR_TURN_ON;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_COLOR_TURN_OFF:
      return Supla::Event::ON_COLOR_TURN_OFF;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_COLOR_BRIGHTNESS_CHANGE:
      return Supla::Event::ON_COLOR_BRIGHTNESS_CHANGE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_RED_TURN_ON:
      return Supla::Event::ON_RED_TURN_ON;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_RED_TURN_OFF:
      return Supla::Event::ON_RED_TURN_OFF;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_RED_CHANGE:
      return Supla::Event::ON_RED_CHANGE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_GREEN_TURN_ON:
      return Supla::Event::ON_GREEN_TURN_ON;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_GREEN_TURN_OFF:
      return Supla::Event::ON_GREEN_TURN_OFF;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_GREEN_CHANGE:
      return Supla::Event::ON_GREEN_CHANGE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_BLUE_TURN_ON:
      return Supla::Event::ON_BLUE_TURN_ON;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_BLUE_TURN_OFF:
      return Supla::Event::ON_BLUE_TURN_OFF;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_BLUE_CHANGE:
      return Supla::Event::ON_BLUE_CHANGE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_WHITE_TEMPERATURE_CHANGE:
      return Supla::Event::ON_WHITE_TEMPERATURE_CHANGE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CHANNEL_OFFLINE:
      return Supla::Event::ON_CHANNEL_OFFLINE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CHANNEL_ONLINE:
      return Supla::Event::ON_CHANNEL_ONLINE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CHANNEL_FUNCTION_CHANGE:
      return Supla::Event::ON_CHANNEL_FUNCTION_CHANGE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_HEATING:
      return Supla::Event::ON_HVAC_HEATING;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_COOLING:
      return Supla::Event::ON_HVAC_COOLING;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_STANDBY:
      return Supla::Event::ON_HVAC_STANDBY;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_WEEKLY_SCHEDULE_ENABLED:
      return Supla::Event::ON_HVAC_WEEKLY_SCHEDULE_ENABLED;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_WEEKLY_SCHEDULE_DISABLED:
      return Supla::Event::ON_HVAC_WEEKLY_SCHEDULE_DISABLED;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_MODE_OFF:
      return Supla::Event::ON_HVAC_MODE_OFF;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_MODE_HEAT:
      return Supla::Event::ON_HVAC_MODE_HEAT;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_MODE_COOL:
      return Supla::Event::ON_HVAC_MODE_COOL;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_MODE_HEAT_COOL:
      return Supla::Event::ON_HVAC_MODE_HEAT_COOL;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_ERROR:
      return Supla::Event::ON_ERROR;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_ERROR_CLEAR:
      return Supla::Event::ON_ERROR_CLEAR;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_ALARM_ACTIVE:
      return Supla::Event::ON_CONTAINER_ALARM_ACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_ALARM_INACTIVE:
      return Supla::Event::ON_CONTAINER_ALARM_INACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_WARNING_ACTIVE:
      return Supla::Event::ON_CONTAINER_WARNING_ACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_WARNING_INACTIVE:
      return Supla::Event::ON_CONTAINER_WARNING_INACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_INVALID_SENSOR_STATE_ACTIVE:
      return Supla::Event::ON_CONTAINER_INVALID_SENSOR_STATE_ACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_INVALID_SENSOR_STATE_INACTIVE:
      return Supla::Event::ON_CONTAINER_INVALID_SENSOR_STATE_INACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SOUND_ALARM_ACTIVE:
      return Supla::Event::ON_CONTAINER_SOUND_ALARM_ACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SOUND_ALARM_INACTIVE:
      return Supla::Event::ON_CONTAINER_SOUND_ALARM_INACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SENSOR_OFFLINE_ACTIVE:
      return Supla::Event::ON_CONTAINER_SENSOR_OFFLINE_ACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SENSOR_OFFLINE_INACTIVE:
      return Supla::Event::ON_CONTAINER_SENSOR_OFFLINE_INACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SOUND_ALARM_MUTED:
      return Supla::Event::ON_CONTAINER_SOUND_ALARM_MUTED;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_OPEN:
      return Supla::Event::ON_OPEN;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLOSE:
      return Supla::Event::ON_CLOSE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_FLOODING_ACTIVE:
      return Supla::Event::ON_FLOODING_ACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_FLOODING_INACTIVE:
      return Supla::Event::ON_FLOODING_INACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_VALVE_MANUALLY_CLOSED_ACTIVE:
      return Supla::Event::ON_VALVE_MANUALLY_CLOSED_ACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_VALVE_MANUALLY_CLOSED_INACTIVE:
      return Supla::Event::ON_VALVE_MANUALLY_CLOSED_INACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_MOTOR_PROBLEM_ACTIVE:
      return Supla::Event::ON_MOTOR_PROBLEM_ACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_MOTOR_PROBLEM_INACTIVE:
      return Supla::Event::ON_MOTOR_PROBLEM_INACTIVE;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_SUBDEVICE_PAIRING_START:
      return Supla::Event::ON_SUBDEVICE_PAIRING_START;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_SUBDEVICE_PAIRING_END:
      return Supla::Event::ON_SUBDEVICE_PAIRING_END;


    case Z2S_SUPLA_EVENT_CORE_EVENT_ON_IMPULSE:
      return Supla::Event::ON_IMPULSE;


    default:
      return (Supla::Event)core_supla_event;
  }
}

uint32_t convertSuplaEventToZ2SEvent(Supla::Event supla_event) {
  
  switch (supla_event) {
    
    
    //Supla::Event::ON_PRESS
    case (Supla::Event)0:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_PRESS;


    //Supla::Event::ON_RELEASE
    case (Supla::Event)1:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_RELEASE;


    //Supla::Event::ON_CHANGE
    case (Supla::Event)2:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CHANGE;


    //Supla::Event::ON_HOLD
    case (Supla::Event)3:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_HOLD;


    //Supla::Event::ON_CLICK_1
    case (Supla::Event)4:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_1;


    //Supla::Event::ON_CLICK_2
    case (Supla::Event)5:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_2;


    //Supla::Event::ON_CLICK_3
    case (Supla::Event)6:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_3;


    //Supla::Event::ON_CLICK_4
    case (Supla::Event)7:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_4;


    //Supla::Event::ON_CLICK_5
    case (Supla::Event)8:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_5;


    //Supla::Event::ON_CLICK_6
    case (Supla::Event)9:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_6;


    //Supla::Event::ON_CLICK_7
    case (Supla::Event)10:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_7;


    //Supla::Event::ON_CLICK_8
    case (Supla::Event)11:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_8;


    //Supla::Event::ON_CLICK_9
    case (Supla::Event)12:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_9;


    //Supla::Event::ON_CLICK_10
    case (Supla::Event)13:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLICK_10;


    //Supla::Event::ON_CRAZY_CLICKER
    case (Supla::Event)14:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CRAZY_CLICKER;


    //Supla::Event::ON_SEQUENCE_MATCH
    case (Supla::Event)15:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_SEQUENCE_MATCH;


    //Supla::Event::ON_SEQUENCE_DOESNT_MATCH
    case (Supla::Event)16:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_SEQUENCE_DOESNT_MATCH;


    //Supla::Event::ON_TURN_ON
    case (Supla::Event)17:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_TURN_ON;


    //Supla::Event::ON_TURN_OFF
    case (Supla::Event)18:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_TURN_OFF;


    //Supla::Event::ON_SECONDARY_CHANNEL_CHANGE
    case (Supla::Event)19:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_SECONDARY_CHANNEL_CHANGE;


    //Supla::Event::ON_LONG_CLICK_0
    case (Supla::Event)20:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_0;


    //Supla::Event::ON_LONG_CLICK_1
    case (Supla::Event)21:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_1;


    //Supla::Event::ON_LONG_CLICK_2
    case (Supla::Event)22:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_2;


    //Supla::Event::ON_LONG_CLICK_3
    case (Supla::Event)23:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_3;


    //Supla::Event::ON_LONG_CLICK_4
    case (Supla::Event)24:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_4;


    //Supla::Event::ON_LONG_CLICK_5
    case (Supla::Event)25:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_5;


    //Supla::Event::ON_LONG_CLICK_6
    case (Supla::Event)26:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_6;


    //Supla::Event::ON_LONG_CLICK_7
    case (Supla::Event)27:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_7;


    //Supla::Event::ON_LONG_CLICK_8
    case (Supla::Event)28:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_8;


    //Supla::Event::ON_LONG_CLICK_9
    case (Supla::Event)29:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_9;


    //Supla::Event::ON_LONG_CLICK_10
    case (Supla::Event)30:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_LONG_CLICK_10;


    //Supla::Event::CONDITIONAL_ON_PRESS
    case (Supla::Event)31:
      return Z2S_SUPLA_EVENT_CORE_EVENT_CONDITIONAL_ON_PRESS;


    //Supla::Event::CONDITIONAL_ON_RELEASE
    case (Supla::Event)32:
      return Z2S_SUPLA_EVENT_CORE_EVENT_CONDITIONAL_ON_RELEASE;


    //Supla::Event::CONDITIONAL_ON_CHANGE
    case (Supla::Event)33:
      return Z2S_SUPLA_EVENT_CORE_EVENT_CONDITIONAL_ON_CHANGE;


    //Supla::Event::ON_HOLD_RELEASE
    case (Supla::Event)34:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_HOLD_RELEASE;


    //Supla::Event::ON_DEVICE_STATUS_CHANGE
    case (Supla::Event)35:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_DEVICE_STATUS_CHANGE;


    //Supla::Event::ON_IDENTIFY
    case (Supla::Event)36:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_IDENTIFY;


    //Supla::Event::ON_EVENT_1
    case (Supla::Event)37:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_1;


    //Supla::Event::ON_EVENT_2
    case (Supla::Event)38:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_2;


    //Supla::Event::ON_EVENT_3
    case (Supla::Event)39:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_3;


    //Supla::Event::ON_EVENT_4
    case (Supla::Event)40:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_4;


    //Supla::Event::ON_EVENT_5
    case (Supla::Event)41:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_5;


    //Supla::Event::ON_EVENT_6
    case (Supla::Event)42:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_6;


    //Supla::Event::ON_EVENT_7
    case (Supla::Event)43:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_7;


    //Supla::Event::ON_EVENT_8
    case (Supla::Event)44:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_8;


    //Supla::Event::ON_EVENT_9
    case (Supla::Event)45:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_9;


    //Supla::Event::ON_EVENT_10
    case (Supla::Event)46:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_EVENT_10;


    //Supla::Event::ON_DIMMER_TURN_ON
    case (Supla::Event)47:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_DIMMER_TURN_ON;


    //Supla::Event::ON_DIMMER_TURN_OFF
    case (Supla::Event)48:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_DIMMER_TURN_OFF;


    //Supla::Event::ON_DIMMER_BRIGHTNESS_CHANGE
    case (Supla::Event)49:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_DIMMER_BRIGHTNESS_CHANGE;


    //Supla::Event::ON_COLOR_TURN_ON
    case (Supla::Event)50:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_COLOR_TURN_ON;


    //Supla::Event::ON_COLOR_TURN_OFF
    case (Supla::Event)51:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_COLOR_TURN_OFF;


    //Supla::Event::ON_COLOR_BRIGHTNESS_CHANGE
    case (Supla::Event)52:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_COLOR_BRIGHTNESS_CHANGE;


    //Supla::Event::ON_RED_TURN_ON
    case (Supla::Event)53:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_RED_TURN_ON;


    //Supla::Event::ON_RED_TURN_OFF
    case (Supla::Event)54:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_RED_TURN_OFF;


    //Supla::Event::ON_RED_CHANGE
    case (Supla::Event)55:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_RED_CHANGE;


    //Supla::Event::ON_GREEN_TURN_ON
    case (Supla::Event)56:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_GREEN_TURN_ON;


    //Supla::Event::ON_GREEN_TURN_OFF
    case (Supla::Event)57:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_GREEN_TURN_OFF;


    //Supla::Event::ON_GREEN_CHANGE
    case (Supla::Event)58:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_GREEN_CHANGE;


    //Supla::Event::ON_BLUE_TURN_ON
    case (Supla::Event)59:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_BLUE_TURN_ON;


    //Supla::Event::ON_BLUE_TURN_OFF
    case (Supla::Event)60:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_BLUE_TURN_OFF;


    //Supla::Event::ON_BLUE_CHANGE
    case (Supla::Event)61:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_BLUE_CHANGE;


    //Supla::Event::ON_WHITE_TEMPERATURE_CHANGE
    case (Supla::Event)62:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_WHITE_TEMPERATURE_CHANGE;


    //Supla::Event::ON_CHANNEL_OFFLINE
    case (Supla::Event)63:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CHANNEL_OFFLINE;


    //Supla::Event::ON_CHANNEL_ONLINE
    case (Supla::Event)64:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CHANNEL_ONLINE;


    //Supla::Event::ON_CHANNEL_FUNCTION_CHANGE
    case (Supla::Event)65:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CHANNEL_FUNCTION_CHANGE;


    //Supla::Event::ON_HVAC_HEATING
    case (Supla::Event)66:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_HEATING;


    //Supla::Event::ON_HVAC_COOLING
    case (Supla::Event)67:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_COOLING;


    //Supla::Event::ON_HVAC_STANDBY
    case (Supla::Event)68:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_STANDBY;


    //Supla::Event::ON_HVAC_WEEKLY_SCHEDULE_ENABLED
    case (Supla::Event)69:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_WEEKLY_SCHEDULE_ENABLED;


    //Supla::Event::ON_HVAC_WEEKLY_SCHEDULE_DISABLED
    case (Supla::Event)70:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_WEEKLY_SCHEDULE_DISABLED;


    //Supla::Event::ON_HVAC_MODE_OFF
    case (Supla::Event)71:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_MODE_OFF;


    //Supla::Event::ON_HVAC_MODE_HEAT
    case (Supla::Event)72:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_MODE_HEAT;


    //Supla::Event::ON_HVAC_MODE_COOL
    case (Supla::Event)73:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_MODE_COOL;


    //Supla::Event::ON_HVAC_MODE_HEAT_COOL
    case (Supla::Event)74:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_HVAC_MODE_HEAT_COOL;


    //Supla::Event::ON_ERROR
    case (Supla::Event)75:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_ERROR;


    //Supla::Event::ON_ERROR_CLEAR
    case (Supla::Event)76:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_ERROR_CLEAR;


    //Supla::Event::ON_CONTAINER_ALARM_ACTIVE
    case (Supla::Event)77:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_ALARM_ACTIVE;


    //Supla::Event::ON_CONTAINER_ALARM_INACTIVE
    case (Supla::Event)78:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_ALARM_INACTIVE;


    //Supla::Event::ON_CONTAINER_WARNING_ACTIVE
    case (Supla::Event)79:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_WARNING_ACTIVE;


    //Supla::Event::ON_CONTAINER_WARNING_INACTIVE
    case (Supla::Event)80:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_WARNING_INACTIVE;


    //Supla::Event::ON_CONTAINER_INVALID_SENSOR_STATE_ACTIVE
    case (Supla::Event)81:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_INVALID_SENSOR_STATE_ACTIVE;


    //Supla::Event::ON_CONTAINER_INVALID_SENSOR_STATE_INACTIVE
    case (Supla::Event)82:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_INVALID_SENSOR_STATE_INACTIVE;


    //Supla::Event::ON_CONTAINER_SOUND_ALARM_ACTIVE
    case (Supla::Event)83:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SOUND_ALARM_ACTIVE;


    //Supla::Event::ON_CONTAINER_SOUND_ALARM_INACTIVE
    case (Supla::Event)84:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SOUND_ALARM_INACTIVE;


    //Supla::Event::ON_CONTAINER_SENSOR_OFFLINE_ACTIVE
    case (Supla::Event)85:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SENSOR_OFFLINE_ACTIVE;


    //Supla::Event::ON_CONTAINER_SENSOR_OFFLINE_INACTIVE
    case (Supla::Event)86:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SENSOR_OFFLINE_INACTIVE;


    //Supla::Event::ON_CONTAINER_SOUND_ALARM_MUTED
    case (Supla::Event)87:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CONTAINER_SOUND_ALARM_MUTED;


    //Supla::Event::ON_OPEN
    case (Supla::Event)88:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_OPEN;


    //Supla::Event::ON_CLOSE
    case (Supla::Event)89:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_CLOSE;


    //Supla::Event::ON_FLOODING_ACTIVE
    case (Supla::Event)90:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_FLOODING_ACTIVE;


    //Supla::Event::ON_FLOODING_INACTIVE
    case (Supla::Event)91:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_FLOODING_INACTIVE;


    //Supla::Event::ON_VALVE_MANUALLY_CLOSED_ACTIVE
    case (Supla::Event)92:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_VALVE_MANUALLY_CLOSED_ACTIVE;


    //Supla::Event::ON_VALVE_MANUALLY_CLOSED_INACTIVE
    case (Supla::Event)93:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_VALVE_MANUALLY_CLOSED_INACTIVE;


    //Supla::Event::ON_MOTOR_PROBLEM_ACTIVE
    case (Supla::Event)94:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_MOTOR_PROBLEM_ACTIVE;


    //Supla::Event::ON_MOTOR_PROBLEM_INACTIVE
    case (Supla::Event)95:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_MOTOR_PROBLEM_INACTIVE;


    //Supla::Event::ON_SUBDEVICE_PAIRING_START
    case (Supla::Event)96:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_SUBDEVICE_PAIRING_START;


    //Supla::Event::ON_SUBDEVICE_PAIRING_END
    case (Supla::Event)97:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_SUBDEVICE_PAIRING_END;


    //Supla::Event::ON_IMPULSE
    case (Supla::Event)98:
      return Z2S_SUPLA_EVENT_CORE_EVENT_ON_IMPULSE;


    default:
      return 0;
  }
}