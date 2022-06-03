/**
 *******************************************************************************
 *  @file 	Steering.c
 *  @brief 	Steering modul  basic header. All Steering projects need to implement ECU_Main function!
 *  		<b>BME Formula Racing Team - Electronics Team</b>
 *	@date 	2022.01.
 *  @author Nandor Fafula
 *  @version 0.21
 *
 *  <B> Changelog: <BR></B>
 *******************************************************************************
 */
/*-------------------- Includes --------------------*/
#include "Steering.h"

extern 	uint8_t ButtonStates[ButtonN]; 		//Output array of buttons
extern 	uint8_t RotaryPositions[RotaryN];	//Output array of rotary switches

static Comm_Layer_Message_t message;

void ECU_main(void)
{
/*-------------------- Registering CAN Rx --------------------*/
	/* Critical Cell Values */
	message.id = CANID_R_CRITICAL_CELL_VALUES;
	comm_layer_registrate_Rx_message(CANBUS_R_CRITICAL_CELL_VALUES, &message, 0u, &Lowest_Cell_Voltage_Callback);

    /* General Information from CAN */
    message.id = CANID_R_GENERAL_INFORMATION;
    comm_layer_registrate_Rx_message(CANBUS_R_GENERAL_INFORMATION, &message, 0u, &General_Information_Callback);

	/* TCU Status 2 */
	message.id = CANID_R_USED_ENERGY_PER_LAP;
	comm_layer_registrate_Rx_message(CANBUS_R_USED_ENERGY_PER_LAP, &message, 0u, &Used_Energy_Per_Lap_Callback);

/*-------------------- Registering CAN Tx --------------------*/
	/* Steering Control */
	message.id = CANID_S_STEERING_CONTROL;
	message.length = CANLENGTH_S_STEERING_CONTROL;
	comm_layer_registrate_Tx_message(CANBUS_S_STEERING_CONTROL, &message, 0, 0);

/*-------------------- Initializer functions --------------------*/
	/* Running a test on the display */
	testDisplay();

	/* Initializing display */
	initDisplay();

	/*Set LED brightness*/
	//uint8_t redBright[LedN] = {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127};
	//uint8_t greenBright[LedN] = {127,127,127,32,32,32,32,32,32,32,32,32,32,127,127,127};
	//uint8_t blueBright[LedN] = {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127};
	//setLEDBright(redBright, greenBright, blueBright);

/*-------------------- Registering tasks --------------------*/
	RTOS_layer_registrate("Display_On_14Segment_Display", Display_On_14Segment_Display, _100MS_TIME, 1);
	RTOS_layer_registrate("Display_On_LEDs", Display_On_LEDs, _100MS_TIME, 1);
    RTOS_layer_registrate("RotaryButtonToCAN", RotaryButtonToCAN, _100MS_TIME, 1);
}

void Display_On_14Segment_Display(void)
{
    char digits[DigitN];
    uint8_t decimal_points[DigitN] = {0u,0u,0u,0u,0u,0u,0u,0u};
    uint8_t plane = 0u;

    uint16_t critical_cell_voltage = 0u;
    uint16_t state_of_charge = 0u;
    static uint8_t heartbeat = 0u;
    static uint16_t heartbeat_error = 0u;

    if(appearance_count_general_info != 0)
    {
        digits[0] = general_information.data[0];
        digits[1] = general_information.data[1];
        digits[2] = general_information.data[2];
        digits[3] = general_information.data[3];
        digits[4] = general_information.data[4];
        digits[5] = general_information.data[5];
        digits[6] = general_information.data[6];
        digits[7] = general_information.data[7];

        appearance_count_general_info++;

        if(appearance_count_general_info >= 25)
        {
            appearance_count_general_info = 0;
        }
    }
    else if(appearance_count_general_info == 0)
    {
        if(heartbeat == critical_cell_values.data[0])
        {
            heartbeat_error++;
        }
        else
        {
            heartbeat_error = 0u;
        }
        heartbeat = critical_cell_values.data[0];

        critical_cell_voltage = ((critical_cell_values.data[1] << 8u ) | (critical_cell_values.data[2] ));
        state_of_charge = ((critical_cell_values.data[5] << 8u ) | (critical_cell_values.data[6]));

        if (heartbeat_error > 25)
        {
            digits[0] = 'B';
            digits[1] = 'A';
            digits[2] = 'T';
            digits[3] = 'T';
            digits[4] = ' ';
            digits[5] = 'E';
            digits[6] = 'R';
            digits[7] = 'R';
        }
        else
        {
            getBut();
            if(ButtonStates[3] == 1u)
            {
                digits[0] = ' ';
                digits[1] = (state_of_charge / 10000);
                digits[2] = ((state_of_charge / 1000) % 10);
                digits[3] = ((state_of_charge / 100)   % 10);
                digits[4] = ((state_of_charge / 10)    % 10);
                digits[5] = (state_of_charge % 10);
                digits[6] = ' ';
                digits[7] = ' ';
                decimal_points[2] = 1;
            }
            else
            {
                digits[0] = ' ';
                digits[1] = ' ';
                digits[2] = (critical_cell_voltage / 1000) + 48;
                digits[3] = (critical_cell_voltage / 100 % 10) + 48;
                digits[4] = (critical_cell_voltage / 10 % 10) + 48;
                digits[5] = (critical_cell_voltage % 10) + 48;
                digits[6] = ' ';
                digits[7] = 'V';
                decimal_points[2] = 1;
            }
        }
    }
    printDisplay(digits, decimal_points, plane);
}

