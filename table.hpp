#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ezlib {
using Row = std::vector<std::string>;
class Table {

  private:
    std::vector<Row> rows;
    std::vector<int> columnMax;
    int padding;
    int rowSize;
    char _horizontal;
    char _vertical;

    std::string repeat(int times, char ch) {
        std::string ret;
        for (; times > 0; times--) {
            ret += ch;
        }
        return ret;
    }
    char _corner;

    void printDelim() {
        std::stringstream ss;
        for (const int &max : columnMax) {
            ss << _corner << repeat(max + padding, _horizontal);
        }
        ss << _corner;
        std::cout << ss.str() << std::endl;
    }

  public:
    Table(char horizontal, char vertical, char corner)
        : _vertical(vertical), _horizontal(horizontal), _corner(corner) {
        rowSize = 0;
        padding = 2;
    }

    void clear() {
        rows.clear();
        rowSize = 0;
        columnMax.clear();
    }

    std::vector<Row> &getRows() { return rows; }

    void addRow(const Row &row) {
        if (rowSize == 0) {
            rowSize = row.size();
            columnMax.insert(columnMax.end(), rowSize, 0);
        }
        if (row.size() == rowSize) {
            for (int i = 0; i < rowSize; i++) {
                if (row[i].size() > columnMax[i]) {
                    columnMax[i] = row[i].size();
                }
            }
            rows.push_back(row);
        } else {
            throw std::runtime_error("Wrong columns count");
        }
    }

    void recalc() {
        for (std::size_t i = 0; i < rows.size(); ++i) {
            for (auto it = rows[i].begin(); it != rows[i].end(); ++it) {
                if (it->size() > columnMax[i]) {
                    columnMax[i] = it->size();
                }
            }
        }
    }

    void print() {
        for (std::vector<Row>::const_iterator rows_it = rows.begin();
             rows_it != rows.end(); rows_it++) {
            if (rows_it == rows.begin()) {
                printDelim();
            }
            for (int i = 0; i < rowSize; i++) {
                std::cout << _vertical;
                std::cout << std::setw(columnMax[i] + padding);
                std::cout << std::left << (*rows_it)[i];
                if (i == rowSize - 1) {
                    std::cout << _vertical;
                }
            }
            std::cout << std::endl;
            printDelim();
        }
    }
};
} // namespace ezlib
