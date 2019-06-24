#pragma once

enum {
    ACTION_URL = 1,
    AIACTION_URL = 2
};

namespace urlhandle {

    void request_cb(evhttp_request *req, void *arg);

    void post_request(evhttp_request *req, funcPtr);

    void get_request(evhttp_request *req, funcPtr);

    void default_request(evhttp_request *req);
}