/**
  ******************************************************************************
  * @file	: bms_slave_ubt.hpp
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

#ifndef BMS_SLAVE_UBT_HPP
#define BMS_SLAVE_UBT_HPP


#include <stdint.h>
#include "hal_uart.hpp"


namespace Battery
{

namespace Ubtbat
{



/**
  * @brief Bms Ubetter Request Type
  */
#pragma pack(1)
union bms_ubetter_request_type
{
	struct
	{
		uint8_t  start_bit;
		uint8_t  status_bit;
		uint8_t  command_code;
		uint8_t  data_length;
		uint16_t checksum;
		uint8_t  stop_bit;
	}data;

	uint8_t buffer[7];
	bms_ubetter_request_type():
		buffer{}
	{ }
};
#pragma pack()



/**
  * @brief Bms Ubetter Response Type
  */
#pragma pack(1)
union bms_ubetter_response_type
{
	struct
	{
		uint8_t  start_bit;
		uint8_t  command_code;
		uint8_t  status_bit;
		uint8_t  data_length;
		uint8_t  payload[120];
		uint16_t checksum;
		uint8_t  stop_bit;
	}data;

	uint8_t buffer[128];
	bms_ubetter_response_type():
		buffer{}
	{ }
};
#pragma pack()



/**
  * @brief Bms Protection Type
  */
#pragma pack(1)
union bms_protection_status_type
{
	struct
	{
		uint16_t cell_overvoltage_protec	:1;		//bit0
		uint16_t cell_undervoltage_protec	:1;
		uint16_t pack_overvoltage_protec	:1;
		uint16_t pack_undervoltage_protec	:1;
		uint16_t charging_over_temp			:1;
		uint16_t charging_low_temp			:1;
		uint16_t discharge_over_temp		:1;
		uint16_t discharge_low_temp			:1;
		uint16_t charging_over_current		:1;
		uint16_t discharge_over_current		:1;
		uint16_t short_circuit				:1;
		uint16_t frontend_detect_ic_error	:1;
		uint16_t software_lock_mos			:1;
		uint16_t reverse1					:1;
		uint16_t reverse2					:1;
		uint16_t reverse3					:1;
	}bits;

	uint16_t u16;
	bms_protection_status_type():
		u16(0)
	{ }
};
#pragma pack()



/**
  * @brief Bms Mosfet Control Status Type
  */
#pragma pack(1)
union fet_control_status_type
{
	struct
	{
		uint8_t fet_charge_status			:1;		//bit0
		uint8_t fet_discharge_status		:1;		//bit1
		uint8_t reverse						:6;		//bit2-7


	}bits;

	uint8_t u8;
	fet_control_status_type():
		u8(0)
	{ }
};
#pragma pack()



/**
  * @brief	Date of Manufacture Struct
  */
#pragma pack(1)
union production_date_type
{
	struct
	{
		uint16_t days;
		uint16_t months;
		uint16_t years;
	}data;

	uint8_t buffer[6];

	production_date_type():
	buffer{}
	{ }
};
#pragma pack()



/**
  * @brief	Software Version Struct
  */
#pragma pack(1)
union software_version_type
{
	struct
	{
		uint8_t major;
		uint8_t minor;
		uint8_t patch;
	}data;

	uint8_t buffer[3];

	software_version_type():
	buffer{}
	{ }
};
#pragma pack()



/**
  * @brief Command Cell Data Type
  */
#pragma pack(1)
union commnd_cell_data_type
{
	struct
	{
		uint16_t  cell_voltage_mv[17];
	}data;

	uint8_t buffer[34];
	commnd_cell_data_type():
		buffer{}
	{ }
};
#pragma pack()



/**
  * @brief Command Version Data Type
  */
#pragma pack(1)
union commnd_version_data_type
{
	struct
	{
		uint8_t version_number[10];
	}data;

	uint8_t buffer[10];
	commnd_version_data_type():
		buffer{}
	{ }
};
#pragma pack()



/**
  * @brief Command Info Data Type
  */
