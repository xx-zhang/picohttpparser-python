from setuptools import setup, Extension
import pybind11

setup(
    name="picohttpparser",      # 修改库名称
    version="0.0.1",            # 指定版本号
    ext_modules=[
        Extension(
            "picohttpparser",   # 修改模块名称（需与 C++ 代码一致）
            ["http_parser.cpp", "picohttpparser.c"],
            include_dirs=[pybind11.get_include(), "."],
            language="c++",
        )
    ],
    zip_safe=False,
)
