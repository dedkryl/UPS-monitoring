#include "ModbusManager.h"
#include <iostream>
#include <fstream>
#include "../njson.hpp"
#include "crc.h"


extern HandledData out_data_holder;

NLOHMANN_JSON_SERIALIZE_ENUM( e_action, {
    {e_action::invalid, nullptr},
	{e_action::idle, "idle"},
    {e_action::reading, "reading"},
    {e_action::writing, "writing"},
})


NLOHMANN_JSON_SERIALIZE_ENUM( e_handler_type, {
    {e_handler_type::invalid, nullptr},
    {e_handler_type::SmartHandlerFaultStatus, "SmartHandlerFaultStatus"},
	{e_handler_type::SmartHandlerSystemStatus, "SmartHandlerSystemStatus"},
	{e_handler_type::SmartHandlerChargingStatus, "SmartHandlerChargingStatus"},	
	{e_handler_type::SmartHandlerAnalog, "SmartHandlerAnalog"},
    {e_handler_type::empty, "empty"},
})


void from_json(const nlohmann::json& j, device_connection_params& pars)
{
    assert(j.is_object());
    j.at("tds_ip_address").get_to(pars.tds_ip_address);
    j.at("slave_id").get_to(pars.slave_id);
	j.at("request_period").get_to(pars.request_period);
}


void from_json(const nlohmann::json& j, database_params& pars)
{
    assert(j.is_object());
    j.at("db_name").get_to(pars.db_name);
    j.at("table_name").get_to(pars.table_name);
	j.at("mode").get_to(pars.mode);
	j.at("days_of_storage").get_to(pars.days_of_storage);
}

void from_json(const nlohmann::json& j, rest_server_params& pars)
{
    assert(j.is_object());
    j.at("port").get_to(pars.port);
    j.at("threads").get_to(pars.threads);
    j.at("connection_limit").get_to(pars.connection_limit);	
}


void from_json(const nlohmann::json& j, ModbusObject& mo)
{
    assert(j.is_object());
    j.at("command_name").get_to(mo.command_name);
    j.at("description").get_to(mo.description);

    j.at("action").get_to(mo.action);
    j.at("func_num").get_to(mo.func_num);

    std::string reg_addr;
    j.at("reg_addr").get_to(reg_addr);
    mo.reg_addr = std::stoul(reg_addr,nullptr,16);
    
    j.at("reg_count").get_to(mo.reg_count);
    j.at("coeff").get_to(mo.coeff);
    j.at("handler").get_to(mo.handler_type);
}

int ModbusManager::get_request_period() const
{
	return device_params.request_period;
}

bool ModbusManager::Init(std::string file_name)
{
    try
	{
        std::ifstream file_input;
        file_input.open(file_name);
        if(file_input.fail())
		{
             //throw std::runtime_error(std::string("Error: Unable to open file ") +  file_name);
			std::cout << std::string("Error: Unable to open file ") +  file_name << std::endl;
			return false;
		}

        std::string str((std::istreambuf_iterator<char>(file_input)),
                    std::istreambuf_iterator<char>());

        nlohmann::json obj = nlohmann::json::parse(str);
        if(obj.contains("modbus_objects"))
        {
            obj.at("modbus_objects").get_to(modbus_objects);
        }
        
        if(obj.contains("device_connection_params"))
        {
            obj.at("device_connection_params").get_to(device_params);
        }

		if(obj.contains("database_params"))
        {
            obj.at("database_params").get_to(db_params);
			dbm = std::make_shared<DBManager>(db_params);
        }

		if(obj.contains("rest_server_params"))
        {
            obj.at("rest_server_params").get_to(rest_params);
        }
		
		
 	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << '\n';
		return false;
	}
    return true;             
}

rest_server_params ModbusManager::get_rest_params() const
{
	return rest_params;
}