void Display_On_LEDs(void)
{
/* Displaying approximate battery charge and warning on low cell voltage */

    /* Variables */
    uint16_t critical_cell_voltage = 0u;
    static uint8_t used_energy_in_prev_lap = 0u;
    static uint8_t new_lap = 1u;

    uint16_t redLED = 0u;
    uint16_t greenLED = 0u;
    uint16_t blueLED = 0u;

    static uint8_t heartbeat = 0u;
    static uint16_t heartbeat_error = 0u;
    static uint8_t alert_blink_counter = 0u;
    static uint8_t used_energy_blink_counter = 0u;
    static uint8_t used_energy_blink_periods = 0u;

    /* Checking for heartbeat error */
    if(heartbeat == critical_cell_values.data[0])
    {
        heartbeat_error++;
    }
    else
    {
        heartbeat_error = 0u;
    }
    heartbeat = critical_cell_values.data[0];

    /* Reading critical cell voltage */
    critical_cell_voltage = ((critical_cell_values.data[1] << 8u ) | (critical_cell_values.data[2] ));

    /* Use binary numbers for the LEDs as it is descriptive of the states */
    if(heartbeat_error < 25)
    {
        if(critical_cell_voltage > 4000)
        {
            redLED = 0u;
            greenLED = 0b0001111111111000u;
            blueLED = 0u;
        }
        else if(critical_cell_voltage > 3950)
        {
            redLED = 0u;
            greenLED = 0b0000111111110000u;
            blueLED = 0u;
        }
        else if(critical_cell_voltage > 3900)
        {
            redLED = 0u;
            greenLED = 0b0000011111100000u;
            blueLED = 0u;
        }
        else if(critical_cell_voltage > 3850)
        {
            redLED = 0u;
            greenLED = 0b0000001111000000u;
            blueLED = 0u;
        }
        else if(critical_cell_voltage > 3800)
        {
            redLED = 0u;
            greenLED = 0b0000000110000000u;
            blueLED = 0u;
        }
        else if(critical_cell_voltage > 3750)
        {
            redLED = 0u;
            greenLED = 0u;
            blueLED = 0b0001111111111000u;
        }
        else if(critical_cell_voltage > 3700)
        {
            redLED = 0u;
            greenLED = 0u;
            blueLED = 0b0000111111110000u;
        }
        else if(critical_cell_voltage > 3650)
        {
            redLED = 0u;
            greenLED = 0u;
            blueLED = 0b0000011111100000u;
        }
        else if(critical_cell_voltage > 3600)
        {
            redLED = 0u;
            greenLED = 0u;
            blueLED = 0b0000001111000000u;
        }
        else if(critical_cell_voltage > 3550)
        {
            redLED = 0u;
            greenLED = 0u;
            blueLED = 0b0000000110000000u;
        }
        else if(critical_cell_voltage > 3500)
        {
            redLED = 0b0001000000001000u;
            greenLED = 0u;
            blueLED = 0u;
        }
        else if(critical_cell_voltage > 3450)
        {
            redLED = 0b0001100000011000u;
            greenLED = 0u;
            blueLED = 0u;
        }
        else if(critical_cell_voltage > 3400)
        {
            redLED = 0b0001110000111000u;
            greenLED = 0u;
            blueLED = 0u;
        }
        else if(critical_cell_voltage > 3350)
        {
            redLED = 0b0001111001111000u;
            greenLED = 0u;
            blueLED = 0u;
        }
        else if(critical_cell_voltage > 3300)
        {
            redLED = 0b0001111111111000u;
            greenLED = 0u;
            blueLED = 0u;
        }
        else if(critical_cell_voltage > 3250)
        {
            redLED = 0b0001111111111000u;
            greenLED = 0u;
            blueLED = 0u;
        }
        else if(critical_cell_voltage > 3200)
        {
            if(alert_blink_counter < 10)
            {
                redLED = 0b0001111111111000u;
                greenLED = 0u;
                blueLED = 0u;
            }
            else
            {
                redLED = 0u;
                greenLED = 0u;
                blueLED = 0u;
                if(alert_blink_counter > 19)
                {
                    alert_blink_counter = 0u;
                }
            }
        }
        else if(critical_cell_voltage < 3200)
        {
            if(alert_blink_counter < 2)
            {
                redLED = 0b0001111111111000u;
                greenLED = 0u;
                blueLED = 0u;
            }
            else
            {
                redLED = 0u;
                greenLED = 0u;
                blueLED = 0u;
                if(alert_blink_counter > 3)
                {
                    alert_blink_counter = 0u;
                }
            }
        }
    }
    else if((appearance_count_general_info != 0) || (heartbeat_error >= 25))
    {
        redLED = 0u;
        greenLED = 0u;
        blueLED = 0u;
    }

    /* Displaying used energy in the previous lap on LEDs. */
    if((used_energy_in_prev_lap != tcu_status_message.data[7]) && (new_lap == 1u))
    {
        used_energy_in_prev_lap = tcu_status_message.data[7];
        new_lap = 0u;
    }
    if((used_energy_in_prev_lap == tcu_status_message.data[7]) && (new_lap == 0u))
    {
        if(used_energy_blink_counter < 5)
        {
            if(used_energy_in_prev_lap > 105)
            {
                redLED = 0b1111111111111111u;
                greenLED = 0u;
                blueLED = 0u;
            }
            else if(used_energy_in_prev_lap >= 95 && used_energy_in_prev_lap <= 105)
            {
                redLED = 0u;
                greenLED = 0u;
                blueLED = 0b1111111111111111u;
            }
            else if(used_energy_in_prev_lap < 95)
            {
                redLED = 0u;
                greenLED = 0b1111111111111111u;
                blueLED = 0u;
            }
        }
        else
        {
            redLED = 0u;
            greenLED = 0u;
            blueLED = 0u;
            if(used_energy_blink_counter > 10)
            {
                used_energy_blink_periods++;
                used_energy_blink_counter = 0u;
            }
        }
        if(used_energy_blink_periods >= 5)
        {
            used_energy_blink_periods = 0u;
            new_lap = 1u;
            used_energy_in_prev_lap = 0u;
            tcu_status_message.data[7] = 0u;
        }
    }


    /* Incrementing counters, setting the LEDs */
    alert_blink_counter++;
    used_energy_blink_counter++;
    setLEDState(redLED,greenLED,blueLED);
}

