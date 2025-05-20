#pragma once

#include <map> 
#include <string> 
#include <iomanip> 
#include <iostream> 

template <typename V>
void print_vector(const std::string& caption, V v)
{
    std::cout << caption << " " << std::dec << v.size() << " bytes : " << std::endl;   
    for (int i = 0; i < v.size(); i++)
        std::cout << std::hex << std::setfill('0') << std::setw(2) << std::setprecision(2) << static_cast<unsigned>(v.at(i));
    std::cout << std::endl;
}

void LogException(int exceptionCode);

void print_subparams(const std::map<std::string, std::string>& sps);