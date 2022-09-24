/**
  ******************************************************************************
  * @file	: bms_slave_ubt.cpp
  * @brief	: Slave Software for Ubetter BMS
  * @author	: Muhammed Emin CELIK
  * @date	: 17.01.2018
  * @version: 0.1.0
 *******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 Makerland A.S.,
  * All Rights Reserved </center></h2>
  *
  * All  information  contained  herein is,  and  remains  the property of
  * Makerland A.S.The intellectual and technical concepts contained herein
  * are proprietary  to  Makerland A.S. and are protected  by trade secret
  * or copyright law.  Dissemination of this  information or  reproduction
  * of this material is strictly forbidden unless prior written permission
  * is obtained from   Makerland A.S.  Access to the source code contained
  * herein is  hereby forbidden  to  anyone  except current Makerland A.S.
  * employees, managers or contractors  who have executed  Confidentiality
  * and Non-disclosure agreements explicitly covering such access.
  *
 *******************************************************************************
  */

#include <bms_slave_ubt.hpp>
#include <cstring>


/**
  * @brief	Class object
  */
Battery::Ubtbat::BMS_SLAVE_UBT ubetter();

namespace Battery
{

namespace Ubtbat
{



/*|Frame Structure|**********************************************************************************************

Request
---------------------------------------------------------------------------------------------------------------
Start Bit	Status Bit	     Command Code 	 Length			Checksum	     Stop Bit
---------------------------------------------------------------------------------------------------------------
  0xDD          0xA5(read)          	0X03		  0x00		 	 2Bytes		       0x77

                0X5A(write)         	0X04

                                    	0X05
*****************************************************************************************************************
Response
---------------------------------------------------------------------------------------------------------------
Start Bit       Command Code	      Status Bit         Length	        Payload      Checksum        Stop Bit
---------------------------------------------------------------------------------------------------------------
  0xDD              0X03       	   0x00(correct)                                      2Bytes           0x77

                    0X04            0X80(error)

                    0X05
*****************************************************************************************************************/



const uint8_t START_BIT			= 0XDD;
const uint8_t STOP_BIT			= 0X77;

const uint8_t STATUS_BIT_READ		= 0XA5;
const uint8_t STATUS_BIT_WRITE		= 0X5A;

const uint8_t STATUS_CORRECT		= 0X00;
const uint8_t STATUS_ERROR		= 0X80;

const uint8_t COMMAND_CODE_INFO		= 0x03;
const uint8_t COMMAND_CODE_CELL		= 0x04;
const uint8_t COMMAND_CODE_VERS		= 0X05;

const uint8_t REQUEST_LENGTH		= 0X00;



/**
  * @brief 	Default constructor
  * @param[in]  void
  * @return 	void
  */
BMS_SLAVE_UBT::BMS_SLAVE_UBT()
{ }



/**
  * @brief 	Initialize function, runs only once
  * @param[in]  void
  * @return 	void
  */
void BMS_SLAVE_UBT::initialize(void)
{

}



/**
  * @brief 	Request Send Function, runs with request
  * @param[in]  uint8_t status_bit	:
  * @param[in]  uint8_t command_code 	:
  * @return 	void
  */
void BMS_SLAVE_UBT::requestSend(uint8_t status_bit, uint8_t command_code)
{
	bms_ubetter_request_type bms_request_type;

	bms_request_type.data.start_bit			= START_BIT;
	bms_request_type.data.status_bit	  	= status_bit;
	bms_request_type.data.command_code 	 	= command_code;
	bms_request_type.data.data_length	 	= REQUEST_LENGTH;
	bms_request_type.data.stop_bit			= STOP_BIT;

	calculateChecksum16(bms_request_type.buffer, sizeof(bms_request_type.buffer));						//crc calculate
	uart1.writeToBuffer(bms_request_type.buffer, sizeof(bms_request_type.buffer));						//request data buffer write
}



/**
  * @brief 	Response Read function, runs with response
  * @param[in]  uint8_t command_code 	:
  * @param[in]  uint8_t status_bit 	:
  * @return 	void
  */
void BMS_SLAVE_UBT::responseRead(uint8_t command_code)
{
	static parse_state_type state = parse_state_type::START_BIT;
	bms_ubetter_response_type bms_response_type;

	uint8_t read_buffer[1024]	=	{0};
	uint16_t read_buffer_size	=	0;
	static uint16_t read_index	=	0;
	uint16_t calculated_checksum	=	0;
	bool result			=	false;

	read_buffer_size = uart1.readFromBuffer(read_buffer, sizeof(read_buffer));

	if(read_buffer_size > 0)
	{
		for(read_index = 0; read_index < read_buffer_size; read_index++)
		{
			switch(state)
			{
				case parse_state_type::START_BIT:
					if(read_buffer[read_index] == START_BIT)
					{
						bms_response_type.data.start_bit = read_buffer[read_index];
						state = parse_state_type::COMMAND_CODE;
					}
					break;

				case parse_state_type::COMMAND_CODE:
					if(read_buffer[read_index] == command_code)
					{
						bms_response_type.data.command_code = read_buffer[read_index];
						state = parse_state_type::STATUS_BIT;
					}
					else
					{
						state = parse_state_type::START_BIT;
					}
					break;

				case parse_state_type::STATUS_BIT:
					if(read_buffer[read_index] == STATUS_CORRECT)
					{
						bms_response_type.data.status_bit = read_buffer[read_index];
						state = parse_state_type::LENGTH;
					}
					else if(read_buffer[read_index] == STATUS_ERROR)
					{
						bms_response_type.data.status_bit = read_buffer[read_index];
						state = parse_state_type::START_BIT;
					}
					else
					{
						state = parse_state_type::START_BIT;
					}
					break;

				case parse_state_type::LENGTH:
					bms_response_type.data.data_length = read_buffer[read_index];
					state = parse_state_type::PAYLOAD;
					break;

				case parse_state_type::PAYLOAD:
					bms_response_type.data.payload[read_index - 4] = read_buffer[read_index];							//Getting Message Values Into Array
					if((read_index - 2) > bms_response_type.data.data_length)
					{
						state = parse_state_type::CHECKSUM;
					}
					break;

				case parse_state_type::CHECKSUM:
					if((read_index - 4) > bms_response_type.data.data_length)
					{
						bms_response_type.data.checksum = static_cast<uint16_t>( read_buffer[read_index] ) | (static_cast<uint16_t>( read_buffer[read_index - 1] ) << 8);

						for (uint8_t i = 0; i < bms_response_type.data.data_length; i++)
						{
							calculated_checksum += bms_response_type.data.payload[i];
						}

						calculated_checksum += bms_response_type.data.status_bit + bms_response_type.data.data_length;
						calculated_checksum = (( ~calculated_checksum ) + 1);

						if(calculated_checksum == bms_response_type.data.checksum)
						{
							result = true;
							state = parse_state_type::STOP_BIT;
						}
						else
						{
							state = parse_state_type::START_BIT;
						}
					}
					break;

				case parse_state_type::STOP_BIT:
					if(read_buffer[read_index] == STOP_BIT)
					{
						bms_response_type.data.stop_bit = read_buffer[read_index];
						if(result == true)
						{
							processData(bms_response_type);
						}
					}
					state = parse_state_type::START_BIT;
					break;

				default:
					break;
			}
		}
	}
}



/**
  * @brief 	Calculate Checksum16 Uart
  * @param[in]  data_buffer, size
  * @return 	void
  */
void BMS_SLAVE_UBT::calculateChecksum16(uint8_t  data_buffer[], uint8_t size)							//checksum message send
{
	uint16_t calculate_checksum = 0;
	uint16_t checksum_index = 2;

	for(checksum_index = 2; checksum_index < 4; checksum_index++)
	{
		calculate_checksum += data_buffer[checksum_index];								//data sum process
	}

	calculate_checksum = (( ~calculate_checksum ) + 1);									// ( (0xFFFF - calculate_checksum) + 1)
	data_buffer[size - 3] = (uint16_t) ((calculate_checksum & 0xFF00) >> 8);						//making chekcsum 2byte
	data_buffer[size - 2] = (uint16_t) (calculate_checksum & 0xFF);								//making chekcsum 2byte
}



/**
  * @brief 	Process Data
  * @param[in]  bms_ubetter_response_type bms_response_type
  * @return 	void
  */
void BMS_SLAVE_UBT::processData(bms_ubetter_response_type bms_response_type)
{
	static uint8_t cell_index = 0;

	switch(bms_response_type.data.command_code)
	{

		case COMMAND_CODE_INFO:

			memcpy(&info_type.buffer[0], &bms_response_type.data.payload, bms_response_type.data.data_length);

			raw_type.data.total_voltage		= static_cast<uint16_t>(info_type.data.total_voltage_lo) 		| (static_cast<uint16_t>(info_type.data.total_voltage_hi) 		<< 8);
			raw_type.data.current			= static_cast<uint16_t>(info_type.data.current_lo) 			| (static_cast<uint16_t>(info_type.data.current_hi) 			<< 8);
			raw_type.data.residual_capacity		= static_cast<uint16_t>(info_type.data.residual_capacity_lo)		| (static_cast<uint16_t>(info_type.data.residual_capacity_hi) 	<< 8);
			raw_type.data.nominal_capacity		= static_cast<uint16_t>(info_type.data.nominal_capacity_lo) 		| (static_cast<uint16_t>(info_type.data.nominal_capacity_hi) 	<< 8);
			raw_type.data.number_of_cycles		= static_cast<uint16_t>(info_type.data.number_of_cycles_lo) 		| (static_cast<uint16_t>(info_type.data.number_of_cycles_hi) 	<< 8);
			raw_type.data.production_date		= static_cast<uint16_t>(info_type.data.production_date_lo)		| (static_cast<uint16_t>(info_type.data.production_date_hi) 	<< 8);
			raw_type.data.balance_status_low	= static_cast<uint16_t>(info_type.data.balance_status_low_lo) 		| (static_cast<uint16_t>(info_type.data.balance_status_low_hi) 	<< 8);
			raw_type.data.balance_status_high	= static_cast<uint16_t>(info_type.data.balance_status_high_lo) 		| (static_cast<uint16_t>(info_type.data.balance_status_high_hi) << 8);
			raw_type.data.protection_status		= static_cast<uint16_t>(info_type.data.protection_status_lo) 		| (static_cast<uint16_t>(info_type.data.protection_status_hi) 	<< 8);
			raw_type.data.software_version		= info_type.data.software_version;
			raw_type.data.remaining_capacity	= info_type.data.remaining_capacity;
			raw_type.data.fet_control_status	= info_type.data.fet_control_status;
			raw_type.data.number_of_battery		= info_type.data.number_of_battery;
			raw_type.data.number_of_ntc		= info_type.data.number_of_ntc;
			raw_type.data.cell_temp_1st		= static_cast<uint16_t>(info_type.data.cell_temp_1st_lo) 		| (static_cast<uint16_t>(info_type.data.cell_temp_1st_hi) 		<< 8);
			raw_type.data.cell_temp_2nd		= static_cast<uint16_t>(info_type.data.cell_temp_2nd_lo) 		| (static_cast<uint16_t>(info_type.data.cell_temp_2nd_hi) 		<< 8);
			raw_type.data.cell_temp_3rd		= static_cast<uint16_t>(info_type.data.cell_temp_3rd_lo) 		| (static_cast<uint16_t>(info_type.data.cell_temp_3rd_hi) 		<< 8);
			raw_type.data.cell_temp_4th		= static_cast<uint16_t>(info_type.data.cell_temp_4th_lo) 		| (static_cast<uint16_t>(info_type.data.cell_temp_4th_hi) 		<< 8);



			bms_data.data.total_voltage_v 			= static_cast<float>(raw_type.data.total_voltage) * 0.01;
			bms_data.data.current_a 			= static_cast<float>(~(0xFFFF - raw_type.data.current)) * 0.01;
			bms_data.data.residual_capacity_mah 		= raw_type.data.residual_capacity * 10;
			bms_data.data.nominal_capacity_mah		= raw_type.data.nominal_capacity * 10;
			bms_data.data.number_of_cycles			= raw_type.data.number_of_cycles;
			bms_data.data.production_date.data.years	= ((raw_type.data.production_date >> 9) + 2000);
			bms_data.data.production_date.data.months	= ((raw_type.data.production_date >> 5) & 0x0F);
			bms_data.data.production_date.data.days		= (raw_type.data.production_date & 0x1F);

			bms_data.data.balance_status_low		= raw_type.data.balance_status_low;
			bms_data.data.balance_status_high		= raw_type.data.balance_status_high;
			bms_data.data.protection_status.u16		= raw_type.data.protection_status;
			bms_data.data.software_version.data.major	= (raw_type.data.software_version / 10);
			bms_data.data.software_version.data.minor	= (raw_type.data.software_version % 10);
			bms_data.data.remaining_capacity_per		= raw_type.data.remaining_capacity;
			bms_data.data.fet_control_status.u8		= raw_type.data.fet_control_status;

			bms_data.data.number_of_battery_strings		= raw_type.data.number_of_battery;
			bms_data.data.number_of_ntc			= raw_type.data.number_of_ntc;
			bms_data.data.cell_temp_1st			= ((static_cast<float>(raw_type.data.cell_temp_1st) - 2731) / 10);
			bms_data.data.cell_temp_2nd			= ((static_cast<float>(raw_type.data.cell_temp_2nd) - 2731) / 10);
			bms_data.data.cell_temp_3rd			= ((static_cast<float>(raw_type.data.cell_temp_3rd) - 2731) / 10);
			bms_data.data.cell_temp_4th			= ((static_cast<float>(raw_type.data.cell_temp_4th) - 2731) / 10);

			break;

		case COMMAND_CODE_CELL:
			memcpy(&cell_type.data.cell_voltage_mv[0], &bms_response_type.data.payload, bms_response_type.data.data_length);

			for(cell_index = 0; (cell_index < bms_response_type.data.data_length / 2); cell_index++)
			{
				cell_type.data.cell_voltage_mv[cell_index] = ((cell_type.data.cell_voltage_mv[cell_index] & 0xFF) << 8) | (cell_type.data.cell_voltage_mv[cell_index] >> 8);
			}

			memcpy(&bms_data.data.cell_voltage_mv[0], &cell_type.data.cell_voltage_mv[0], bms_response_type.data.data_length);
			break;

		case COMMAND_CODE_VERS:
			memcpy(&version_type.data.version_number[0], &bms_response_type.data.payload, bms_response_type.data.data_length);

			memcpy(&bms_data.data.version_number[0], &version_type.data.version_number[0], bms_response_type.data.data_length);
			break;

		default:
			break;
	}
}



/**
  * @brief 	Bms Getter Function
  * @param[in]  void
  * @return 	void
  */
bms_data_type BMS_SLAVE_UBT::getData(void)													
{
	return bms_data;
}



/**
  * @brief 	Scheduler function
  * @param[in]  void
  * @return 	void
  */
void BMS_SLAVE_UBT::scheduler(void)
{
	static bms_state_type state = bms_state_type::INFO_REQUEST;

	switch(state)
	{
		case bms_state_type::INFO_REQUEST:
			requestSend(STATUS_BIT_READ, COMMAND_CODE_INFO);
			state = bms_state_type::INFO_RESPONSE;;
			break;

		case bms_state_type::INFO_RESPONSE:
			responseRead(COMMAND_CODE_INFO);
			state = bms_state_type::VERS_REQUEST;
			break;

		case bms_state_type::VERS_REQUEST:
			requestSend(STATUS_BIT_READ, COMMAND_CODE_VERS);
			state = bms_state_type::VERS_RESPONSE;
			break;

		case bms_state_type::VERS_RESPONSE:
			responseRead(COMMAND_CODE_VERS);
			state = bms_state_type::CELL_REQUEST;
			break;

		case bms_state_type::CELL_REQUEST:
			requestSend(STATUS_BIT_READ, COMMAND_CODE_CELL);
			state = bms_state_type::CELL_RESPONSE;
			break;

		case bms_state_type::CELL_RESPONSE:
			responseRead(COMMAND_CODE_CELL);
			state = bms_state_type::INFO_REQUEST;
			break;

		default:
			break;
	}
}



/**
  * @brief 	Default copy constructor
  * @param[in]  void
  * @return 	void
  */
BMS_SLAVE_UBT::BMS_SLAVE_UBT(const BMS_SLAVE_UBT& orig)
{ }



/**
  * @brief 	Default destructor
  * @param[in]  void
  * @return 	void
  */
BMS_SLAVE_UBT::~BMS_SLAVE_UBT()
{ }


} /* namespace Ubtbat */

} /* namespace Battery */


/********************************* END OF FILE *********************************/

