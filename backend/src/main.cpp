#include <crow.h>
#include <iostream>

int main() {
    crow::SimpleApp app;

    // Root route
    CROW_ROUTE(app, "/")([]() {
        crow::response res(200, "Sweet Shop API is running");
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    // Preflight (CORS)
    CROW_ROUTE(app, "/<path>")
        .methods("OPTIONS"_method)
    ([](const crow::request&, std::string) {
        crow::response res(204);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
        return res;
    });

    // Get sweets
    CROW_ROUTE(app, "/api/sweets")
        .methods("GET"_method)
    ([]() {
        crow::response res(200, "[]");
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    // Register user
    CROW_ROUTE(app, "/api/auth/register")
        .methods("POST"_method)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, "Invalid JSON");
        }

        crow::json::wvalue resBody;
        resBody["message"] = "registered";
        resBody["username"] = body["username"].s();

        crow::response res(201, resBody);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    std::cout << "Server running on http://localhost:8080\n";
    app.port(8080).multithreaded().run();

    return 0;
}
