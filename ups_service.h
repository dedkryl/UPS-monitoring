#pragma once


#include "microrestd.h"

using namespace std;
using namespace ufal::microrestd;

class ups_service : public rest_service {
  
 public:
  virtual bool handle(rest_request& req) override; 
};
