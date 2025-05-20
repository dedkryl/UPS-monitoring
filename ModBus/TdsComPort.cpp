#include <cassert>
#include "TdsComPort.h"
#include "../utils.h"

TdsComPort::TdsComPort(){}

TdsComPort::TdsComPort(const std::string& ip_address, const std::string& com_port, bool binary_mode)
{
	if (!open_com_port(ip_address, com_port, binary_mode))
		throw std::runtime_error(std::string{ "Unable to open " } + com_port);
}

TdsComPort::~TdsComPort()
{
	close_com_port();
}

bool TdsComPort::connect_tcp(const std::string& ip, const int port, const int timeout_sec, int& sockfd)
{
	struct sockaddr_in servaddr, cli;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        return false;
    }
    //else
      //  printf("Socket successfully created..\n");
    
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    servaddr.sin_port = htons(port);

    //set off 'Nagle algo'
	int value = 1;
	int rc = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&value, sizeof(value));
	if (rc < 0) {
		printf("setsockopt for TCP_NODELAY failed \n");
		return false;
	}

	//set timeout
    struct timeval tv;
    tv.tv_sec = timeout_sec;  
    tv.tv_usec = 0;
	rc = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv,sizeof(struct timeval));
	if (rc < 0)
	{
		printf("setsockopt for ConnSocket(SO_RCVTIMEO) failed \n");
		return false;
    }

    // connect the client socket to server socket
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))!= 0) {
        printf("connection with the server failed...\n");
        return false;
    }
    //else
      //  printf("connected to the server..\n");
   
   return true;
}

bool TdsComPort::setting_appl(const std::vector<char>& command, const std::vector<char>& answer_pattern, bool fuzzy)
{
	int i = 0;
	int rc =  write(control_socket, (char*)command.data(), command.size());
	if (rc < 0)
	{

		return false;
	}
	
    char buff[256];
	uint32_t l = 0;
    std::vector<char> answer;
	while (l < answer_pattern.size())
	{

        bzero(buff, sizeof(buff));
		rc = read(control_socket, buff, sizeof(buff));
		if (rc < 0)
		{
			printf("This is receive err!");
			return false;
		}
		if (rc == 0)
		{
			printf("If 0 bytes are received, the connection was closed..\n");
			return false;
		}
		l += rc;
        answer.insert(answer.end(),buff, buff + rc);
	}//while 

    if(!fuzzy)
    {
        if (answer == answer_pattern)
        {
            //std::cout << "Pattern and answer are equals!" << std::endl;
            //print_vector(" answer_pattern : ", answer_pattern);
            //print_vector(" answer : ", answer);
            return  true;
        }
        else
        {
            std::cout << "Pattern and answer are not equals!" << std::endl;
            print_vector(" answer_pattern : ", answer_pattern);
            print_vector(" answer : ", answer);
            return false;
        }
    }
    else
    {
        //fuzzy
        if (std::string{answer.begin(), answer.end()}.find(std::string{answer_pattern.begin(), answer_pattern.end()}) != std::string::npos)
        {
            //std::cout << "Answer contains pattern inside" << std::endl;
		    return  true;
        }
        else
        {
            std::cout << "Answer doesnt contain pattern inside" << std::endl;
            print_vector(" answer_pattern : ", answer_pattern);
            print_vector(" answer : ", answer);
            return false;
        }
    }
}

