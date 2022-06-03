/**
 *******************************************************************************
 *  @file   CAN_callbacks.h
 *  @brief  Header file for the CAN callback functions of the Steering.
 *          BME Formula Racing Team - Electronics Team
 *  @date   2022.02.03.
 *  @author Nandor Fafula
 *  @version 1.0
 *
 *  <B> Changelog: <BR></B>
 *
 *******************************************************************************
 */

#ifndef FRT_LIBRARY_ECU_LAYER_CAN_CALLBACKS_H_
#define FRT_LIBRARY_ECU_LAYER_CAN_CALLBACKS_H_

#include "STAMP.h"

extern Comm_Layer_Message_t critical_cell_values;
extern Comm_Layer_Message_t general_information;
extern Comm_Layer_Message_t tcu_status_message;

extern uint8_t appearance_count_general_info;

void Lowest_Cell_Voltage_Callback(uint8_t interface, Comm_Layer_Message_t *message);
void General_Information_Callback(uint8_t interface, Comm_Layer_Message_t *message);
void Used_Energy_Per_Lap_Callback(uint8_t interface, Comm_Layer_Message_t *message);

#endif
