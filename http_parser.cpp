#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "picohttpparser.h"

namespace py = pybind11;

py::dict parse_http_request(const std::string& request_raw) {
    const char* buf = request_raw.data();
    size_t buf_len = request_raw.size();
    const char *method = nullptr, *path = nullptr;
    size_t method_len = 0, path_len = 0;
    int minor_version = 0;
    struct phr_header headers[100];
    size_t num_headers = sizeof(headers) / sizeof(headers[0]);
    size_t prev_buf_len = 0;

    py::dict result;

    // 解析请求
    int ret = phr_parse_request(
        buf, buf_len, &method, &method_len, &path, &path_len,
        &minor_version, headers, &num_headers, prev_buf_len
    );

    // 处理解析结果
    if (ret <= 0) {
        // 构造错误信息
        py::dict error;
        error["code"] = ret;
        error["message"] = ret == -1 ? "Invalid request format" : 
                          ret == -2 ? "Incomplete request" : 
                          "Unknown error";
        
        result["error"] = error;
        result["method"] = py::none();
        result["path"] = py::none();
        result["version"] = py::none();
        result["headers"] = py::dict();
        result["body"] = py::none();
        return result;
    }

    // 成功时填充数据
    result["error"] = py::none();
    result["method"] = std::string(method, method_len);
    result["path"] = std::string(path, path_len);
    result["version"] = minor_version == 1 ? "HTTP/1.1" : "HTTP/1.0";

    // 处理头部
    py::dict headers_dict;
    for (size_t i = 0; i < num_headers; ++i) {
        std::string name(headers[i].name, headers[i].name_len);
        std::string value(headers[i].value, headers[i].value_len);
        headers_dict[name.c_str()] = value;
    }
    result["headers"] = headers_dict;

    // 处理 Body
    size_t body_start = ret;
    size_t body_len = buf_len - body_start;
    const char* body_data = buf + body_start;
    result["body"] = body_len > 0 ? py::bytes(body_data, body_len) : py::none();

    return result;
}

PYBIND11_MODULE(picohttpparser, m) {
    m.def("parse_http_request", &parse_http_request, "Parse HTTP request");
}
