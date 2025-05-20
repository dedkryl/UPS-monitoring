#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../DataBase/DBManager.h"
#include "SmartHandler.h"

typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef unsigned short WORD;

enum class e_action {
    reading,
    writing,
	idle,
    invalid=-1,
};

enum class e_handler_type {
    SmartHandlerFaultStatus,
    SmartHandlerSystemStatus,
	SmartHandlerChargingStatus,
    SmartHandlerAnalog,	
    empty,
    invalid=-1,
};


class ModbusObject
{
	std::shared_ptr<SmartHandlerBase> create_smart_handler(const BYTE* buf, const int buf_size);
public:
	ModbusObject();
	~ModbusObject();
	bool data_handling();
	std::shared_ptr<SmartHandlerBase> sh = nullptr;
	std::vector<uint8_t> answer;
	std::string command_name;
	std::string description;
	int func_num;
	int reg_addr;
	int reg_count;
	float coeff;
	e_action action;
	e_handler_type handler_type;
	int slave_id;
	WORD trans_id;
};
