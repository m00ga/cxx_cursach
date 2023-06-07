#include "linkedList.hpp"
#include "table.hpp"
#include "utils.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

void clearScreen() {
#ifdef _WIN32
    system("cls");
#elif defined __LINUX__
    system("clear");
#elif defined __APPLE__
    system("clear");
#endif
}

inline void pressEnter() {
    std::cout << "Press Enter to continue...";
    getchar();
}

struct Product {
  public:
    std::string name;
    std::string manufacturer;
    int article;
    float weight;
    std::string category;
    float availability;
    float sellPrice;
    float buyPrice;
    std::time_t expirationTime;
    struct NameComp {
        constexpr NameComp() {}

        constexpr bool operator()(const Product &c1,
                                  const std::string &c) const {
            return c1.name == c;
        }
        constexpr bool operator()(const std::string &c,
                                  const Product &c1) const {
            return c == c1.name;
        }
    };
    struct ArticleComp {
        constexpr bool operator()(const Product &c1, int c) const {
            return c1.article == c;
        }
        constexpr bool operator()(int c, const Product &c1) const {
            return c == c1.article;
        }
    };

    Product()
        : article(0), weight(0), availability(0), buyPrice(0), sellPrice(0),
          expirationTime(0) {}
};

struct Revenue {
  public:
    std::string name;
    unsigned int article;
    float weightBuyed;
    float revenue;
    struct NameComp {
        constexpr NameComp() {}

        constexpr bool operator()(const Revenue &c1,
                                  const std::string &c) const {
            return c1.name == c;
        }
        constexpr bool operator()(const std::string &c,
                                  const Revenue &c1) const {
            return c == c1.name;
        }
    };
    struct WeightSort {
        constexpr bool operator()(const Revenue &lhs,
                                  const Revenue &rhs) const {
            return lhs.weightBuyed > rhs.weightBuyed;
        }
    };
    struct RevenueSort {
        constexpr bool operator()(const Revenue &lhs,
                                  const Revenue &rhs) const {
            return lhs.revenue > rhs.revenue;
        }
    };
};

template <typename T> class ElemTable {
  private:
    std::fstream fileHandle;
    ezlib::LinkedList<T> elements;
    std::string tableName;
    int length;
    bool isWrite;
    bool isInitted;

  public:
    ElemTable(const std::string &tableFile) {
        isWrite = false;
        tableName = tableFile;
        length = 0;
        fileHandle.open(tableName,
                        std::ios::in | std::ios::binary | std::ios::app);
        if (fileHandle) {
            fileHandle.seekg(0, std::ios::end);
            length = (fileHandle.tellg() / sizeof(T));
            T elem{};
            for (int i = 0; i < length; ++i) {
                fileHandle.seekg(i * sizeof(T), std::ios::beg);
                fileHandle.read(reinterpret_cast<char *>(&elem), sizeof(T));
                elements.push_back(elem);
            }
        }
    }

    ~ElemTable() {
        fileHandle.close();
        fileHandle.open(tableName,
                        std::ios::out | std::ios::binary | std::ios::trunc);
        int i = 0;
        for (ezlib::Iterator<T> it = elements.begin(); it != elements.end();
             ++it) {
            fileHandle.seekg(i * sizeof(T), std::ios::beg);
            fileHandle.write(reinterpret_cast<char *>(&(*it)), sizeof(T));
            i++;
        }
    }

    template <typename Compare, typename K> T &getRow(const K &key) {
        return *elements.find_if_linear(key, Compare{});
    }

    ezlib::LinkedList<T> &getElements() { return elements; }
    template <typename Compare, typename K>
    void addOrUpdate(const T &row, const K &key) {
        try {
            T &prod = getRow<Compare>(key);
            *prod = row;
        } catch (const std::runtime_error &e) {
            addRow(row);
        }
    }

    void addRow(const T &row) {
        if (!isWrite) {
            isWrite = true;
        }
        length++;
        elements.push_back(row);
    }

    template <typename Compare, typename K> void removeRow(const K &key) {
        if (!isWrite) {
            isWrite = true;
        }
        elements.template remove<Compare>(key, Compare{});
        length--;
    }

    int getLength() { return length; }
};

void addHeader(ezlib::Table *tab) {
    tab->addRow({
        "Name",
        "Manufactorer",
        "Article",
        "Weight",
        "Category",
        "Availability",
        "Price for sell",
        "Buy price",
        "Expiration time",
    });
}
std::string makeStringTime(const std::time_t &t) {
    if (t == 0) {
        return "";
    }
    std::time_t t_now = std::time(nullptr);
    std::time_t razn = t - t_now;
    if (razn == 0) {
        return "Expired";
    }
    int hours = razn / 3600;
    int days = hours / 24;
    razn -= hours * 3600;
    hours -= days * 24;
    int minutes = razn / 60;
    razn -= minutes * 60;
    int seconds = razn;
    std::ostringstream os;
    os << "d " << days << " " << hours << ":" << minutes << ":" << seconds;
    return os.str();
}

