#include "Database.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstring>

Database::Database(const std::string& host,
                   const std::string& user,
                   const std::string& password,
                   const std::string& dbName,
                   unsigned int port)
    : conn_(nullptr),
      host_(host),
      user_(user),
      password_(password),
      database_(dbName),
      port_(port) {}

Database::~Database() {
    disconnect();
}

bool Database::connect() {
    if (conn_) return true;
    conn_ = mysql_init(nullptr);
    if (!conn_) {
        std::cerr << "mysql_init failed\n";
        return false;
    }
    if (!mysql_real_connect(conn_, host_.c_str(), user_.c_str(),
                            password_.c_str(), database_.c_str(),
                            port_, nullptr, 0)) {
        std::cerr << "mysql_real_connect error: " << mysql_error(conn_) << "\n";
        mysql_close(conn_);
        conn_ = nullptr;
        return false;
    }
    return true;
}

void Database::disconnect() {
    if (conn_) {
        mysql_close(conn_);
        conn_ = nullptr;
    }
}

bool Database::isConnected() const {
    return conn_ != nullptr;
}

std::string Database::escape(const std::string& input) {
    if (!conn_) return input;
    std::string out;
    out.resize(input.size() * 2 + 1);
    unsigned long len = mysql_real_escape_string(conn_,
                                                 &out[0],
                                                 input.c_str(),
                                                 static_cast<unsigned long>(input.size()));
    out.resize(len);
    return out;
}

bool Database::createUser(const std::string& username,
                          const std::string& passwordHash,
                          const std::string& email,
                          bool isAdmin) {
    if (!connect()) return false;
    std::string u = escape(username);
    std::string p = escape(passwordHash);
    std::string e = escape(email);
    std::ostringstream q;
    q << "INSERT INTO users (username, password_hash, email, is_admin) VALUES ('"
      << u << "','" << p << "','" << e << "'," << (isAdmin ? 1 : 0) << ")";
    if (mysql_query(conn_, q.str().c_str()) != 0) {
        // Duplicate or other error
        return false;
    }
    return true;
}

std::map<std::string, std::string> Database::getUserByUsername(const std::string& username) {
    std::map<std::string, std::string> result;
    if (!connect()) return result;
    std::string u = escape(username);
    std::ostringstream q;
    q << "SELECT id,username,password_hash,email,is_admin,created_at FROM users WHERE username='"
      << u << "' LIMIT 1";
    if (mysql_query(conn_, q.str().c_str()) != 0) return result;
    MYSQL_RES* res = mysql_store_result(conn_);
    if (!res) return result;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return result;
    }
    MYSQL_FIELD* fields = mysql_fetch_fields(res);
    unsigned int num_fields = mysql_num_fields(res);
    for (unsigned int i = 0; i < num_fields; ++i) {
        const char* val = row[i];
        result[fields[i].name] = val ? val : "";
    }
    mysql_free_result(res);
    return result;
}

bool Database::createSweet(const std::string& name,
                           const std::string& description,
                           const std::string& category,
                           double price,
                           int quantity) {
    if (!connect()) return false;
    std::string n = escape(name);
    std::string d = escape(description);
    std::string c = escape(category);
    std::ostringstream q;
    q << "INSERT INTO sweets (name, description, category, price, quantity) VALUES ('"
      << n << "','" << d << "','" << c << "'," << price << "," << quantity << ")";
    if (mysql_query(conn_, q.str().c_str()) != 0) {
        return false;
    }
    return true;
}

std::vector<std::map<std::string, std::string>> Database::getAllSweets() {
    std::vector<std::map<std::string, std::string>> out;
    if (!connect()) return out;
    const char* q = "SELECT id,name,description,category,price,quantity,created_at,updated_at FROM sweets";
    if (mysql_query(conn_, q) != 0) return out;
    MYSQL_RES* res = mysql_store_result(conn_);
    if (!res) return out;
    MYSQL_FIELD* fields = mysql_fetch_fields(res);
    unsigned int num_fields = mysql_num_fields(res);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        std::map<std::string, std::string> rowmap;
        for (unsigned int i = 0; i < num_fields; ++i) {
            rowmap[fields[i].name] = row[i] ? row[i] : "";
        }
        out.push_back(std::move(rowmap));
    }
    mysql_free_result(res);
    return out;
}

