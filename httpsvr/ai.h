
#pragma once

namespace ai {

	//����ֵ int ��http ������ HTTP_OK ��HTTP_NOTFOUND ......
	//      std::string ��������
	//      std::string ���ص�body
    std::tuple<int, std::string,std::string> postRequest(const std::string &strInput, const std::map<std::string, std::string>& oQuery);
}