/**
 *******************************************************************************
 *  @file   CAN_callbacks.c
 *  @brief  Source file for the CAN callback functions of the Steering.
 *          BME Formula Racing Team - Electronics Team
 *  @date   2022.02.03.
 *  @author Nandor Fafula
 *  @version 1.0
 *
 *  <B> Changelog: <BR></B>
 *
 *******************************************************************************
 */

#include "CAN_callbacks.h"

Comm_Layer_Message_t critical_cell_values = {0};
Comm_Layer_Message_t general_information = {0};
Comm_Layer_Message_t tcu_status_message = {0};

uint8_t appearance_count_general_info = 0u;

void Lowest_Cell_Voltage_Callback(uint8_t interface, Comm_Layer_Message_t *message)
{
    critical_cell_values.id = message->id;
    critical_cell_values.length = message->length;
    critical_cell_values.data[0] = message->data[0];
    critical_cell_values.data[1] = message->data[1];
    critical_cell_values.data[2] = message->data[2];
    critical_cell_values.data[3] = message->data[3];
    critical_cell_values.data[4] = message->data[4];
    critical_cell_values.data[5] = message->data[5];
    critical_cell_values.data[6] = message->data[6];
    critical_cell_values.data[7] = message->data[7];
}

void General_Information_Callback(uint8_t interface, Comm_Layer_Message_t *message)
{
    general_information.id = message->id;
    general_information.length = message->length;
    general_information.data[0] = message->data[0];
    general_information.data[1] = message->data[1];
    general_information.data[2] = message->data[2];
    general_information.data[3] = message->data[3];
    general_information.data[4] = message->data[4];
    general_information.data[5] = message->data[5];
    general_information.data[6] = message->data[6];
    general_information.data[7] = message->data[7];
    appearance_count_general_info = 0;
    appearance_count_general_info++;
}

void Used_Energy_Per_Lap_Callback(uint8_t interface, Comm_Layer_Message_t *message)
{
    tcu_status_message.id = message->id;
    tcu_status_message.length = message->length;
    tcu_status_message.data[0] = message->data[0];
    tcu_status_message.data[1] = message->data[1];
    tcu_status_message.data[2] = message->data[2];
    tcu_status_message.data[3] = message->data[3];
    tcu_status_message.data[4] = message->data[4];
    tcu_status_message.data[5] = message->data[5];
    tcu_status_message.data[6] = message->data[6];
    tcu_status_message.data[7] = message->data[7];
}
