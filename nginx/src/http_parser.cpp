#include "http_parser.hpp"

HttpRequest parseRequest(const std::string& raw) {
    HttpRequest req;

    size_t line_end = raw.find("\r\n");
    std::string request_line = raw.substr(0, line_end);

    size_t first_space = request_line.find(' ');
    size_t second_space = request_line.find(' ', first_space + 1);

    if (first_space == std::string::npos || second_space == std::string::npos) {
        return req; // req.path stays empty — caller treats this as malformed
    }

    req.method = request_line.substr(0, first_space);
    req.path = request_line.substr(first_space + 1, second_space - first_space - 1);

    return req;
}