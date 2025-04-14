#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "picohttpparser.h"
#include "nlohmann/json.hpp"
#include "base64/base64.h" // 需要 base64 库，例如 https://github.com/ReneNyffenegger/cpp-base64

using json = nlohmann::json;

// 辅助函数：将二进制数据转换为 UTF-8 字符串（忽略错误）
std::string bytes_to_utf8(const char* data, size_t len) {
    return std::string(data, std::find(data, data + len, '\0'));
}

// 解析查询参数
std::map<std::string, std::string> parse_query(const char* query, size_t len) {
    std::map<std::string, std::string> params;
    std::string input(query, len);
    size_t pos = 0;

    while (pos < input.length()) {
        size_t amp_pos = input.find('&', pos);
        std::string pair = input.substr(pos, amp_pos - pos);
        
        size_t eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
            std::string key = pair.substr(0, eq_pos);
            std::string value = pair.substr(eq_pos + 1);
            
            // 简单 URL 解码
            std::replace(key.begin(), key.end(), '+', ' ');
            std::replace(value.begin(), value.end(), '+', ' ');
            
            params[key] = value;
        }
        
        pos = (amp_pos == std::string::npos) ? input.length() : amp_pos + 1;
    }
    return params;
}

json parse_http_request(const char* data, size_t len) {
    json result;
    const char *method, *path;
    size_t method_len, path_len;
    int minor_version;
    struct phr_header headers[100];
    size_t num_headers = sizeof(headers)/sizeof(headers[0]);

    // 解析 HTTP 请求
    int ret = phr_parse_request(
        data, len, &method, &method_len, &path, &path_len,
        &minor_version, headers, &num_headers, 0
    );

    if (ret <= 0) {
        result["error"] = {
            {"code", ret},
            {"message", ret == -1 ? "Invalid format" : "Incomplete request"}
        };
        return result;
    }

    //​**​​**​​**​​**​​**​​**​​**​​**​ 处理路径部分 ​**​​**​​**​​**​​**​​**​​**​​**​/
    std::string full_path(path, path_len);
    size_t qmark_pos = full_path.find('?');
    
    // CGI 路径（不含查询参数）
    result["cgi"] = bytes_to_utf8(path, qmark_pos != std::string::npos ? qmark_pos : path_len);
    
    // CGI 完整路径（包含查询参数）
    result["cgi_full"] = bytes_to_utf8(path, path_len);
    
    // CGI 原始路径
    result["cgi_raw"] = full_path;

    //​**​​**​​**​​**​​**​​**​​**​​**​ 处理查询参数 ​**​​**​​**​​**​​**​​**​​**​​**​/
    if (qmark_pos != std::string::npos) {
        const char* query_start = path + qmark_pos + 1;
        size_t query_len = path_len - (qmark_pos + 1);
        
        // 参数键值对
        result["arg_key"] = parse_query(query_start, query_len);
        
        // 原始查询字符串
        result["arg"] = bytes_to_utf8(query_start, query_len);
    } else {
        result["arg_key"] = json::object();
        result["arg"] = "";
    }

    //​/**​​**​​**​​**​​**​​**​​**​​**​ 处理头部 ​**​​**​​**​​**​​**​​**​​**​​**​/
    json headers_json;
    std::string headers_str;
    std::map<std::string, std::string> cookies;

    for (size_t i = 0; i < num_headers; ++i) {
        std::string key = bytes_to_utf8(headers[i].name, headers[i].name_len);
        std::string value = bytes_to_utf8(headers[i].value, headers[i].value_len);
        
        // 转换为小写
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        
        // 添加到 head_key
        headers_json[key] = value;
        
        // 构建 headers 字符串
        headers_str += key + ": " + value + "\r\n";
        
        // 处理 Cookie
        if (key == "cookie") {
            // 简单 Cookie 解析
            std::string cookie_str = value;
            size_t pos = 0;
            
            while (pos < cookie_str.length()) {
                size_t semi_pos = cookie_str.find(';', pos);
                std::string pair = cookie_str.substr(pos, semi_pos - pos);
                
                size_t eq_pos = pair.find('=');
                if (eq_pos != std::string::npos) {
                    std::string ckey = pair.substr(0, eq_pos);
                    std::string cval = pair.substr(eq_pos + 1);
                    cookies[ckey] = cval;
                }
                
                pos = (semi_pos == std::string::npos) ? cookie_str.length() : semi_pos + 2;
            }
        }
    }
    
    result["head_key"] = headers_json;
    result["head"] = headers_str.empty() ? "" : headers_str.substr(0, headers_str.length() - 2);

    //​**​​**​​**​​**​​**​​**​​**​​**​ 处理 Cookie ​**​​**​​**​​**​​**​​**​​**​​**​/
    result["cookie_key"] = cookies;
    result["cookie_decode"] = "";   // 需要实现解码逻辑
    result["cookie_decode_key"] = json::object();

    //​**​​**​​**​​**​​**​​**​​**​​**​ 处理请求体 ​**​​**​​**​​**​​**​​**​​**​​**​/
    const char* body_start = data + ret;
    size_t body_len = len - ret;
    std::string body_str(body_start, body_len);
    
    // Base64 编码
    result["body_base64"] = base64_encode(body_str);
    
    // 文本内容
    result["body"] = bytes_to_utf8(body_start, body_len);

    //​**​​**​​**​​**​​**​​**​​**​​**​ 其他字段 ​**​​**​​**​​**​​**​​**​​**​​**​/
    result["method"] = bytes_to_utf8(method, method_len);
    result["http_version"] = minor_version == 1 ? "HTTP/1.1" : "HTTP/1.0";
    result["upload_cnt"] = "";
    result["upload_filename"] = "";

    return result;
}

