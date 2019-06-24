// httpsvr.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


static void default_request_cb(struct evhttp_request* req, void* arg);

/*
 * 注册指定url下的 post、get、put、delet 的处理函数
 */
std::map<std::string, std::map<evhttp_cmd_type, funcPtr> > g_urlHandle =
{
	{ "/v1.0/open/action",
		{
			{ EVHTTP_REQ_POST, action::postRequest }
		}
	},
	{ "/v1.0/open/ai",
		{
			{ EVHTTP_REQ_POST, ai::postRequest }
		}
	}
};

// 以单线程模式启动httpsvr
int startHttpSvrSingle(const short& nPort)
{
	event_base* base = nullptr;
	evhttp* http = nullptr;
	evhttp_bound_socket* handle = nullptr;

	/* firstly, you should build a event_base looper */
	base = event_base_new();// 返回慢，则需要关闭本地防火墙
	if (!base)
	{
		LOG_ERROR("Couldn't create an event_base: exiting");
		return 1;
	}

	/* sencondly, Create a new evhttp object to handle requests. */
	http = evhttp_new(base);
	if (!http) {
		LOG_ERROR("couldn't create evhttp. Exiting.");
		return 1;
	}


	// 注册url回调函数
	for (auto& v : g_urlHandle)
	{
		evhttp_set_cb(http, v.first.c_str(), urlhandle::request_cb, &(v.second));
	}

	/* We want to accept arbitrary requests, so we need to set a "generic"
	*   * cb.  We can also add callbacks for specific paths. */
	evhttp_set_gencb(http, default_request_cb, NULL);

	/* Now we tell the evhttp what port to listen on */

	handle = evhttp_bind_socket_with_handle(http, "0.0.0.0", nPort);
	if (!handle) {
		LOG_ERROR("couldn't bind to port %d. Exiting.", nPort);
		return 1;
	}

	LOG_INFO("http server bind %d port", nPort);

	event_base_dispatch(base);

	return 0;

}

// 以多线程模式启动httpsvr
evutil_socket_t httpsvr_bindsocket(const short& nPort)
{
	evutil_socket_t  sock_fd = ::socket(AF_INET, SOCK_STREAM, 0); //创建tcp的fd

	evutil_make_socket_nonblocking(sock_fd);//设置非阻塞

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(nPort);

	if (0 != bind(sock_fd, (struct sockaddr*) & addr, sizeof(addr)))
	{
		LOG_ERROR("bind port [%d] failed", nPort);
		return -1;
	}

	if (0 != listen(sock_fd, 10))
	{
		LOG_ERROR("listen failed");
		return -1;
	}
	return sock_fd;
}

void httpsvr_dispatch(void* arg)
{
	event_base_dispatch((struct event_base*)arg);
}

int startHttpSvrMult(const short& nPort, const size_t& nThreadNum)
{
	evutil_socket_t nfd = httpsvr_bindsocket(nPort);

	std::list<std::thread > oThreadList;

	if (-1 != nfd)
	{
		for (size_t i = 0; i < nThreadNum; i++)
		{
			event_base* base = event_base_new();
			evhttp* httpd = evhttp_new(base);

			if (0 != evhttp_accept_socket(httpd, nfd))
			{
				LOG_ERROR("evhttp_accept_socket failed");
			}

			// 注册url回调函数
			for (auto& v : g_urlHandle)
			{
				evhttp_set_cb(httpd, v.first.c_str(), urlhandle::request_cb, &(v.second));
			}

			/* We want to accept arbitrary requests, so we need to set a "generic"
			*   * cb.  We can also add callbacks for specific paths. */
			evhttp_set_gencb(httpd, default_request_cb, NULL);

			std::thread t(httpsvr_dispatch, base);

			oThreadList.push_back(std::move(t));
		}

		LOG_INFO("http server bind %d port", nPort);
		for (auto& v : oThreadList)
		{
			v.join();
		}

	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	// Initialize Winsock
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	int nPort = 8090;
	startHttpSvrMult(nPort, 10);

	return 0;
}

static void default_request_cb(struct evhttp_request* req, void* arg)
{

	const char* cmdtype;
	struct evkeyvalq* headers;
	struct evkeyval* header;

	switch (evhttp_request_get_command(req)) {
	case EVHTTP_REQ_GET: cmdtype = "GET"; break;
	case EVHTTP_REQ_POST: cmdtype = "POST"; break;
	case EVHTTP_REQ_HEAD: cmdtype = "HEAD"; break;
	case EVHTTP_REQ_PUT: cmdtype = "PUT"; break;
	case EVHTTP_REQ_DELETE: cmdtype = "DELETE"; break;
	case EVHTTP_REQ_OPTIONS: cmdtype = "OPTIONS"; break;
	case EVHTTP_REQ_TRACE: cmdtype = "TRACE"; break;
	case EVHTTP_REQ_CONNECT: cmdtype = "CONNECT"; break;
	case EVHTTP_REQ_PATCH: cmdtype = "PATCH"; break;
	default: cmdtype = "unknown"; break;
	}

	LOG_TRACE("Received a %s request for %s\nHeaders: ",
		cmdtype, evhttp_request_get_uri(req));
	headers = evhttp_request_get_input_headers(req);
	for (header = headers->tqh_first; header;
		header = header->next.tqe_next) {
		LOG_TRACE("  %s: %s", header->key, header->value);
	}
	evhttp_send_reply(req, 200, "OK", NULL);
}