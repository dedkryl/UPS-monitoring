#include <iostream>
#include "HandledData.h"
#include "njson.hpp"
#include "ModBus/ModbusManager.h"

extern ModbusManager mm;

void HandledData::Update(const PARAM& param)
{
    try
    {
        std::lock_guard<std::mutex> lock(mux);
        out_params[param.first] = param.second;
        PutTimeStamp();
    }
    catch(const std::exception& e)
    {
        std::cout << " HandledData::Update exception : "<< e.what() << '\n';
    }
}

bool HandledData::is_device_connected()
{
    time_t now = std::time(nullptr);
    //std::cout << " is_device_connected : " << now - last_update << std::endl;
    if((now - last_update) > (3 * mm.get_request_period()))
        return false;
    else
        return true;    
}

void HandledData::PutTimeStamp()
{
    //do not locked (again)
    last_update = std::time(nullptr);
    auto tm = *std::localtime(&last_update);
    std::stringstream ss;
    ss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
    SUBPARAMS time_stamp;
    time_stamp.insert({"time", ss.str()});
    out_params["Updated"] = time_stamp;
}

PARAMS HandledData::GetOutParams()
{
    std::lock_guard<std::mutex> lock(mux);
    PARAMS pars = out_params;
    return pars;
}


std::string HandledData::GetOutputJson()
{
    try
    {
        std::lock_guard<std::mutex> lock(mux);
        //std::cout << " GetOutputJson --> " << std::endl;
        nlohmann::json js_out;
        nlohmann::json js_params;
        for (auto &&param : out_params)
        {
            //std::cout << param.first << std::endl;
            nlohmann::json js_subparams;
            for (auto &&subparam : param.second)
            {
                nlohmann::json js_subparam;
                //std::cout << subparam.first << std::endl;
                //std::cout << subparam.second << std::endl;
                js_subparam[subparam.first] = subparam.second;
                js_subparams += js_subparam;  
            }
            js_params[param.first] = js_subparams;
        }

        js_out["UPS params"] = js_params;

        return js_out.dump();
    }
    catch(const std::exception& e)
    {
        std::cout << " HandledData::GetOutputJson : " << e.what() << '\n';
        nlohmann::json js_err;
        js_err["UPS params"] = "Internal error";
        return js_err.dump();
    }    
} 