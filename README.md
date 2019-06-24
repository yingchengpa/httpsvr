# 编译环境
vs2013

# 使用说明

## 单线程模式
> startHttpSvrSingle

## 多线程模式
> startHttpSvrMult

# 回调设置
示例中演示了action、ai 路径的post处理回调
~~~
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
~~~

# 回调参数说明
- strInput : http的body数据 std::string
- oQuery   : uri 的参数查询

# 返回值
- http 错误码
- http 错误描述
- 返回的body内容