

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <netinet/tcp.h>

class TdsComPort
{
	std::string com_port_name;
	int control_socket;//port 10000
	int data_socket;//port 10002
	const int rcv_timeout = 3;
   
	bool connect_tcp(const std::string& ip, const int port, const int timeout_sec, int& sock);
	bool setting_appl(const std::vector<char>& command, const std::vector<char>& answer_pattern, bool fuzzy = false);
	//bool setting_appl_2(const std::vector<char>& command, const std::string& answer_pattern);

public:
	TdsComPort();
	TdsComPort(const std::string& ip_address, const std::string& com_port, bool binary_mode);
	~TdsComPort();
	//DEBUG : this echo test is available for COM3 only with Tx/Rx loop !
	bool send_to_loop(const std::string& message);
    //DEBUG : tds + UPS
	bool simple_modbus_exchange();

	bool open_com_port(const std::string& ip_address, const std::string& com_port, bool binary_mode);
	void close_com_port();

	bool modbus_exchange(const std::vector<uint8_t>& request, std::vector<uint8_t>& answer, const int fun_num, const int expected_answer_len, const int exception_len);
};
