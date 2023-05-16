#include <iostream>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <iomanip>
#include "router.h"

using json = nlohmann::json;
using namespace route;

int main() {
    std::cout << "Hello, World!" << std::endl;
    fmt::print(fmt::runtime("The answer is {}."), 42);

    json j =
    {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {
            "answer", {
                {"everything", 42}
            }
        },
        {"list", {1, 0, 2}},
        {
            "object", {
                {"currency", "USD"},
                {"value", 42.99}
            }
        }
    };

    // add new values
    j["new"]["key"]["value"] = {"another", "list"};

    // count elements
    auto s = j.size();
    j["size"] = s;

    // pretty print with indent of 4 spaces
    std::cout << std::setw(4) << j << '\n';
    std::string res = j.dump();

    printf("%s\n\n\n\n", res.c_str());
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
    

    return 0;
}
