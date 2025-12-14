#ifndef SWEET_SHOP_SWEET_H
#define SWEET_SHOP_SWEET_H

#include <string>
#include <vector>
#include <map>

class Database; // forward declaration

struct Sweet {
    int id{0};
    std::string name;
    std::string description;
    std::string category;
    double price{0.0};
    int quantity{0};
};

class SweetManager {
public:
    explicit SweetManager(Database& db);

    std::vector<Sweet> getAllSweets();
    Sweet getSweetById(int id);
    bool addSweet(const std::string& name,
                  const std::string& description,
                  const std::string& category,
                  double price,
                  int quantity);
    bool updateSweet(int id,
                     const std::string& name,
                     const std::string& description,
                     const std::string& category,
                     double price,
                     int quantity);
    bool deleteSweet(int id);

    // purchase returns total price on success via outTotal, false on failure
    bool purchaseSweet(int userId, int sweetId, int quantity, double& outTotal);
    bool restockSweet(int sweetId, int quantity);

private:
    Database& db_;

    // non-copyable
    SweetManager(const SweetManager&) = delete;
    SweetManager& operator=(const SweetManager&) = delete;
};

#endif // SWEET_SHOP_SWEET_H