#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "picohttpparser.h"

namespace py = pybind11;

py::dict parse_http_request(const std::string& request_raw) {
    const char* buf = request_raw.data();
    size_t buf_len = request_raw.size();
    
    // 使用正确的类型声明变量
    const char *method, *path;
    size_t method_len, path_len;
    int minor_version;
    struct phr_header headers[100];
    size_t num_headers = sizeof(headers) / sizeof(headers[0]); // 类型为 size_t
    size_t prev_buf_len = 0;

    // 调用 phr_parse_request
    int ret = phr_parse_request(
        buf, buf_len,
        &method, &method_len,    // 正确类型: const char​**​, size_t*
        &path, &path_len,        // 正确类型: const char​**​, size_t*
        &minor_version,          // int*
        headers,                 // struct phr_header*
        &num_headers,            // size_t*（关键修正）
        prev_buf_len             // size_t
    );

    if (ret <= 0) {
        throw std::runtime_error("解析失败");
    }

    // 提取 Body（从 ret 位置到结尾）
    size_t body_start = ret;
    size_t body_len = buf_len - body_start;
    const char* body_data = buf + body_start;

    // 构建 Python 字典
    py::dict result;
    result["method"] = std::string(method, method_len);
    result["path"] = std::string(path, path_len);
    result["version"] = minor_version == 1 ? "HTTP/1.1" : "HTTP/1.0";

    // 处理头部字段
    py::dict headers_dict;
    for (size_t i = 0; i < num_headers; ++i) {  // 注意循环变量类型改为 size_t
        std::string name(headers[i].name, headers[i].name_len);
        std::string value(headers[i].value, headers[i].value_len);
        headers_dict[name.c_str()] = value;
    }
    result["headers"] = headers_dict;

    // 处理 Body
    if (body_len > 0) {
        result["body"] = py::bytes(body_data, body_len);
    } else {
        result["body"] = py::none();
    }

    return result;
}

PYBIND11_MODULE(http_parser, m) {
    m.def("parse_http_request", &parse_http_request, "解析 HTTP 请求原始数据");
}