std::map<std::string, std::string> Database::getSweetById(int id) {
    std::map<std::string, std::string> result;
    if (!connect()) return result;
    std::ostringstream q;
    q << "SELECT id,name,description,category,price,quantity,created_at,updated_at FROM sweets WHERE id="
      << id << " LIMIT 1";
    if (mysql_query(conn_, q.str().c_str()) != 0) return result;
    MYSQL_RES* res = mysql_store_result(conn_);
    if (!res) return result;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return result;
    }
    MYSQL_FIELD* fields = mysql_fetch_fields(res);
    unsigned int num_fields = mysql_num_fields(res);
    for (unsigned int i = 0; i < num_fields; ++i) {
        result[fields[i].name] = row[i] ? row[i] : "";
    }
    mysql_free_result(res);
    return result;
}

bool Database::updateSweet(int id,
                           const std::string& name,
                           const std::string& description,
                           const std::string& category,
                           double price,
                           int quantity) {
    if (!connect()) return false;
    std::string n = escape(name);
    std::string d = escape(description);
    std::string c = escape(category);
    std::ostringstream q;
    q << "UPDATE sweets SET name='" << n << "', description='" << d
      << "', category='" << c << "', price=" << price
      << ", quantity=" << quantity << " WHERE id=" << id;
    if (mysql_query(conn_, q.str().c_str()) != 0) return false;
    return true;
}

bool Database::deleteSweet(int id) {
    if (!connect()) return false;
    std::ostringstream q;
    q << "DELETE FROM sweets WHERE id=" << id;
    if (mysql_query(conn_, q.str().c_str()) != 0) return false;
    return true;
}

bool Database::purchaseSweet(int userId, int sweetId, int quantity, double& outTotal) {
    outTotal = 0.0;
    if (!connect()) return false;
    // Start transaction
    if (mysql_query(conn_, "START TRANSACTION") != 0) {
        return false;
    }

    // Lock and read current quantity and price
    std::ostringstream q1;
    q1 << "SELECT quantity, price FROM sweets WHERE id=" << sweetId << " FOR UPDATE";
    if (mysql_query(conn_, q1.str().c_str()) != 0) {
        mysql_query(conn_, "ROLLBACK");
        return false;
    }
    MYSQL_RES* res = mysql_store_result(conn_);
    if (!res) { mysql_query(conn_, "ROLLBACK"); return false; }
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) { mysql_free_result(res); mysql_query(conn_, "ROLLBACK"); return false; }
    int curQty = row[0] ? std::stoi(row[0]) : 0;
    double price = row[1] ? std::stod(row[1]) : 0.0;
    mysql_free_result(res);

    if (curQty < quantity) {
        mysql_query(conn_, "ROLLBACK");
        return false;
    }

    int newQty = curQty - quantity;
    std::ostringstream q2;
    q2 << "UPDATE sweets SET quantity=" << newQty << " WHERE id=" << sweetId;
    if (mysql_query(conn_, q2.str().c_str()) != 0) {
        mysql_query(conn_, "ROLLBACK");
        return false;
    }

    double total = price * quantity;
    std::ostringstream q3;
    q3 << "INSERT INTO purchases (user_id, sweet_id, quantity, total_price) VALUES ("
       << userId << "," << sweetId << "," << quantity << "," << total << ")";
    if (mysql_query(conn_, q3.str().c_str()) != 0) {
        mysql_query(conn_, "ROLLBACK");
        return false;
    }

    if (mysql_query(conn_, "COMMIT") != 0) {
        mysql_query(conn_, "ROLLBACK");
        return false;
    }

    outTotal = total;
    return true;
}

bool Database::restockSweet(int sweetId, int quantity) {
    if (!connect()) return false;
    std::ostringstream q;
    q << "UPDATE sweets SET quantity = quantity + " << quantity << " WHERE id=" << sweetId;
    if (mysql_query(conn_, q.str().c_str()) != 0) return false;
    return true;
}