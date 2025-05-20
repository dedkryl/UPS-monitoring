#pragma once
#include <mutex>
#include <string>
#include <map>

using SUBPARAMS = std::map<std::string, std::string>;
using PARAM = std::pair<std::string, SUBPARAMS>;
using PARAMS = std::map<std::string, SUBPARAMS>;


class HandledData
{
    PARAMS out_params;
    std::mutex mux;
    time_t last_update;
    void PutTimeStamp();
public:    
    void Update(const PARAM& param);
    std::string GetOutputJson();
    bool is_device_connected();
    PARAMS GetOutParams(); 
};
