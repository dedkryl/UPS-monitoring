#include <iostream>
#include "../utils.h"
#include "SmartHandler.h"
#include "../HandledData.h"
#include "ModbusObject.h"
#include <cstring>
#include <cassert>

//TODO create  as singleton
HandledData out_data_holder;

Data SmartHandlerBase::data;

////////////////////////////////////////////////////////////////////////////////////
//SmartHandlerFaultStatus

//Low byte

//Bit 1 : OTP:Over temperature protection
//0=Normal internal temperature
//1=Abnormal internal temperature

//Bit 2 : OVP:Output over-voltage protection
//0=Normal output voltage
//1=Abnormal output

//Bit 3 : OLP:Output over current protection
//0=Normal output current
//1=Abnormal output current

//Bit 4 : SHORT:Short circuit protection
//0=Shorted circuit does not exist
//1=Shorted circuit protected

//Bit 5 : AC_FAIL:AC abnormal flag
//0=Normal AC range
//1=Abnormal AC range

//Bit 6 : OP_OFF:DC status
//0=DC turned on
//1=DC turned off

//Bit 7 : HI_TEMP:High ambient temperature protection
//0=Normal ambient temperature
//1=Abnormal ambient temperature

SmartHandlerFaultStatus::SmartHandlerFaultStatus(const unsigned char* buf, const int buf_size, const ModbusObject* mo)
{
      command_name = mo->command_name;
      description = mo->description;
      //std::cout << command_name << " : " <<  buf_size << " bytes " << std::endl;
      bsLowByte = *(buf + 1);
      //std::cout <<  " ctor : "<< bsLowByte << std::endl;
}
void SmartHandlerFaultStatus::update(const unsigned char* buf, const int buf_size)
{
      bsLowByte = *(buf + 1);
      //std::cout << " update " << bsLowByte << std::endl;
}
void SmartHandlerFaultStatus::handle()
{
     SUBPARAMS subparams;
      
      if(bsLowByte[1])
      {
            subparams["OTP"] = "Abnormal internal temperature";
            SmartHandlerBase::data.OTP = true;
      }
      else
      {
            subparams["OTP"] = "Normal internal temperature";
            SmartHandlerBase::data.OTP = false;
      }

      if(bsLowByte[2])
      {
            subparams["OVP"] = "Abnormal output";
            SmartHandlerBase::data.OVP = true;
      }
      else
      {
            subparams["OVP"] = "Normal output voltage";
            SmartHandlerBase::data.OVP = false;
      }      

      if(bsLowByte[3])
      {
            subparams["OLP"] = "Abnormal output current";
            SmartHandlerBase::data.OLP = true;
      }
      else
      {
            subparams["OLP"] = "Normal output current";
            SmartHandlerBase::data.OLP = false;
      }      
     
      if(bsLowByte[4])
      {
            subparams["SHORT"] = "Shorted circuit protected";
            SmartHandlerBase::data.SHORT = true;
      }
      else
      {
            subparams["SHORT"] = "Shorted circuit does not exist";
            SmartHandlerBase::data.SHORT = false;
      }      
     
      if(bsLowByte[5])
      {
            subparams["AC_FAIL"] = "Abnormal AC range";
            SmartHandlerBase::data.AC_FAIL = true;
      }
      else
      {
            subparams["AC_FAIL"] = "Normal AC range";
            SmartHandlerBase::data.AC_FAIL = false;
      }      
     
      if(bsLowByte[6])
      {
            subparams["OP_OFF"] = "DC turned off";
            SmartHandlerBase::data.OP_OFF = true;
      }
      else
      {
            subparams["OP_OFF"] = "DC turned on";
            SmartHandlerBase::data.OP_OFF = false;
      }      
     
      if(bsLowByte[7])
      {
            subparams["HI_TEMP"] = "Abnormal ambient temperature";
            SmartHandlerBase::data.HI_TEMP = true;
      }
      else
      {
            subparams["HI_TEMP"] = "Normal ambient temperature";
            SmartHandlerBase::data.HI_TEMP = false;
      }      


      //print_subparams(subparams);

      out_data_holder.Update({command_name ,subparams});
}

///////////////////////////////////////////////////////////////////////////////////////

//Low byte:

//Bit 1 : DC_OK:Secondary DC output voltage status
//0=Secondary DD output voltage status TOO LOW
//1=Secondary DD output voltage status NORMAL

