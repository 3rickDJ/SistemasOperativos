#include "html_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "router.h"

 html_server::html_server(int port) {
    std::ifstream f("cine.json");
    data = json::parse(f);
     start(port);
}

void html_server::start(int PORT) {
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    int addrlen = sizeof(server_addr);

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Listening on port " << PORT << std::endl;

    while (true) {
        if ((client_sock = accept(server_sock, (struct sockaddr *) &client_addr, (socklen_t *) &addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        handle_request();
        close(client_sock);
    }

}

std::string html_server::generate_http_response(const std::string &body, const std::string &content_type) {
    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "\r\n";
    response << body;
    return response.str();
}

bool html_server::parse_http_request(const char *buffer, std::string &method, std::string &path, std::string &version,
                                     std::map<std::string, std::string> &query_params) {

    std::istringstream iss(buffer);
    std::string request_line;
    if (!std::getline(iss, request_line)) {
        return false;
    }

    std::istringstream iss_request_line(request_line);
    if (!(iss_request_line >> method >> path >> version)) {
        return false;
    }

    size_t pos = path.find("?");
    if (pos != std::string::npos) {
        std::string query_string = path.substr(pos + 1);
        path = path.substr(0, pos);
        std::istringstream iss_query_string(query_string);
        std::string param;
        while (std::getline(iss_query_string, param, '&')) {
            size_t equals_pos = param.find("=");
            if (equals_pos != std::string::npos) {
                std::string key = param.substr(0, equals_pos);
                std::string value = param.substr(equals_pos + 1);
                query_params[key] = value;
            }
        }
    }

    return true;
}

void html_server::handle_request() {
    char buffer[1024] = {0};
    read(client_sock, buffer, 1024);


    std::string method, path, version;
    std::map<std::string, std::string> query_params;
    if (!parse_http_request(buffer, method, path, version, query_params)) {
        std::stringstream response;
        response << "HTTP/1.1 400 Bad Request\r\n\r\n";
        write(client_sock, response.str().c_str(), response.str().size());
        return;
    }

    route::Route router;
    route::Match match = router.set(path);
    log(method, path, version, query_params);


    // First endpoint get asientos
    if (match.test( "/sala/:sala/horario/:horario/asientos")){
        std::cout << "Matched /prueba.html/:id id="<< match.get("id").value_or("not found") << std::endl;
        auto sala_s = match.get("sala");
        int sala = std::stoi(sala_s->c_str());
        auto horario_s = match.get("horario");
        int horario = std::stoi(horario_s->c_str());
        std::cout << "sala: " << sala << " horario: " << horario << '\n';
        auto asientos = data["salas"][sala]["horarios"][horario]["asientos"];
    }


    def_route(path);
}

void html_server::log(const std::string& method, const std::string& path, const std::string& version,
                      const std::map<std::string, std::string>& query_params) {
    std::cout << "\n\n\nRequest💌:" << std::endl;
    std::cout << "method: " << method << std::endl;
    std::cout << "path: " << path << std::endl;
    std::cout << "version: " << version << std::endl;
    std::cout << "query_params: " << std::endl;
    for (auto& [key, value] : query_params) {
        std::cout << "\t" << key << " = " << value << std::endl;
    }
}

void html_server::get_asientos(const std::map<std::string, std::string> &query_params) {

}

void html_server::def_route(const std::string& path) {
    std::string file_path = "." + path;

    std::ifstream file(file_path);
    if (!file.good()) {
        deliver_html("NotFound.html");
    }else{
        deliver_html(path);
    }

}

void html_server::deliver_html(const std::string& path){
    std::string file_path = "." + path;
    std::ifstream file(file_path);
    std::stringstream response;
    //response << "HTTP/1.1 200 OK\r\n\r\n";
    std::string line;
    while (std::getline(file, line)) {
        response << line << std::endl;
    }
    std::string  res = generate_http_response(response.str(), "text/html");
    write(client_sock, res.c_str(), res.size());
}