void addProduct(ezlib::Table *tab, const Product &prod) {
    tab->addRow({prod.name, prod.manufacturer, std::to_string(prod.article),
                 std::to_string(prod.weight), prod.category,
                 std::to_string(prod.availability),
                 std::to_string(prod.sellPrice), std::to_string(prod.buyPrice),
                 makeStringTime(prod.expirationTime)});
}

bool fillProduct(Product *prod, ezlib::Table &tab) {
    ezlib::Row &prodTable = tab.getRows()[1];
    int exp = 0;
    while (true) {
        clearScreen();
        tab.print();
        std::cout << "1. Name\n2. Manufactorer\n3. Article\n4. "
                     "Weight\n5. Category\n6. Availability\n7. Sell "
                     "price\n8. Buy price\n9. Expiration time\n10. "
                     "Save\n0. Exit\n"
                  << std::endl;
        int i = ezlib::input<int>("Choice: ");
        if (i == 1) {
            prod->name = ezlib::input<std::string>("Enter new name: ");
            prodTable[0] = prod->name;
        } else if (i == 2) {
            prod->manufacturer =
                ezlib::input<std::string>("Enter new manufactorer: ");
            prodTable[1] = prod->manufacturer;
        } else if (i == 3) {
            prod->article = ezlib::input<int>("Enter new article: ");
            prodTable[2] = std::to_string(prod->article);
        } else if (i == 4) {
            prod->weight = ezlib::input<float>("Enter new weight: ");
            prodTable[3] = std::to_string(prod->weight);
        } else if (i == 5) {
            prod->category = ezlib::input<std::string>("Enter new category: ");
            prodTable[4] = prod->category;
        } else if (i == 6) {
            prod->availability =
                ezlib::input<float>("Enter new availability: ");
            prodTable[5] = std::to_string(prod->availability);
        } else if (i == 7) {
            prod->sellPrice = ezlib::input<float>("Enter new sell price: ");
            prodTable[6] = std::to_string(prod->sellPrice);
        } else if (i == 8) {
            prod->buyPrice = ezlib::input<float>("Enter new buy price: ");
            prodTable[7] = std::to_string(prod->buyPrice);
        } else if (i == 9) {
            exp = ezlib::input<int>("Enter new expiration time(in days): ");
            exp = std::abs(exp);
            prodTable[8] = std::to_string(exp);
        } else if (i == 10) {
            prod->expirationTime = std::time(nullptr) + (exp * 3600 * 24);
            return true;
        } else if (i == 0) {
            return false;
        } else {

            std::cout << "Wrong selection!";
            pressEnter();
        }
        tab.recalc();
    }
}

template <typename T> void getProduct(Product **prod, ElemTable<T> &table) {
    while (true) {
        clearScreen();
        std::cout << "Type 0 for exit" << std::endl;
        std::cout << "1. Search by name\t2. Search by article\t0. Exit\n";
        int inp = ezlib::input<int>("Choice: ");
        if (inp == 0) {
            break;
        } else if (inp > 2 || inp < 0) {
            std::cout << "Wrong selection!\n";
            pressEnter();
        }
        try {
            if (inp == 1) {
                std::string name =
                    ezlib::input<std::string>("Enter name of product: ");
                *prod = &table.template getRow<Product::NameComp>(name);
            } else if (inp == 2) {
                int article = ezlib::input<int>("Enter article of product: ");
                *prod = &table.template getRow<Product::ArticleComp>(article);
            }
            break;
        } catch (const std::runtime_error &e) {
            std::cout << "That product doesn't exists" << std::endl;
            pressEnter();
        }
    }
}