//Bit 5 : INITIAL_STATE:Device initialized status
//0=In initialization status
//1=NOT in initialization status

//Bit 6 : EEPER:EEPROM data access error
//0=Normal EEPROM data access
//1=Abnormal EEPROM data access

//Bit 7 : CHG/UPS:Operation status
//0=Charging mode
//1=UPS mode

SmartHandlerSystemStatus::SmartHandlerSystemStatus(const unsigned char* buf, const int buf_size, const ModbusObject* mo)
{
      command_name = mo->command_name;
      description = mo->description;
      //std::cout << command_name << " : " <<  buf_size << " bytes " << std::endl;
      bsLowByte = *(buf + 1);
      //std::cout <<  " ctor : "<< bsLowByte << std::endl;
}
void SmartHandlerSystemStatus::update(const unsigned char* buf, const int buf_size)
{
      bsLowByte = *(buf + 1);
      //std::cout << " update " << bsLowByte << std::endl;
}
void SmartHandlerSystemStatus::handle()
{
      SUBPARAMS subparams;
      
      if(bsLowByte[1])
      {
            subparams["DC_OK"] = "Secondary DC output voltage status NORMAL";
            data.DC_OK = true;
      }
      else
      {
            subparams["DC_OK"] = "Secondary DC output voltage status TOO LOW";
            data.DC_OK = false;
      }

      if(bsLowByte[5])
      {
            subparams["INITIAL_STATE"] = "NOT in initialization status";
            data.INITIAL_STATE = true;
      }
      else
      {
            subparams["INITIAL_STATE"] = "In initialization status";
            data.INITIAL_STATE = false;
      }      

      if(bsLowByte[6])
      {
            subparams["EEPER"] = "Abnormal EEPROM data access";
            data.EEPER = true;
      }
      else
      {
            subparams["EEPER"] = "Normal EEPROM data access";
            data.EEPER = false;
      }            

      if(bsLowByte[7])
      {
            subparams["CHG_UPS"] = "UPS mode";
            data.CHG_UPS = true;
      }
      else
      {
            subparams["CHG_UPS"] = "Charging mode";
            data.CHG_UPS = false;
      }     

      //print_subparams(subparams);

      out_data_holder.Update({command_name ,subparams});
}

/////////////////////////////////////////////

/*
OUTPUT for CHG_STATUS without battery :

Successful reading from device 
 update : High 00001000
 update : Low 00000000
BTNC: Battery detection : NO battery detected
//Not used BUFFTOF: Time out flag of buffering : NO time out in buffering
CCM: Constant current mode status : The charger NOT in constant current mode
CVM: Constant voltage mode status : The charger NOT in constant voltage mode
DCM: Battery discharge mode : Charging
FULLM: Fully charged mode status : NOT fully charged
// Not used coz 2-stage charging: FVM: Float mode status : The charger NOT in float mode
// Not used NTCER: Temperature compensation status : NO short-circuit in the circuitry of temperature compensation
Successful reading from device 
 
*/