#pragma pack(1)
union commnd_info_data_type
{
	struct
	{
		uint8_t total_voltage_hi;
		uint8_t total_voltage_lo;
		uint8_t current_hi;
		uint8_t current_lo;
		uint8_t residual_capacity_hi;
		uint8_t residual_capacity_lo;
		uint8_t nominal_capacity_hi;
		uint8_t nominal_capacity_lo;
		uint8_t number_of_cycles_hi;
		uint8_t number_of_cycles_lo;
		uint8_t production_date_hi;
		uint8_t production_date_lo;
		uint8_t balance_status_low_hi;
		uint8_t balance_status_low_lo;
		uint8_t balance_status_high_hi;
		uint8_t balance_status_high_lo;
		uint8_t protection_status_hi;
		uint8_t protection_status_lo;
		uint8_t software_version;
		uint8_t remaining_capacity;
		uint8_t fet_control_status;
		uint8_t number_of_battery;
		uint8_t number_of_ntc;
		uint8_t cell_temp_1st_hi;
		uint8_t cell_temp_1st_lo;
		uint8_t cell_temp_2nd_hi;
		uint8_t cell_temp_2nd_lo;
		uint8_t cell_temp_3rd_hi;
		uint8_t cell_temp_3rd_lo;
		uint8_t cell_temp_4th_hi;
		uint8_t cell_temp_4th_lo;
	}data;

	uint8_t buffer[31];
	commnd_info_data_type():
		buffer{}
	{ }
};
#pragma pack()



/**
  * @brief Bms Ubetter Raw Data
  */
#pragma pack(1)
union raw_data_info_type
{
	struct
	{
		uint16_t total_voltage;
		uint16_t current;
		uint16_t residual_capacity;
		uint16_t nominal_capacity;
		uint16_t number_of_cycles;
		uint16_t production_date;
		uint16_t balance_status_low;
		uint16_t balance_status_high;
		uint16_t protection_status;
		uint8_t software_version;
		uint8_t remaining_capacity;
		uint8_t fet_control_status;
		uint8_t number_of_battery;
		uint8_t number_of_ntc;
		uint16_t cell_temp_1st;
		uint16_t cell_temp_2nd;
		uint16_t cell_temp_3rd;
		uint16_t cell_temp_4th;
	}data;

	uint8_t buffer[31];
	raw_data_info_type():
		buffer{}
	{ }
};
#pragma pack()



/**
  * @brief Bms Data Type
  */
#pragma pack(1)
union bms_data_type
{
	struct
	{
		float total_voltage_v;
		float current_a;
		uint16_t residual_capacity_mah;
		uint16_t nominal_capacity_mah;
		uint16_t number_of_cycles;
		production_date_type production_date;
		uint16_t balance_status_low;
		uint16_t balance_status_high;
		bms_protection_status_type protection_status;
		software_version_type software_version;
		uint16_t remaining_capacity_per;
		fet_control_status_type fet_control_status;
		uint16_t number_of_battery_strings;
		uint16_t number_of_ntc;
		float cell_temp_1st;
		float cell_temp_2nd;
		float cell_temp_3rd;
		float cell_temp_4th;
		uint16_t cell_voltage_mv[17];
		uint8_t version_number[10];

	}data;

	uint8_t buffer[256];
	bms_data_type():
		buffer{}
	{ }
};
#pragma pack()



/**
  * @brief Parse State Enum
  */
enum class parse_state_type: uint8_t
{
	START_BIT	 	= 0,
	COMMAND_CODE 	= 1,
	STATUS_BIT		= 2,
	LENGTH			= 3,
	PAYLOAD	 	 	= 4,
	CHECKSUM		= 5,
	STOP_BIT		= 6,
};



/**
  * @brief Parse State Enum
  */
enum class bms_state_type: uint8_t
{
	INFO_REQUEST 	= 0,
	INFO_RESPONSE 	= 1,
	VERS_REQUEST	= 2,
	VERS_RESPONSE	= 3,
	CELL_REQUEST 	= 4,
	CELL_RESPONSE	= 5,
};



/**
  * @brief	Example Class Brief Info
  */
class BMS_SLAVE_UBT
{
	public:
		BMS_SLAVE_UBT();

        void initialize(void);
        void scheduler(void);

        BMS_SLAVE_UBT(const BMS_SLAVE_UBT& orig);
		virtual ~BMS_SLAVE_UBT();
		bms_data_type getData(void);
	protected:

	private:
		void requestSend(uint8_t status_bit, uint8_t command_code);
		void responseRead(uint8_t command_code);
		void calculateChecksum16(uint8_t  data_buffer[], uint8_t size);
		void processData(bms_ubetter_response_type bms_response_type);
		void bitShift(uint8_t buffer[], uint8_t length);

		bms_data_type bms_data;

		//DEBUG------------------------------------------------------//

		commnd_cell_data_type cell_type;
		commnd_version_data_type version_type;
		commnd_info_data_type info_type;
		raw_data_info_type raw_type;

		//-----------------------------------------------------------//

};


} /* namespace Ubtbat */

} /* namespace Battery */



/**
  * @brief External Linkages
  */
extern Battery::Ubtbat::BMS_SLAVE_UBT ubetter;



#endif /* BMS_SLAVE_UBT */

/********************************* END OF FILE *********************************/