bool ModbusManager::ReadDataFromDevice()
{
    bool ret = false;
	for (int i = 0; i < REQUEST_ATTEMPTS; i++)
	{
		ret = comport.open_com_port(device_params.tds_ip_address,"COM2", true);
		if (ret)
			break;
	}

	if (!ret)
	{
		printf("Connection error to COM port\n");
        comport.close_com_port();
		return false;
	}

	int count = 0;
	
    for (auto& mo : modbus_objects)
	{
		if(mo.action != e_action::idle)
		{
			//std::cout << mo.command_name << std::endl;
			if (RequestAndAnswer(mo))
			{
				if (!mo.data_handling())
				{
					std::cout << "data_handling problem with device \n";
					continue;
				}
			}
			else
			{
				std::cout << "request error for device\n";
				continue;
			}
			count++;
		}
	}
	std::cout << "readed and handled " << count << " modbus object(s)" << std::endl;
    comport.close_com_port();
/////////////////////////////////////////////////
	if(!db_ready)	
		db_ready = dbm->CreateTable(out_data_holder.GetOutParams());
	if(db_ready)
	{
		static int cleaner_count = 0;
		dbm->InsertData(out_data_holder.GetOutParams());
		if(cleaner_count == 50)
		{
			dbm->ClearOldRecords();
			cleaner_count = 0;
		}
		cleaner_count++;
	}
/////////////////////////////////////////////////	
    return true;
}


bool ModbusManager::RequestAndAnswer(ModbusObject& mo)
{
	bool Ret = false;
	for (int i = 0; i < REQUEST_ATTEMPTS; i++)
	{
		Ret = OneRequestAndAnswer(mo);
		if (Ret)
			break;
	}

	if (!Ret)
	{
		printf("Unsuccessfull call of RequestAndAnswer \n");
		return false;
	}

    return true;    
}

bool ModbusManager::OneRequestAndAnswer(ModbusObject& mo)
{
	int err = 0;
	mo.trans_id = trans_counter;//(WORD)rand();
	trans_counter++;
	if (trans_counter == std::numeric_limits<uint32_t>::max())
		trans_counter = 0;

#pragma pack(1)
	struct {
		WORD startAddress;
		WORD regCount;
	} sendData = { UINT16_TO_BE(mo.reg_addr), UINT16_TO_BE(mo.reg_count) };
#pragma pack()

	struct iovec sendVector[1] = { { &sendData, sizeof(sendData) } };
	int send_len;
    mo.slave_id = device_params.slave_id + 0x80;
	if (!CreateModBusADU(SendBuffer, SEND_DATA_BUFSIZE, &send_len, mo.func_num, sendVector, 1, mo.slave_id, mo.trans_id))
	{
		printf("Error! Unable to create ADU!\n");
		return false;
	}

	int NormalAnswFullLen = CalcNormalAnswFullLen(mo.reg_count);
    std::vector<uint8_t> request{SendBuffer, SendBuffer + send_len};
    mo.answer.clear();
    if(!comport.modbus_exchange(request, mo.answer, mo.func_num, NormalAnswFullLen, ExceptionalAnswerSerialFullLen))
        return false;

	return  true;
}

bool ModbusManager::CreateModBusADU(void *dst_, int dstSize, int *pdstLen, BYTE funCode, const struct iovec *srcVector, int srcCount, BYTE devAddress, WORD TransID) {

	int i, srcLenAll;
	BYTE *dst = (BYTE*)dst_;

	if (!srcVector || srcCount < 0)
		srcCount = 0;

	srcLenAll = 0;
	for (i = 0; i < srcCount; ++i) if (srcVector[i].iov_base && srcVector[i].iov_len > 0)
		srcLenAll += srcVector[i].iov_len;

    //serial
	*pdstLen = srcLenAll + 4;

	if (*pdstLen <= dstSize)
	{
		int na;
		na = 0;
		dst[na++] = devAddress;

		// funCode
		dst[na++] = funCode;

		// data
		for (i = 0; i < srcCount; ++i) if (srcVector[i].iov_base && srcVector[i].iov_len > 0) {
			memcpy(dst + na, srcVector[i].iov_base, srcVector[i].iov_len);
			na += srcVector[i].iov_len;
		}

		// suffix

		WORD crc = calc_crc16(dst, na, 0xFFFF);

		*((WORD*)(dst + na)) = crc;
		na += 2;
		return true;
	}
	else
	{
		printf("Send data too big...\n");
		return false;
	}

}

int ModbusManager::CalcNormalAnswFullLen(int regCount) {
	int len = 0;
	// prefix
	len += sizeof(BYTE); // address
	// header
	len += sizeof(BYTE); // funCode
	len += sizeof(BYTE); // ByteCount
	len += regCount*2; // funCode
	// suffix
	len += sizeof(WORD); // crc

	return len;
}

