#pragma once

#define BUF_MAX (1024*16)

typedef std::tuple<int, std::string, std::string>(*funcPtr)(const std::string& strPB, const std::map<std::string, std::string>& oQuery);

#define LOG_ERROR printf
#define LOG_INFO  printf
#define LOG_TRACE printf