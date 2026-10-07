#pragma once

#include "http_parser.hpp"
#include <string>


std::string buildResponse(const HttpRequest& request);