#include <iostream>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <iomanip>
#include "router.h"
#include <string>
#include <fstream>

using json = nlohmann::json;
using namespace route;

json data;

int main() {
    std::cout << "Hello, World!" << std::endl;
    fmt::print(fmt::runtime("The answer is {}."), 42);


//    printf("%s\n\n\n\n", res.c_str());
    // ++++++++++++++++++++++++++++

    Route router;

    auto match = router.set("/sala/1/horario/1/asientos/1");
    if( match.test("/sala/:sala/horario/:horario/asientos/:asientos") ){
        std::cout << "\n\n\n🔴matched: sala=> " << *match.get("sala") << ", horario=>" << *match.get("horario") << ", asientos=>" << *match.get("asientos");
    }

    std::cout << "\n\n\n\n";
    match = router.set("/books/1/hola/2");
    if (match.test("/books/:id/hola/:coma")) {
        std::cout << "💌 first: " << *match.get("id") << " second: " << *match.get("coma");
    };
    std::cout << "\n🌟🌟🌟🌟\n\n";
    std::ifstream f("cine.json");
    data = json::parse(f);
    match = router.set("/sala/1/horario/3/asientos");

    // endpoint de asientos de una sala en un determinado horario
    if(match.test("/sala/:sala/horario/:horario/asientos")){
        auto sala_s = match.get("sala");
        int sala = std::stoi(sala_s->c_str());
        auto horario_s = match.get("horario");
        int horario = std::stoi(horario_s->c_str());
        std::cout << "sala: " << sala << " horario: " << horario << '\n';
        auto asientos = data["salas"][sala]["horarios"][horario]["asientos"];
        std::cout << asientos;
//        printf("\n\n%s", asientos.dump().c_str());
    }
    data["salas"][1]["horarios"][1]["asientos"][9]["estado"] = "🚩";
    std::cout << data.dump(2).c_str();

    match = router.set("/sala/1/horario/1/asientos/1");
    if(match.test("/sala/:sala/horario/:horario/asientos/:asientos")){
        std::cout << "\n🐳             🐳\n";
        auto sala_s = match.get("sala");
        int sala = std::stoi(sala_s->c_str());
        auto horario_s = match.get("horario");
        int horario = std::stoi(horario_s->c_str());
        auto asiento_s = match.get("asientos");
        std::cout << "🐥 asiento_s " << asiento_s->c_str() << "\n\n";
        int asiento = std::stoi(asiento_s->c_str());
        std::cout << "sala: " << sala << " horario: " << horario << " asiento: " << asiento << '\n';
        data["salas"][sala]["horarios"][horario]["asientos"][asiento]["estado"] = "💋query params";
//        std::cout << new_data.dump().c_str();
        std::cout << data.dump().c_str();
    }

    return 0;
}
