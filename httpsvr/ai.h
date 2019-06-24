
#pragma once

namespace ai {

	//返回值 int ：http 错误码 HTTP_OK 、HTTP_NOTFOUND ......
	//      std::string 错误描述
	//      std::string 返回的body
    std::tuple<int, std::string,std::string> postRequest(const std::string &strInput, const std::map<std::string, std::string>& oQuery);
}