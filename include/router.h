#ifndef HTTP_ROUTER_OHk5uTNNR2Motyp7
#define HTTP_ROUTER_OHk5uTNNR2Motyp7

#include <regex>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>
#include <optional>

namespace route {

    class Match;
    class Route;

    struct Path {
        std::regex re;
        std::vector<std::string> keys;
    };

    class Route {

        friend class Match;

    private:
        std::string url;
        std::regex path_to_regex;

        //
        // TODO
        // Support a more sophisticated matching pattern with something like
        // https://github.com/pillarjs/path-to-regexp/blob/master/index.js#L11
        //

        //
        // a pattern used to replace the url template's "captures".
        //
        const std::string capture_pattern = "(?:([^\\/]+?))";

        //
        // a pattern for finding "captures" in a url template.
        //
        const std::string path_pattern = ":([^\\/]+)?";

        //
        // cache each regex and its keys as it is created by the
        // test method.
        //

        std::map<std::string, Path> cache;

    public:

        //
        // set the url for this instance, create a regex
        // from the url and return a match instance.
        //
        Match set(const std::string& u);
    };

    class Match {

    private:
        Route *route;
        std::map<std::string, std::string> pairs;

    public:
        int keys;
        std::optional<std::string> get(const std::string& key);
        bool test(const std::string& tmpl);

        Match(Route &r) : route(&r) {}
    };

}

#endif /* HTTP_ROUTER_OHk5uTNNR2Motyp7 */