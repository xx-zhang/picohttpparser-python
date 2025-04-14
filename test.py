from urllib import urlparse

import http_parser # type: ignore

raw_request = (
    b"GET /path?name=Alice&age=30'aaa=112222 HTTP/1.1\r\n"
    b"Host: example.com\r\n"
    b"Content-Type: text/plain\r\n\r\n"
    b"body content ffffffffffffffff"
)

parsed = http_parser.parse_http_request(raw_request)
print("Method:", parsed["method"])
print("Path:", parsed["path"])
print("Headers:", dict(parsed["headers"]))
# print("Args:", dict(parsed["args"]))  # 需要完善查询参数解析逻辑

import json 
# json.dumps(parsed, indent=2)
print(parsed.keys())
print(parsed["body"].decode())
print(parsed["headers"])
