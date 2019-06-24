// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

//#include <afx.h>

#include <stdio.h>
#include <tchar.h>

#include <WinSock2.h>

// TODO:  在此处引用程序需要的其他头文件
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <mutex>
#include <list>
#include <thread>

// libevent http
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/keyvalq_struct.h>

#include "httpsvr.h"

#include "urlHandle.h"
#include "action.h"
#include "ai.h"
