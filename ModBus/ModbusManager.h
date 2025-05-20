#pragma once
#include <vector>
#include "TdsComPort.h"
#include "ModbusObject.h"

///////////////////////////////
#define REQUEST_ATTEMPTS 3

#define UINT16_SWAP_LE_BE_CONSTANT(val)	((WORD) ( \
    (WORD) ((WORD) (val) >> 8) |	\
    (WORD) ((WORD) (val) << 8)))

#define UINT32_SWAP_LE_BE_CONSTANT(val)	((DWORD) ( \
    (((DWORD) (val) & (DWORD) 0x000000ffU) << 24) | \
    (((DWORD) (val) & (DWORD) 0x0000ff00U) <<  8) | \
    (((DWORD) (val) & (DWORD) 0x00ff0000U) >>  8) | \
    (((DWORD) (val) & (DWORD) 0xff000000U) >> 24)))


#define UINT16_SWAP_LE_BE(val) (UINT16_SWAP_LE_BE_CONSTANT (val))
#define UINT32_SWAP_LE_BE(val) (UINT32_SWAP_LE_BE_CONSTANT (val))

#define INT16_TO_LE(val)			((SHORT) (val))
#define UINT16_TO_LE(val)			((WORD) (val))
#define INT16_TO_BE(val)			((SHORT) UINT16_SWAP_LE_BE (val))
#define UINT16_TO_BE(val)			(UINT16_SWAP_LE_BE (val))
#define INT32_TO_LE(val)			((INT) (val))
#define UINT32_TO_LE(val)			((DWORD) (val))
#define INT32_TO_BE(val)			((INT) UINT32_SWAP_LE_BE (val))
#define UINT32_TO_BE(val)			(UINT32_SWAP_LE_BE (val))

#define INT16_FROM_LE(val)			(INT16_TO_LE (val))
#define UINT16_FROM_LE(val)		(UINT16_TO_LE (val))
#define INT16_FROM_BE(val)			(INT16_TO_BE (val))
#define UINT16_FROM_BE(val)		(UINT16_TO_BE (val))
#define INT32_FROM_LE(val)			(INT32_TO_LE (val))
#define UINT32_FROM_LE(val)		(UINT32_TO_LE (val))
#define INT32_FROM_BE(val)			(INT32_TO_BE (val))
#define UINT32_FROM_BE(val)		(UINT32_TO_BE (val))


#define SEND_DATA_BUFSIZE 256
#define RECV_DATA_BUFSIZE 256

#define ExceptionalAnswerTCPFullLen 9
#define ExceptionalAnswerSerialFullLen ExceptionalAnswerTCPFullLen+2


#pragma pack(1)
struct MBAP_Header {
	WORD transactionIdentifier;
	WORD protocolIdentifier;
	WORD length;
	BYTE unitIdentifier;
};
#pragma pack()


struct device_connection_params
{
   std::string tds_ip_address;
   int slave_id;
   int request_period;//in sec 
};


struct rest_server_params
{
    int port;
    int threads;
    int connection_limit;  
};


//////////////////////////////



class ModbusManager
{
    public:
        bool Init(std::string config_fileName);
        bool ReadDataFromDevice();
        rest_server_params get_rest_params() const;
        int get_request_period() const;
        std::string get_tds_ip_address() { return device_params.tds_ip_address; }
    private:
        device_connection_params device_params;
        database_params db_params;
        rest_server_params rest_params;
        TdsComPort comport;
        uint32_t trans_counter = 0;
        std::vector<ModbusObject> modbus_objects;
 
        char SendBuffer[SEND_DATA_BUFSIZE];
	    char RecvBuffer[RECV_DATA_BUFSIZE];

        bool RequestAndAnswer(ModbusObject& mo);
	    bool OneRequestAndAnswer(ModbusObject& mo);

        bool CreateModBusADU(void *dst_, int dstSize, int *pdstLen, BYTE funCode, const struct iovec *srcVector, int srcCount, BYTE devAddress, WORD TransID);
	    int CalcNormalAnswFullLen(int reg_count);
        
        bool db_ready = false;
        std::shared_ptr<DBManager> dbm = nullptr;
	    
};