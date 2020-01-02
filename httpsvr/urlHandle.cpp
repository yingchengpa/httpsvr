#include "stdafx.h"
#include "urlHandle.h"

namespace urlhandle{

	void print_uri_parts_info(evhttp_request* req)
	{
		const evhttp_uri* uri = evhttp_request_get_evhttp_uri(req);

		LOG_TRACE("scheme:%s", evhttp_uri_get_scheme(uri));
		LOG_TRACE("host:%s", evhttp_uri_get_host(uri));
		LOG_TRACE("path:%s", evhttp_uri_get_path(uri));
		LOG_TRACE("port:%d", evhttp_uri_get_port(uri));
		LOG_TRACE("query:%s", evhttp_uri_get_query(uri));
		LOG_TRACE("userinfo:%s", evhttp_uri_get_userinfo(uri));
		LOG_TRACE("fragment:%s", evhttp_uri_get_fragment(uri));
	}

	void get_uri_query_map(const std::string& strUrl, std::map<std::string, std::string>& oMap)
	{
		//  127.0.0.1:8090/v1.0/open/aiaction?param1=1&param2=b
		evkeyvalq  pars;
		evhttp_parse_query(strUrl.c_str(), &pars);
		for (evkeyval*  header = pars.tqh_first; header;header = header->next.tqe_next)
		{
			oMap[header->key] = header->value;
		}
	}

	void get_request_body(evhttp_request* req,std::string &strBody)
	{
		evbuffer* buf = evhttp_request_get_input_buffer(req);
		const size_t body_size = evbuffer_get_length(buf);
		if (body_size > 0)
		{
			const size_t copy_len = body_size > BUF_MAX ? BUF_MAX : body_size;

			std::unique_ptr<char[]> bodydata(new char[copy_len]);

			memset(bodydata.get(), 0, copy_len);
			memcpy(bodydata.get(), evbuffer_pullup(buf, -1), copy_len);

			//assign the byte data , include '\0' char.
			strBody.assign(bodydata.get(), copy_len);
		}
	}

	void url_method_dowith(evhttp_request* req, httpFuncPtr func)
	{
		DWORD dwStart = GetTickCount();

		// get url
		std::string strUrl = evhttp_request_get_uri(req);

		////get head
		//evkeyvalq *headers = evhttp_request_get_input_headers(req);

		////trace the heads 
		//for (evkeyval *header = headers->tqh_first; header; header = header->next.tqe_next) {
		//    LOG_TRACE("  %s: %s", header->key, header->value);
		//}

		//get the query map
		std::map<std::string, std::string> oQueryMap;
		get_uri_query_map(strUrl,oQueryMap);

		//get the body
		std::string strBody;
		get_request_body(req, strBody);

		//LOG_DEBUG("url is %s , body is %s", strUrl.c_str(), strBody.c_str());

		//call the register func
		std::string strResponse;	//
		if (func)
		{
			int nRet = 0;			 // http errcode , HTTP_OK HTTP_NOCONTENT ......
			std::string strRetDes;	 // describe of errcode 

			std::string strBody;
			get_request_body(req, strBody);
			//url
			//body	  
			//queryMap
			std::tie(nRet, strRetDes, strResponse) = func(strUrl, strBody, oQueryMap);

			if (!strResponse.empty())
			{
				evbuffer* retbuff = nullptr;
				retbuff = evbuffer_new();

				//append the byte data ：evbuffer_add_reference只是临时copy，再未发送前会释放内存
				evbuffer_add(retbuff, strResponse.c_str(), strResponse.size());

				evhttp_send_reply(req, nRet, strRetDes.c_str(), retbuff);
				evbuffer_free(retbuff);
			}
			else
			{
				evhttp_send_reply(req, nRet, strRetDes.c_str(), NULL);
			}
		}
		else
		{
			evhttp_send_reply(req, HTTP_OK, "OK", NULL);
		}

	}

    void default_request(evhttp_request *req)
    {
        evhttp_send_reply(req, HTTP_NOTFOUND, "not found", NULL);
    }

	void request_cb(evhttp_request* req, void* arg)
	{
		auto& obj = *static_cast<std::map<evhttp_cmd_type, httpFuncPtr>*>(arg);

		const evhttp_cmd_type nCmd = evhttp_request_get_command(req);

		switch (nCmd) {
		case EVHTTP_REQ_POST:
		case EVHTTP_REQ_GET:
			url_method_dowith(req, obj[nCmd]);
			break;
		default:
			default_request(req);
			break;
		}	
	}
}