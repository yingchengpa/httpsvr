#pragma once

#define BUF_MAX (1024*16)


namespace urlhandle {

    void request_cb(evhttp_request *req, void *arg);
}