/* Sending Rotary switch and Button states to CAN */
void RotaryButtonToCAN(void)
{
/* Working copy directly taken from Steering_ECU_0.11 */

	/* Reading the steering wheel state */
	getBut();
	getRot();

	/* CAN variables */
	Comm_Layer_Message_t message_steering_control_send;
	message_steering_control_send.id = CANID_S_STEERING_CONTROL;
	message_steering_control_send.length = CANLENGTH_S_STEERING_CONTROL;

	/* Variables */
	static uint8_t heartbeat = 0u;
	uint8_t byte1 = 0u;
	uint8_t byte2 = 0u;
	uint8_t byte3 = 0u;

	/* Heartbeat */
	heartbeat++;
	if(heartbeat == 255u)
	{
		heartbeat = 0u;
	}

	/* Writing data to byte 1 */
	byte1 |= ButtonStates[0] 	<< 7;   //Button 1 state
	byte1 |= ButtonStates[1] 	<< 6;   //Button 2 state
	byte1 |= ButtonStates[2] 	<< 5;   //Button 3 state
	byte1 |= ButtonStates[3] 	<< 4;   //Button 4 state
	byte1 |= RotaryPositions[0] << 0;   //Rotary 1 position

	/* Writing data to byte 2 */
	byte2 |= RotaryPositions[1] << 4; // Rotary 2 position
	byte2 |= RotaryPositions[2] << 0; // Rotary 3 position

	/* Writing data to byte 3 */
	byte3 |= RotaryPositions[3] << 4; // Rotary 4 position
	byte3 |= RotaryPositions[4] << 0; // Rotary 5 position

	/* Sending message */
	message_steering_control_send.data[0] = heartbeat;
	message_steering_control_send.data[1] = byte1;
	message_steering_control_send.data[2] = byte2;
	message_steering_control_send.data[3] = byte3;

    /* Sending CAN message */
    comm_layer_send_message(CANBUS_S_STEERING_CONTROL, &message_steering_control_send);
}
