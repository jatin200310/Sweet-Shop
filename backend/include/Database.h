   #ifndef SWEET_SHOP_DATABASE_H
#define SWEET_SHOP_DATABASE_H

#include <mysql.h>

#include <string>
#include <vector>
#include <map>

class Database {
public:
    Database(const std::string& host,
             const std::string& user,
             const std::string& password,
             const std::string& dbName,
             unsigned int port = 3306);
    ~Database();

    bool connect();
    void disconnect();
    bool isConnected() const;

    // User operations
    bool createUser(const std::string& username,
                    const std::string& passwordHash,
                    const std::string& email,
                    bool isAdmin = false);
    std::map<std::string, std::string> getUserByUsername(const std::string& username);

    // Sweet operations
    bool createSweet(const std::string& name,
                     const std::string& description,
                     const std::string& category,
                     double price,
                     int quantity);
    std::vector<std::map<std::string, std::string>> getAllSweets();
    std::map<std::string, std::string> getSweetById(int id);
    bool updateSweet(int id,
                     const std::string& name,
                     const std::string& description,
                     const std::string& category,
                     double price,
                     int quantity);
    bool deleteSweet(int id);

    // Purchases
    bool purchaseSweet(int userId, int sweetId, int quantity, double& outTotal);
    bool restockSweet(int sweetId, int quantity);

    // Utility
    std::string escape(const std::string& input);

private:
    MYSQL* conn_;
    std::string host_;
    std::string user_;
    std::string password_;
    std::string database_;
    unsigned int port_;

    // non-copyable
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
};

#endif // SWEET_SHOP_DATABASE_H