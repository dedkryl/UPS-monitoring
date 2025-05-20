#pragma once

#include <string>
#include <bitset>

struct Data
{
    //"CHG_STATUS"
    bool BTNC;//Battery detection
    bool BUFFTOF;//Time out flag of buffering
    bool CCM;//Constant current mode status
    bool CVM;// Constant voltage mode status
    bool DCM;// Battery discharge mode
    bool FULLM;// Fully charged mode status
    bool FVM;// Float mode status
    bool NTCER;//Temperature compensation status
    //FAULT_STATUS
    bool AC_FAIL;//AC abnormal flag
    bool HI_TEMP;//High ambient temperature protection
    bool OLP;//Output over current protection
    bool OP_OFF;//DC status
    bool OTP;//Over temperature protection
    bool OVP;//Output over-voltage protection
    bool SHORT;//Short circuit protection
    //SYSTEM_STATUS;
    bool CHG_UPS;
    bool DC_OK;// Secondary DC output voltage status
    bool EEPER;//EEPROM data access error
    bool INITIAL_STATE;//Device initialized status
    float READ_IBAT;
    float READ_IOUT;
    float READ_TEMPERATURE;
    float READ_VBAT;
    float READ_VIN;
    float READ_VOUT;
    float VOUT_SET;
     
    time_t timestamp;

};
//forward
class ModbusObject;

class SmartHandlerBase
{
protected:    
    //copy from mo
    std::string command_name;
    std::string description;
   
   
public:    
    SmartHandlerBase() {};
    virtual ~SmartHandlerBase() {};
    virtual void update(const unsigned char* buf, const int buf_size) = 0;
    virtual void handle() = 0;
    static Data data;

};

class SmartHandlerFaultStatus : public SmartHandlerBase
{
    std::bitset<8> bsLowByte;
public:    
    SmartHandlerFaultStatus(const unsigned char* buf, const int buf_size, const ModbusObject* mo); 
    virtual ~SmartHandlerFaultStatus() override {} ;
    virtual void update(const unsigned char* buf, const int buf_size);
    virtual void handle() override ;
};


class SmartHandlerSystemStatus : public SmartHandlerBase
{
    std::bitset<8> bsLowByte;
public:    
    SmartHandlerSystemStatus(const unsigned char* buf, const int buf_size, const ModbusObject* mo); 
    virtual ~SmartHandlerSystemStatus() override {} ;
    virtual void update(const unsigned char* buf, const int buf_size);
    virtual void handle() override ;
};



class SmartHandlerChargingStatus : public SmartHandlerBase
{
    std::bitset<8> bsHighByte;
    std::bitset<8> bsLowByte;
public:    
    SmartHandlerChargingStatus(const unsigned char* buf, const int buf_size, const ModbusObject* mo); 
    virtual ~SmartHandlerChargingStatus() override {} ;
    virtual void update(const unsigned char* buf, const int buf_size);
    virtual void handle() override ;
};


class SmartHandlerAnalog : public SmartHandlerBase
{
    float coeff;
    int16_t twoBytes;//NB : signed!
    float Value;
public:    
    SmartHandlerAnalog(const unsigned char* buf, const int buf_size, const float coeff, const ModbusObject* mo); 
    virtual ~SmartHandlerAnalog() override {} ;
    virtual void update(const unsigned char* buf, const int buf_size);
    virtual void handle() override ;
};

