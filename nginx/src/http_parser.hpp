#pragma once

#include <cstdint>
#include <string>

struct HttpRequest {
    std::string method;
    std::string path;
};

HttpRequest parseRequest(const std::string& raw);