int main(){
    std::string b64Str = "UE9TVCAvYXBpL2hybS9zZWNvbmRhcnlwd2QvY2hlY2tQYXNzd29yZCBIVFRQLzEuMQpIb3N0OiAxOTIuMTY4LjE3NC4xOTQKVXNlci1BZ2VudDogTW96aWxsYS81LjAgKFdpbmRvd3MgTlQgMTAuMDsgV2luNjQ7IHg2NDsgcnY6MTA5LjApIEdlY2tvLzIwMTAwMTAxIEZpcmVmb3gvMTE5LjAKQWNjZXB0OiB0ZXh0L2h0bWwsYXBwbGljYXRpb24veGh0bWwreG1sLGFwcGxpY2F0aW9uL3htbDtxPTAuOSxpbWFnZS9hdmlmLGltYWdlL3dlYnAsKi8qO3E9MC44CkFjY2VwdC1MYW5ndWFnZTogemgtQ04semg7cT0wLjgsemgtVFc7cT0wLjcsemgtSEs7cT0wLjUsZW4tVVM7cT0wLjMsZW47cT0wLjIKQWNjZXB0LUVuY29kaW5nOiBnemlwLCBkZWZsYXRlCkNvbm5lY3Rpb246IGNsb3NlCkNvb2tpZTogZWNvbG9neV9KU2Vzc2lvbmlkPWFhYXhoTTN6bVFOTGRmWXNyS0l5ejsgX19yYW5kY29kZV9fPTJmZGY4MTY1LTgwMWMtNDkwNC1hODA5LTZiOTJlOGQyYjhlYTsgbGFuZ3VhZ2VpZHdlYXZlcj03OyBsb2dpbnV1aWRzPTI5OyBKU0VTU0lPTklEPWFhYXhoTTN6bVFOTGRmWXNyS0l5ejsgbG9naW5pZHdlYXZlcj0yOQpVcGdyYWRlLUluc2VjdXJlLVJlcXVlc3RzOiAxCkNvbnRlbnQtVHlwZTogYXBwbGljYXRpb24veC13d3ctZm9ybS11cmxlbmNvZGVkCkNvbnRlbnQtTGVuZ3RoOiAzNjQKCnBhc3N3b3JkPTxyb290IHBhZ2VzaXplPSIxMCIgcGFnZUluZGV4PSIxIiB0YWJsZXR5cGU9ImNoZWNrYm94Ij48Y2hlY2tib3hwb3BlZG9tIHNob3dtZXRob2Q9ImNvbS5hcGkuaW50ZWdyYXRpb24udXRpbC5GaWxlVXRpbC5jcmVhdGVGaWxlIiBwb3BlZG9tcGFyYT0nQzoveWZzYXB3cWV4cXdlJz48Y2hlY2tib3hMaXN0PjwvY2hlY2tib3hMaXN0PjwvY2hlY2tib3hwb3BlZG9tPjxoZWFkPjwvaGVhZD48c3FsIHNxbHByaW1hcnlrZXk9IklNQUdFRklMRUlEIiBiYWNrZmllbGRzPSJJTUFHRUZJTEVJRCIgc3FsZm9ybT0iSU1BR0VGSUxFIiBzcWxzb3J0d2F5PSJzcWxzb3J0d2F5Ij48c3FsaGludD4xPC9zcWxoaW50Pjwvc3FsPjwvcm9vdD4K"; 
    auto s = base64_decode(b64Str); 
    // auto s_len = s.size();
    auto x = parse_http_request(s.data(), s.size());
    std::cout << x << std::endl; 
    return 0; 
}

// g++ -std=c++11 ./example.cpp picohttpparser.c -I ./thirty_lib ./thirty_lib/base64/base64.cpp -o parser
