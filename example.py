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
    parsed = picohttpparser.parse_http_request(raw_request)
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
    result = parser_req_raw_example(request_raw=raw_request) 
    import json 
    print(json.dumps(result, indent=2))
