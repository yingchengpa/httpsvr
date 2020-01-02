#pragma once


#define LOG_ERROR printf
#define LOG_INFO  printf
#define LOG_TRACE printf

typedef std::tuple<int, std::string, std::string>(*httpFuncPtr)(const std::string& strUrl, const std::string& strBody, std::map<std::string, std::string>& oQuery);

//nPort: http tcp listen port 
//nThreadNum: http thread numbers 
void initHttpSvr(int nPort, int nThreadNum = 4);

//	evhttp_cmd_type : method ,  get¡¢post ¡¢delete ..
//  url  :  /v1/checksvr/onresult
//  func :  register funcname
void addHttpHandle(evhttp_cmd_type,const std::string &strUrl, httpFuncPtr);