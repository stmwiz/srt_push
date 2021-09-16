//
// Created by x on 2021/8/18.
//

#pragma once


#include <string>

namespace xlab::base::Network {

extern int GetAvailableListenPort(std::string clientIP = "127.0.0.1");

extern std::string GetLocalIP(std::string networkSegment = std::string());

extern std::string GetLocalIPForNetworkInterface(std::string networkInterface = std::string());

extern bool CheckWireless(const std::string& networkInterface,std::string &protocol);

extern bool CheckSocketPortAvilable(uint16_t port);

extern bool CheckSocketTimeWait(uint16_t port);

}

