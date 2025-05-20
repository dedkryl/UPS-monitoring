#include <stdexcept>
#include <iostream>

#include "ModbusObject.h"
#include "crc.h"


ModbusObject::ModbusObject()
{

}
ModbusObject::~ModbusObject()
{
}

bool ModbusObject::data_handling()
{
	try
	{
			if (slave_id != answer.at(0))
			{
				printf("Error SlaveID : request - %d, answer - %d\n", slave_id, answer.at(0));
				//return false;
				goto bad_reading_exit;
			}
			if (func_num != answer.at(1))
			{
				printf("Error FunNum : request - %d, answer - %d\n", func_num, answer.at(1));
				//return false;
				goto bad_reading_exit;
			}
			if ((reg_count * 2) != answer.at(2))
			{
				printf("Error Byte count : request - %d, answer - %d\n", reg_count * 2, answer.at(2));
				//return false;
				goto bad_reading_exit;
			}
			//CRC
			BYTE* pTemp = (BYTE*)malloc(answer.size() - 2);
			for (UINT k = 0; k < (answer.size() - 2); k++)
			{
				*(pTemp + k) = answer.at(k);
			}

			WORD CrcCalc = calc_crc16(pTemp, answer.size() - 2, 0xFFFF);
			delete pTemp;
			
			BYTE CrcCalcB0 = CrcCalc & 0xFF;
			BYTE CrcCalcB1 = CrcCalc >> 8;
			if ((CrcCalcB0 != answer.at(answer.size() - 2)) || (CrcCalcB1 != answer.at(answer.size() - 1)))
			{
				printf("CRC Error !!!\n");
				//return false;
				goto bad_reading_exit;
			}
			/////////////////////
			//ALL right!
			BYTE* payload_buf = nullptr; 
			UINT payload_len = 0;
			payload_buf = (BYTE*)malloc(reg_count * 2);
			if (!payload_buf)
			{
				printf("Memory alloc error!!\n");
				//return false;
				goto bad_reading_exit;
			}

			for (int i = 0; i < (reg_count * 2); i++)
			{
				*(payload_buf + i) = answer.at(3 + i);
			}
			payload_len = reg_count * 2;
			answer.clear();
			
			if(!sh)//once
				sh = create_smart_handler(payload_buf, payload_len);
			else
				sh->update(payload_buf, payload_len);
				
			free(payload_buf);

			if(sh)
				sh->handle();
			else
				std::cout << "Warning : No smart handler for " << command_name << std::endl;
			

	}
	catch (std::out_of_range e)
	{
		printf("Out of range exception in data_handling function\n");
		//return false;
		goto bad_reading_exit;
	}

	return true;

bad_reading_exit:
	printf("bad_reading_exit ");
	answer.clear();
    
	return false;
}

std::shared_ptr<SmartHandlerBase> ModbusObject::create_smart_handler(const BYTE* buf, const int buf_size)
{
		switch (handler_type)
		{
			case e_handler_type::SmartHandlerFaultStatus:
				return std::make_shared<SmartHandlerFaultStatus>(buf, buf_size, this);
			case e_handler_type::SmartHandlerSystemStatus:
				return std::make_shared<SmartHandlerSystemStatus>(buf, buf_size, this);
			case e_handler_type::SmartHandlerChargingStatus:
				return std::make_shared<SmartHandlerChargingStatus>(buf, buf_size, this);				
			case e_handler_type::SmartHandlerAnalog:
				return std::make_shared<SmartHandlerAnalog>(buf, buf_size, coeff, this);									
			default:
				return nullptr;

		}

}