bool TdsComPort::open_com_port(const std::string& ip_address, const std::string& com_port, bool binary_mode)
{

	com_port_name = com_port;

	if (!connect_tcp(ip_address, 10000, rcv_timeout, control_socket))
		return false;

    //std::cout << "control_socket OK" << std::endl;    

	int data_tcp_port;
	if (com_port == "COM3")
		data_tcp_port = 10002;
	else if (com_port == "COM2")
		data_tcp_port = 10001;
	else
		return false;

	if (!connect_tcp(ip_address, data_tcp_port, rcv_timeout, data_socket))
		return false;

    //std::cout << "data_socket OK" << std::endl;        

	if (com_port == "COM3")
	{
		std::vector<char> c1{ 0x32, 0x37, 0x32, 0x30, 0x0d };
		std::vector<char> a1{ 0x4f, 0x4b, 0x0d };//OK
		if (!setting_appl(c1, a1))
			return false;

		std::vector<char> c2{ 0x32, 0x39, 0x32, 0x0d };
		std::vector<char> a2{ 0x43, 0x54, 0x53, 0x32, 0x3d, 0x30, 0x0d };//CTS2=0
		if (!setting_appl(c2, a2))
			return false;

		std::vector<char> c3{ 0x30, 0x32, 0x32, 0x31, 0x31, 0x35, 0x32, 0x30, 0x30, 0x0d };
		std::vector<char> a3{ 0x4f, 0x4b, 0x0d };//OK
		if (!setting_appl(c3, a3))
			return false;

		std::vector<char> c4{ 0x30, 0x35, 0x32, 0x30, 0x30, 0x0d };
		std::vector<char> a4{ 0x4f, 0x4b, 0x0d };//OK
		if (!setting_appl(c4, a4))
			return false;

		std::vector<char> c5{ 0x30, 0x33, 0x32, 0x38, 0x4e, 0x31, 0x0d }; //0328N1.
		std::vector<char> a5{ 0x4f, 0x4b, 0x0d };//OK
		if (!setting_appl(c5, a5))
			return false;

		std::vector<char> c6{ 0x34, 0x30, 0x32, 0x30, 0x30, 0x0d };
		std::vector<char> a6{ 0x43, 0x6f, 0x6d, 0x6d, 0x61, 0x6e, 0x64, 0x20, 0x45, 0x72, 0x72, 0x6f, 0x72, 0x0d };
		if (!setting_appl(c6, a6))
			return false;

		std::vector<char> c7{ 0x32, 0x39, 0x32, 0x0d };
		std::vector<char> a7{ 0x43, 0x54, 0x53, 0x32, 0x3d, 0x30, 0x0d };//CTS2=0
		if (!setting_appl(c7, a7))
			return false;
	}
	else if (com_port == "COM2")
	{
        
		std::vector<char> c1{ 0x32, 0x37, 0x31, 0x30, 0x0d };
		std::vector<char> a1{ 0x4f, 0x4b, 0x0d };//OK, but can be OK.MSR1=0B. or even MSR1=0B.OK.  
		if (!setting_appl(c1, a1, true))//fuzzy
			return false;
		
		if (binary_mode)
		{
			//in modbus wizard
			std::vector<char> cm{ 0x33, 0x35, 0x31, 0x31, 0x31, 0x0d };//binary mode?
			std::vector<char> am{ 0x4f, 0x4b, 0x0d };//OK
			if (!setting_appl(cm, am))
				return false;
		}

		std::vector<char> c2{ 0x30, 0x32, 0x31, 0x31, 0x31, 0x35, 0x32, 0x30, 0x30, 0x0d };
		std::vector<char> a2{ 0x4f, 0x4b, 0x0d };//OK
		if (!setting_appl(c2, a2))
			return false;

		std::vector<char> c3{ 0x30, 0x33, 0x31, 0x38, 0x4e, 0x31, 0x0d };
		std::vector<char> a3{ 0x4f, 0x4b, 0x0d };//OK
		if (!setting_appl(c3, a3))
			return false;
       
	}
	else
		return false;

	return true;
}

void TdsComPort::close_com_port()
{
	close(control_socket);
	close(data_socket);
}

//this echo test is available for COM3 only!
bool TdsComPort::send_to_loop(const std::string& message)
{
	int rc = write(data_socket, (char*)message.data(), message.size());
	if (rc < 0)
	{
        printf("write error!");
		return false;
	}

    char buff[256];
	rc = read(data_socket, buff, sizeof(buff));
	if (rc < 0)
	{
		printf("read error!");
		return false;
	}
	if (rc == 0)
	{
		printf("If 0 bytes are received, the connection was closed..\n");
		return false;
	}

	std::string answer{ buff, buff + rc};
	if (message == answer)
    {
        std::cout << "Message and answer are equals!" << std::endl;
        std::cout << "message :" << message << std::endl;
        std::cout << "answer :" << answer << std::endl;
		return  true;
    }    
	else
	{
		std::cout << "Message and answer are not equals!" << std::endl;
        std::cout << "message :" << message << std::endl;
        std::cout << "answer :" << answer << std::endl;
		return false;
	}

	return true;
}

bool TdsComPort::simple_modbus_exchange()
{
	std::vector<uint8_t> request{ 0x83, 0x03, 0x00, 0x00, 0x00, 0x01, 0x9A, 0x28 };

    //print_vector("modbus request : ", request);

	int rc = write(data_socket, request.data(), request.size());
	if (rc < 0)
	{
        std::cout << " write request error" << std::endl;
		return false;
	}

	char buff[256];
	rc = read(data_socket, buff, sizeof(buff));
	if (rc < 0)
	{
		printf("read answer error err!");
		return false;
	}
	if (read == 0)
	{
		printf("If 0 bytes are received, the connection was closed..\n");
		return false;
	}

	//expected 83 03 02 00 01 01 9A 
	std::vector<uint8_t> answer{ buff, buff + rc };

    //print_vector("modbus answer : ", answer);

	return  true;

}


bool TdsComPort::modbus_exchange(const std::vector<uint8_t>& request, std::vector<uint8_t>& answer, const int fun_num, const int expected_answer_len, const int exception_len)
{
    //print_vector("modbus request : ", request);

	int rc = write(data_socket, request.data(), request.size());
	if (rc < 0)
	{
        std::cout << " write request error" << std::endl;
		return false;
	}

	while (answer.size() < expected_answer_len)
	{
		char buff[256];
		rc = read(data_socket, buff, sizeof(buff));
		if (rc < 0)
		{
			printf("read answer error err : rc = %d, errno = %d \n", rc, errno);
			if(EAGAIN == errno)
				printf("read() produces errno == EAGAIN, timeout occurs\n");
			return false;
		}
		if (read == 0)
		{
			printf("If 0 bytes are received, the connection was closed..\n");
			return false;
		}

		
		answer.insert(answer.end(), buff, buff + rc);

		if (answer.size() == exception_len)
			if (answer.at(7) == (0x80 | fun_num))
			{
				printf("Exception code = %d \n", answer.at(8));
				LogException(answer.at(8));
				answer.clear();
				return false;
			}
	}

    //print_vector("modbus answer : ", answer);

	return  true;

}


