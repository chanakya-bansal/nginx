#include "http_response.hpp"

#include <fstream>
#include <sstream>

namespace {

// Reads an entire file's contents into a string.
// Returns false if the file couldn't be opened.
bool readFile(const std::string& filepath, std::string& out_contents) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    out_contents = buffer.str();
    return true;
}

// Maps a URL path to a file on disk under www/.
std::string mapPathToFile(const std::string& path) {
    if (path == "/") {
        return "www/index.html";
    }
    if (path.find("..") != std::string::npos) {
        return "";
    }
    return "www" + path; 
}


std::string contentTypeFor(const std::string& filepath) {
    size_t dot = filepath.find_last_of('.');
    if (dot == std::string::npos) {
        return "application/octet-stream"; // unknown type, generic binary fallback
    }

    std::string ext = filepath.substr(dot + 1);

    if (ext == "html") return "text/html";
    if (ext == "css")  return "text/css";
    if (ext == "js")   return "application/javascript";
    if (ext == "png")  return "image/png";
    if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
    if (ext == "gif")  return "image/gif";
    if (ext == "svg")  return "image/svg+xml";
    if (ext == "ico")  return "image/x-icon";

    return "application/octet-stream";
}

} // namespace

std::string buildResponse(const HttpRequest& request) {
    std::string status;
    std::string body;
    std::string content_type = "text/html";

    if (request.path.empty()) {
        status = "400 Bad Request";
        body = "<h1>400 Bad Request</h1>";
    } else {
        std::string filepath = mapPathToFile(request.path);
        std::string file_contents;

        if (!filepath.empty() && readFile(filepath, file_contents)) {
            status = "200 OK";
            body = file_contents;
            content_type = contentTypeFor(filepath);
        } else {
            // Either file traversal was rejected, or file doesn't exist —
            // serve 404 page either way.
            status = "404 Not Found";
            if (!readFile("www/404.html", body)) {
                body = "<h1>404 Not Found</h1>"; // fallback if even 404.html is missing
            }
        }
    }

    std::string response =
        "HTTP/1.1 " + status + "\r\n" +
        "Content-Type: " + content_type + "\r\n" +
        "Content-Length: " + std::to_string(body.size()) + "\r\n" +
        "Connection: close\r\n" +
        "\r\n" +
        body;

    return response;
}