int main() {
    ElemTable<Product> productsTable("products.txt");
    ElemTable<Revenue> revenueTable("revenue.txt");
    ezlib::Table tab('-', '|', '+');
    while (true) {
        tab.clear();
        clearScreen();
        std::cout << "!---- Shop control panel ----!" << std::endl;
        std::cout << "!#    Products in table: " << productsTable.getLength()
                  << "    #!" << std::endl;
        std::cout << "1. Show all products in table" << std::endl;
        std::cout << "2. Add new product" << std::endl;
        std::cout << "3. Edit existing product" << std::endl;
        std::cout << "4. Remove existing product" << std::endl;
        std::cout << "5. Sell existing product" << std::endl;
        std::cout << "6. Show revenue" << std::endl;
        std::cout << "0. Exit" << std::endl;
        int choice = ezlib::input<int>("Choice: ");
        if (choice == 1) {
            clearScreen();
            auto &ll = productsTable.getElements();
            addHeader(&tab);
            for (ezlib::Iterator<Product> it = ll.begin(); it != ll.end();
                 ++it) {
                addProduct(&tab, *it);
            }
            tab.print();
            pressEnter();
        } else if (choice == 2) {
            clearScreen();
            Product prod{};
            addHeader(&tab);
            addProduct(&tab, prod);
            if (fillProduct(&prod, tab)) {
                productsTable.addRow(prod);
            }
        } else if (choice == 3) {
            Product *prod = nullptr;
            getProduct(&prod, productsTable);
            if (prod != nullptr) {
                Product newProd = *prod;
                addHeader(&tab);
                addProduct(&tab, *prod);
                if (fillProduct(&newProd, tab)) {
                    *prod = newProd;
                }
            }
        } else if (choice == 4) {
            Product *prod = nullptr;
            getProduct(&prod, productsTable);
            if (prod != nullptr) {
                addHeader(&tab);
                addProduct(&tab, *prod);
                while (true) {
                    clearScreen();
                    tab.print();
                    std::string inp =
                        ezlib::input<std::string>("Delete this row (Y/n): ");
                    std::transform(
                        inp.begin(), inp.end(), inp.begin(),
                        [](unsigned char c) { return std::tolower(c); });
                    if (inp == "y" || inp == "") {
                        productsTable.removeRow<Product::NameComp>(prod->name);
                        break;
                    } else if (inp == "n") {
                        break;
                    } else {
                        std::cout << "Wrong input!" << std::endl;
                        pressEnter();
                    }
                }
            }
        } else if (choice == 5) {
            Product *prod = nullptr;
            getProduct(&prod, productsTable);
            if (prod != nullptr) {
                addHeader(&tab);
                addProduct(&tab, *prod);
                while (true) {
                    clearScreen();
                    tab.print();
                    std::cout << "0 to exit" << std::endl;
                    float weight =
                        ezlib::input<float>("Enter weight to sell(in kg): ");
                    if (weight == 0.0) {
                        break;
                    }
                    if (weight < 0.0) {
                        std::cout << "Weight must be positive number"
                                  << std::endl;
                        pressEnter();
                        continue;
                    }
                    if (weight > prod->availability) {
                        std::cout << "There is more than we have" << std::endl;
                        pressEnter();
                        continue;
                    }
                    float price = weight * prod->sellPrice;
                    bool flag = false;
                    while (true) {
                        std::cout << "0 to reenter weight" << std::endl;
                        std::cout << "Price: " << price << std::endl;
                        float payed =
                            ezlib::input<float>("How much client payed: ");
                        if (payed < 0.0 || price > payed) {
                            std::cout << "Wrong input" << std::endl;
                            continue;
                        }
                        if (payed == 0.0) {
                            break;
                        }
                        Revenue rev{};
                        try {
                            Revenue &revP =
                                revenueTable.getRow<Revenue::NameComp>(
                                    prod->name);
                            revP.revenue += price;
                            revP.weightBuyed += weight;
                        } catch (const std::runtime_error &e) {
                            rev.name = prod->name;
                            rev.article = prod->article;
                            rev.weightBuyed = weight;
                            rev.revenue = price;
                            revenueTable.addRow(rev);
                        }
                        prod->availability -= weight;
                        std::cout << "Change to give: " << payed - price
                                  << std::endl;
                        pressEnter();
                        flag = true;
                        break;
                    }
                    if (flag) {
                        break;
                    }
                }
            }
        } else if (choice == 6) {
            ezlib::Table revTable('-', '|', '+');
            while (true) {
                revTable.clear();
                clearScreen();
                revTable.addRow({"Name", "Article", "Weight buyed", "Revenue"});
                auto &ll = revenueTable.getElements();
                for (ezlib::Iterator<Revenue> it = ll.begin(); it != ll.end();
                     ++it) {
                    revTable.addRow({it->name, std::to_string(it->article),
                                     std::to_string(it->weightBuyed),
                                     std::to_string(it->revenue)});
                }
                revTable.print();
                std::cout << "1. Sort by weight\t2. Sort by revenue\t0. Exit\n";
                int inp = ezlib::input<int>("Choice: ");
                if (inp == 1) {
                    ll.sort(Revenue::WeightSort{});
                } else if (inp == 2) {
                    ll.sort(Revenue::RevenueSort{});
                } else if (inp == 0) {
                    break;
                } else {
                    std::cout << "Wrong selection!" << std::endl;
                    pressEnter();
                }
            }
        } else if (choice == 0) {
            break;
        } else {
            std::cout << "Wrong selection" << std::endl;
            pressEnter();
        }
    }
    return 0;
}
