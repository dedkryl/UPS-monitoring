#include "utils.h"

#include <iomanip> 
#include <iostream> 



void print_subparams(const std::map<std::string, std::string>& sps)
{
      for (auto &&sp : sps)
      {
            std::cout << sp.first << " : " << sp.second << std::endl;
      }
}

void LogException(int exceptionCode) {
	if (exceptionCode >= 0) {
		const char *exceptionText = NULL;

		switch (exceptionCode) {
		case 0x01:
			exceptionText = "ILLEGAL FUNCTION";
			break;
		case 0x02:
			exceptionText = "ILLEGAL DATA ADDRESS";
			break;
		case 0x03:
			exceptionText = "ILLEGAL DATA VALUE";
			break;
		case 0x04:
			exceptionText = "SLAVE DEVICE FAILURE";
			break;
		case 0x05:
			exceptionText = "ACKNOWLEDGE";
			break;
		case 0x06:
			exceptionText = "SLAVE DEVICE BUSY";
			break;
		case 0x08:
			exceptionText = "MEMORY PARITY ERROR";
			break;
		case 0x0A:
			exceptionText = "GATEWAY PATH UNAVAILABLE";
			break;
		case 0x0B:
			exceptionText = "GATEWAY TARGET DEVICE FAILED TO RESPOND";
			break;
		default:
			exceptionText = "Unknown exception";
			break;
		}

		printf("Device answer exception '0x%X' ('%s')", (unsigned)exceptionCode, exceptionText);
	}
	else {
		printf("ASSERT: exceptionCode");
	}
}
