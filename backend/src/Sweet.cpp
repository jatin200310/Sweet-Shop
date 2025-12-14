#include "Sweet.h"
#include "Database.h"
#include <iostream>
#include <sstream>

SweetManager::SweetManager(Database& db)
    : db_(db) {}

std::vector<Sweet> SweetManager::getAllSweets() {
    std::vector<Sweet> sweets;
    auto rows = db_.getAllSweets();
    for (const auto& row : rows) {
        Sweet s;
        if (row.count("id")) s.id = std::stoi(row.at("id"));
        if (row.count("name")) s.name = row.at("name");
        if (row.count("description")) s.description = row.at("description");
        if (row.count("category")) s.category = row.at("category");
        if (row.count("price")) s.price = std::stod(row.at("price"));
        if (row.count("quantity")) s.quantity = std::stoi(row.at("quantity"));
        sweets.push_back(s);
    }
    return sweets;
}

Sweet SweetManager::getSweetById(int id) {
    Sweet s;
    auto row = db_.getSweetById(id);
    if (row.empty()) return s;
    if (row.count("id")) s.id = std::stoi(row.at("id"));
    if (row.count("name")) s.name = row.at("name");
    if (row.count("description")) s.description = row.at("description");
    if (row.count("category")) s.category = row.at("category");
    if (row.count("price")) s.price = std::stod(row.at("price"));
    if (row.count("quantity")) s.quantity = std::stoi(row.at("quantity"));
    return s;
}

bool SweetManager::addSweet(const std::string& name,
                            const std::string& description,
                            const std::string& category,
                            double price,
                            int quantity) {
    if (name.empty() || category.empty() || price < 0 || quantity < 0) {
        return false;
    }
    return db_.createSweet(name, description, category, price, quantity);
}

bool SweetManager::updateSweet(int id,
                               const std::string& name,
                               const std::string& description,
                               const std::string& category,
                               double price,
                               int quantity) {
    if (id <= 0 || name.empty() || category.empty() || price < 0 || quantity < 0) {
        return false;
    }
    return db_.updateSweet(id, name, description, category, price, quantity);
}

bool SweetManager::deleteSweet(int id) {
    if (id <= 0) return false;
    return db_.deleteSweet(id);
}

bool SweetManager::purchaseSweet(int userId, int sweetId, int quantity, double& outTotal) {
    if (userId <= 0 || sweetId <= 0 || quantity <= 0) {
        return false;
    }
    return db_.purchaseSweet(userId, sweetId, quantity, outTotal);
}

bool SweetManager::restockSweet(int sweetId, int quantity) {
    if (sweetId <= 0 || quantity <= 0) {
        return false;
    }
    return db_.restockSweet(sweetId, quantity);
}