#include "html_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "router.h"
#include <fmt/core.h>

 html_server::html_server(int port) : match(router) {
    std::ifstream f("cine.json");
    data = json::parse(f);
    start(port);
     for (int i = 0; i < 5; i++) {
         for (int j = 0; j < 4; j++) {
             for (int k = 0; k < 5; j++){
                 seats[i][j][k] = 0;
             }
         }
     }
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
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "\r\n";
    response << body;
    return response.str();
}

bool html_server::parse_http_request(const char *buffer, std::string &method, std::string &path, std::string &version,
                                     std::map<std::string, std::string> &query_params,  std::string &body) {

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
    // Extract the body from the request
    std::string body_line;
    while (std::getline(iss, body_line)) {
        // Skip any empty lines until the body is reached
        if (body_line.empty()) {
            break;
        }
    }
    body = iss.str();
    return true;
}

void html_server::handle_request() {
    char buffer[1024] = {0};
    read(client_sock, buffer, 1024);


    std::string method, path, version, body;
    std::map<std::string, std::string> query_params;
    if (!parse_http_request(buffer, method, path, version, query_params, body)) {
        std::stringstream response;
        response << "HTTP/1.1 400 Bad Request\r\n\r\n";
        write(client_sock, response.str().c_str(), response.str().size());
        return;
    }

    match = router.set(path);
    log(method, path, version, query_params);


    // First endpoint get asientos
    if (match.test( "/sala/:sala/horario/:horario/asientos")) {
        std::string res = generate_http_response(get_asientos(query_params), "application/json");
        write(client_sock, res.c_str(), res.size());
    }else if(match.test("/sala/:sala/horario/:horario/asientos/:asientos")) {
        std::string res = generate_http_response(patch_asientos(query_params, body), "application/json");
        write(client_sock, res.c_str(), res.size());
    } else if(match.test("/sala")) {
        std::string body = data.dump().c_str();
        std::string res = generate_http_response(body, "application/json");
        write(client_sock, res.c_str(), res.size());
    } else if(path == "/SoldOut.html"){
        std::string cantidad = query_params["cantidad"];
        std::string horario = query_params["horario"];
        std::string pelicula = query_params["pelicula"];
        std::string sala = query_params["sala"];
        std::string restante = query_params["restante"];

        std::string file_path = "./SoldOut.html";
        std::cout << "Sold Out ☢️" << file_path << "\n";
        std::ifstream file(file_path);
        std::stringstream response;
        std::string line;
        while (std::getline(file, line)) {
            response << line << std::endl;
        }
        std::string SoldOut_tmp = response.str();
        std::string imgUrl;
        if(pelicula=="1"){pelicula = "Star Wars: The Force Awakens"; imgUrl = "peli1.jpg";}
        if(pelicula=="2"){ pelicula = "Brave: a Disney Pixar movie"; imgUrl = "peli2.jpg";}
        if(pelicula=="3"){ pelicula = "Interstellar"; imgUrl = "peli3.jpeg";}
        if(pelicula=="4"){ pelicula = "Warcraft"; imgUrl = "peli4.jpeg";}
        if(pelicula=="5"){ pelicula = "The maze runner"; imgUrl = "peli5.jpeg";}
        // horario
        if(horario=="1"){ horario = "Horario 10:00 AM";}
        if(horario=="2"){ horario = "Horario 1:00 PM";}
        if(horario=="3"){ horario = "Horario 4:00 PM";}
        if(horario=="4"){ horario = "Horario 7:00 PM";}
        std::string soldOut = fmt::format(fmt::runtime(SoldOut_tmp), fmt::arg("pelicula",pelicula),
                                                  fmt::arg("pelicula",pelicula),
                                                  fmt::arg("cantidad",cantidad),
                                                  fmt::arg("horario",horario),
                                                  fmt::arg("sala",sala),
                                                  fmt::arg("restante", restante),
                                                  fmt::arg("img", imgUrl)
        );

        std::string  res = generate_http_response(soldOut, "text/html");
        write(client_sock, res.c_str(), res.size());
    }
    else if( path == "/ticket.html"){
      render_ticket(query_params);
    }
    else if( !query_params.empty()){
        std::string cantidad = query_params["cantidad"];
        std::string horario = query_params["horario"];
        std::string pelicula = query_params["pelicula"];
        std::string sala = query_params["sala"];


        int aumento = std::stoi(cantidad);
        int horario_i = std::stoi(horario);
        int pelicula_i = std::stoi(pelicula);
        int sala_i = std::stoi(sala);



        std::string params = fmt::format("cantidad={}&horario={}&pelicula={}&sala={}", cantidad, horario, pelicula, sala);
        std::stringstream redirectResponse;
        redirectResponse << "HTTP/1.1 302 Found\r\n";
        redirectResponse << "Location: /ticket.html?"<< params <<"\r\n\r\n";
        std::string res = redirectResponse.str();
        if((seats[pelicula_i - 1][horario_i - 1][sala_i - 1] + aumento > 10)){
            int restante = 10 - seats[pelicula_i - 1][horario_i - 1][sala_i - 1];
            params = fmt::format("cantidad={}&horario={}&pelicula={}&sala={}&restante={}", cantidad, horario, pelicula, sala, restante);
            std::stringstream noSeats;
            noSeats << "HTTP/1.1 302 Found\r\n";
            noSeats << "Location: /SoldOut.html?"<< params <<"\r\n\r\n";
            res = noSeats.str();
        }else{
            seats[pelicula_i - 1][horario_i - 1][sala_i - 1] += aumento;
        }

        write(client_sock, res.c_str(), res.size());
    }
    else{
        def_route(path);
    }
}
void html_server::render_ticket(std::map<std::string, std::string> &query_params) {
    std::string file_path = "./ticket2.html";
    std::cout << "TICKET 🎫" << file_path << "\n";
    std::ifstream file(file_path);
    std::stringstream response;
    std::string line;
    while (std::getline(file, line)) {
        response << line << std::endl;
    }
    std::string ticket_template = response.str();

    std::string pelicula, cantidad, horario, sala;
    cantidad = query_params["cantidad"];
    horario = query_params["horario"];
    sala = query_params["sala"];
    pelicula = query_params["pelicula"];
    std::string imgUrl;
    if(pelicula=="1"){pelicula = "Star Wars: The Force Awakens"; imgUrl = "peli1.jpg";}
    if(pelicula=="2"){ pelicula = "Brave: a Disney Pixar movie"; imgUrl = "peli2.jpg";}
    if(pelicula=="3"){ pelicula = "Interstellar"; imgUrl = "peli3.jpeg";}
    if(pelicula=="4"){ pelicula = "Warcraft"; imgUrl = "peli4.jpeg";}
    if(pelicula=="5"){ pelicula = "The maze runner"; imgUrl = "peli5.jpeg";}
    // horario
    if(horario=="1"){ horario = "Horario 10:00 AM";}
    if(horario=="2"){ horario = "Horario 1:00 PM";}
    if(horario=="3"){ horario = "Horario 4:00 PM";}
    if(horario=="4"){ horario = "Horario 7:00 PM";}

    int total = std::stoi(cantidad)*100;

    std::string ticket_formated = fmt::format(fmt::runtime(ticket_template), fmt::arg("pelicula",pelicula),
                                              fmt::arg("pelicula",pelicula),
                                              fmt::arg("cantidad",cantidad),
                                              fmt::arg("horario",horario),
                                              fmt::arg("sala",sala),
                                              fmt::arg("total", total),
                                              fmt::arg("img", imgUrl)
                                              );
    std::string  res = generate_http_response(ticket_formated, "text/html");
    write(client_sock, res.c_str(), res.size());
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

std::string html_server::get_asientos(const std::map<std::string, std::string> &query_params) {
//    std::cout << "Matched /prueba.html/:id id="<< match.get("id").value_or("not found") << "\n";
    auto sala_s = match.get("sala");
    int sala = std::stoi(sala_s->c_str());
    auto horario_s = match.get("horario");
    int horario = std::stoi(horario_s->c_str());
    std::cout << "sala: " << sala << " horario: " << horario << '\n';
    auto asientos = data["salas"][sala]["horarios"][horario]["asientos"];
    std::string body = asientos.dump().c_str();
    return body;
}

void html_server::def_route(const std::string& path) {
    std::string file_path = "." + path;

    std::ifstream file(file_path);
    if (!file.good()) {
        std::cout << "File not found 😰";
        deliver_html("/NotFound.html");
    }else{
        deliver_html(path);
    }

}

void html_server::deliver_html(const std::string& path){
    std::string file_path = "." + path;
    std::cout << " file path 🦠" << file_path << "\n";
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

std::string html_server::patch_asientos(const std::map<std::string, std::string> &query_params, std::string body) {
    int sala, horario, asientos;

    std::cout << "Matched PATCH asientos\n";
    sala = std::stoi(match.get("sala")->c_str());
    horario = std::stoi(match.get("horario")->c_str());
    asientos = std::stoi(match.get("asientos")->c_str());

    std::cout << "sala: " << sala << " horario: " << horario << "asiento: " << asientos<<'\n';
    data["salas"][sala]["horarios"][horario]["asientos"][asientos]["estado"] = "vendido";
    return body;
}
