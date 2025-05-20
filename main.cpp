#include <algorithm>
#include <iostream>
#include "ModBus/TdsComPort.h"
#include "ModBus/ModbusManager.h"
#include "HandledData.h"
#include <thread>
#include "ups_service.h"

extern HandledData out_data_holder;

ModbusManager mm;

void modbus_thread_func()
{
    while(true)
    {
        mm.ReadDataFromDevice();
        //std::cout << out_data_holder.GetOutputJson();
        //std::cout << "Successful reading from device " << std::endl;
        sleep(mm.get_request_period());
    }
}

int main (int argc, char*argv[])
{
    try
    {
        if(!mm.Init("Config.json"))
            return 1;
        //./ups_service loop_test
        if((argc == 2)&&(std::string{"loop_test"} == std::string{argv[1]}))
        {
            TdsComPort com_port{ mm.get_tds_ip_address() , "COM3", true };
            com_port.send_to_loop("Hello, Eugene!");
            return 0;
        }
        else//work mode
        {
            std::thread modbus_thread(modbus_thread_func);
            modbus_thread.detach();
            int port = mm.get_rest_params().port;//which available port ?
            int threads = mm.get_rest_params().threads;
            int connection_limit = mm.get_rest_params().connection_limit;

            rest_server server;
            server.set_log_file(&cerr);//add log file by rd overload
            server.set_max_connections(connection_limit);
            server.set_threads(threads);

            ups_service service;
            if (!server.start(&service, port))
                return cerr << "Cannot start REST server!" << endl, 1;
            server.wait_until_signalled();
            server.stop();

            return 0;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

}

