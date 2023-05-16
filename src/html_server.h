#pragma once
#include <map>
#include <nlohmann/json.hpp>
#include "router.h"
using json = nlohmann::json;
using namespace route;

class html_server {
public:
    html_server(int port);
    json data;
    int client_sock;
    route::Route router;
    route::Match match;
    void start(int port);
    std::string generate_http_response(const std::string &body, const std::string& content_type);
    bool parse_http_request(const char* buffer, std::string& method, std::string& path, std::string& version, std::map<std::string, std::string>& query_params);
    void handle_request();
    std::string get_asientos(const std::map<std::string, std::string>& query_params);
    void patch_asientos(const std::map<std::string, std::string>& query_params);
private:
    void log(const std::string& method, const std::string& path, const std::string& version, const std::map<std::string, std::string>& query_params );
    void def_route(const std::string& path);
    void deliver_html(const std::string& path);
};