
#include "stdafx.h"

namespace action {

    std::tuple<int, std::string,std::string> postRequest(const std::string &strInput, const std::map<std::string, std::string>& oQuery)
    {
        LOG_TRACE("the action  body is \n%s", strInput.c_str());
		std::string strResp = "hello this is response";

		return std::make_tuple(HTTP_OK, R"(OK)", strResp);
    }
}