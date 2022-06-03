/**
 *******************************************************************************
 *  @file   Steering.h
 *  @brief  Steering modul  basic header. All Steering projects need to implement ECU_Main function!
 *          <b>BME Formula Racing Team - Electronics Team</b>
 *  @date   2022.01.
 *  @author Nandor Fafula
 *  @version 0.21
 *
 *  <B> Changelog: <BR></B>
 *******************************************************************************
 */

#ifndef STEERING_0_21_FRT_LIBRARY_ECU_LAYER_STEERING_H_
#define STEERING_0_21_FRT_LIBRARY_ECU_LAYER_STEERING_H_

#include "STAMP.h"
#include "HID.h"
#include "CAN_callbacks.h"

/*-------------------- CAN defines --------------------*/
/* Received CAN message IDs */
#define CANID_R_CRITICAL_CELL_VALUES	(0x313u)		//!< Received CAN message ID for Critical Cell Values message
#define CANID_R_GENERAL_INFORMATION     (0x369u)        //!< Received CAN message ID for General Information message
#define CANID_R_USED_ENERGY_PER_LAP     (0x486u)        //!< Received CAN message ID for Used Energy Per Lap message from TCU

/* Received CAN message interfaces */
#define CANBUS_R_CRITICAL_CELL_VALUES	STAMP_CAN1		//!< Received CAN message Interface for Critical Cell Values message
#define CANBUS_R_GENERAL_INFORMATION    STAMP_CAN1      //!< Received CAN message Interface for General Information message
#define CANBUS_R_USED_ENERGY_PER_LAP    STAMP_CAN1      //!< Received CAN message Interface for Used Energy Per Lap message from TCU

/* Send CAN message IDs */
#define CANID_S_STEERING_CONTROL		(0x290u) 		//!< Sent CAN message ID for Steering Control message

/* Send CAN message interfaces */
#define CANBUS_S_STEERING_CONTROL		STAMP_CAN1		//!< Sent CAN message Interface for Steering Control message

/* Send CAN message length */
#define CANLENGTH_S_STEERING_CONTROL	((uint8_t)4u)	//!< Steering Control CAN message length

/*-------------------- Functions --------------------*/
/* All ECU projects need to implement ECU_Main function! */
void ECU_main(void); 									//!< ECU_main function
void RotaryButtonToCAN(void);                           //!< Sends the button and rotary states to CAN
void Display_On_14Segment_Display(void);
void Display_On_LEDs(void);                             //!< Manages the LEDs on the steering wheel

#endif /* STEERING_0_21_FRT_LIBRARY_ECU_LAYER_STEERING_H_ */

