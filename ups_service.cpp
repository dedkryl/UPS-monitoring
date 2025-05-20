
#include "ups_service.h"
#include <iostream>
#include "HandledData.h"


extern HandledData out_data_holder;

bool ups_service::handle(rest_request& req) 
{
    if (req.method != "HEAD" && req.method != "GET" && req.method != "POST")
         return req.respond_method_not_allowed("HEAD, GET, POST");

    std::cout << " req.url :  "<< req.url << std::endl;

    if (!req.url.empty()) {
      if((req.url == "/data") || (req.url == "/"))
      {  
        //return req.respond("application/json", "{ \"aaa\" : bbb }");
        if(out_data_holder.is_device_connected())
          return req.respond("application/json",out_data_holder.GetOutputJson());
        else
          return req.respond("application/json", "{ \"Error  : \" : \"Data from DRS-240-12 is not available\" }");
      }
      else if(req.url == "/text")    
        return req.respond("text/plain", "Hello, Eugene");  
      else if(req.url == "/table")  
        return req.respond("text/html", "<!DOCTYPE html> <html> <body> <h1>My First Heading</h1> <p>My first paragraph.</p> </body> </html>");  
      else
        return req.respond("text/plain", "Unexpected URL");  
    }

    return req.respond_not_found();
  }