SmartHandlerChargingStatus::SmartHandlerChargingStatus(const unsigned char* buf, const int buf_size, const ModbusObject* mo)
{
      command_name = mo->command_name;
      description = mo->description;
      //std::cout << command_name << " : " <<  buf_size << " bytes " << std::endl;
      bsHighByte = *buf;
      bsLowByte = *(buf + 1);
      //std::cout <<  " ctor : High "<< bsHighByte << std::endl;
      //std::cout <<  " ctor : Low "<< bsLowByte << std::endl;
}
void SmartHandlerChargingStatus::update(const unsigned char* buf, const int buf_size)
{
      bsHighByte = *buf;
      bsLowByte = *(buf + 1);
      //std::cout <<  " update : High "<< bsHighByte << std::endl;
      //std::cout <<  " update : Low "<< bsLowByte << std::endl;
}
void SmartHandlerChargingStatus::handle()
{
      SUBPARAMS subparams;
      
      if(bsLowByte[0])
      {
            subparams["FULLM"] = "Fully charged";
            SmartHandlerBase::data.FULLM = true;
      }      
      else
      {
            subparams["FULLM"] = "NOT fully charged";
            SmartHandlerBase::data.FULLM = false;
      }

      if(bsLowByte[1])
      {
            subparams["CCM"] = "The charger in constant current mode";
            SmartHandlerBase::data.CCM = true;
      }
      else
      {
            subparams["CCM"] = "The charger NOT in constant current mode";
            SmartHandlerBase::data.CCM = false;
      }

      if(bsLowByte[2])
      {
            subparams["CVM"] = "The charger in constant voltage mode";
            SmartHandlerBase::data.CVM = true;
      }
      else
      {
            subparams["CVM"] = "The charger NOT in constant voltage mode";
            SmartHandlerBase::data.CVM = false;      
      }
/*
      if(bsLowByte[3])
      {
            subparams["FVM"] = "The charger in float mode";
            SmartHandlerBase::data.FVM = true;
      }
      else
      {
            subparams["FVM"] = "The charger NOT in float mode";
            SmartHandlerBase::data.FVM = false;
      }
*/
      if(bsLowByte[7])
      {
            subparams["DCM"] = "Discharging";
            SmartHandlerBase::data.DCM = true;      
      }
      else
      {
            subparams["DCM"] = "Charging";
            SmartHandlerBase::data.DCM = false;
      }
/*      
      if(bsHighByte[2])
      {
            subparams["NTCER"] = "The circuitry of temperature compensation has short-circuited";
            SmartHandlerBase::data.NTCER = true;      
      }
      else
      {
            subparams["NTCER"] = "NO short-circuit in the circuitry of temperature compensation";
            SmartHandlerBase::data.NTCER = false;      
      }
*/      
      if(bsHighByte[3])
      {
            subparams["BTNC"] = "NO battery detected";
            SmartHandlerBase::data.BTNC = true;      
      }
      else
      {
            subparams["BTNC"] = "Battery detected";
            SmartHandlerBase::data.BTNC = false;      
      }
/*
      if(bsHighByte[4])
      {      
            subparams["BUFFTOF"] = "Buffering time out";
            SmartHandlerBase::data.BUFFTOF = true;      
      }
      else
      {
            subparams["BUFFTOF"] = "NO time out in buffering";
            SmartHandlerBase::data.BUFFTOF = false;      
      }
*/
      //print_subparams(subparams);

      out_data_holder.Update({command_name ,subparams});
}

/////////////////////////////////////////////


SmartHandlerAnalog::SmartHandlerAnalog(const unsigned char* buf, const int buf_size, const float _coeff,const ModbusObject* mo)
{
      command_name = mo->command_name;
      description = mo->description;
      coeff = _coeff;
      //std::cout << command_name << " : " <<  buf_size << " bytes " << " coeff = " << coeff << std::endl;
      assert(buf_size == sizeof(uint16_t));
      std::memcpy(&twoBytes,buf, buf_size);
      //std::cout <<  " ctor : " << std::hex << twoBytes << std::endl;
}
void SmartHandlerAnalog::update(const unsigned char* buf, const int buf_size)
{
     std::memcpy(&twoBytes,buf, buf_size);
     //std::cout << " update " << std::hex << twoBytes << std::endl;
}
void SmartHandlerAnalog::handle()
{
      SUBPARAMS subparams;

      //std::cout << " twoBytes = 0x" << std::hex << twoBytes << std::endl;

      twoBytes = __builtin_bswap16(twoBytes);
      Value = twoBytes * coeff;
      
      if(command_name == "VOUT_SET")
            SmartHandlerBase::data.VOUT_SET = Value;
      else if(command_name == "READ_VIN")
            SmartHandlerBase::data.READ_VIN = Value;
      else if(command_name == "READ_VOUT")
            SmartHandlerBase::data.READ_VOUT = Value;
      else if(command_name == "READ_IOUT")
      {
            SmartHandlerBase::data.READ_IOUT = Value;
            //std::cout << " READ_IOUT = Value = " << std::dec << Value << std::endl;
      }
      else if(command_name == "READ_TEMPERATURE")
            SmartHandlerBase::data.READ_TEMPERATURE = Value;
      else if(command_name == "READ_IBAT")
      {
            SmartHandlerBase::data.READ_IBAT = Value;
             //std::cout << " READ_IBAT = Value = " << std::dec << Value << std::endl;
      }
      else if(command_name == "READ_VBAT")
            SmartHandlerBase::data.READ_VBAT = Value;      
      
      std::stringstream ss;
      ss << Value;
      subparams.insert({"Value", ss.str()});
      out_data_holder.Update({command_name ,subparams});
}



