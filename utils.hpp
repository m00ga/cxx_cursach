#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace ezlib {
std::vector<std::string> splitString(const std::string &str, const char delim) {
    std::vector<std::string> res;
    std::string word;
    for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
        if (*it == delim) {
            res.push_back(word);
            word.clear();
        } else {
            word += *it;
        }
    }
    return res;
}

template <typename T> T from_string(const std::string &str);

template <> int from_string(const std::string &str) {
    std::size_t pos;
    int ret;
    try {
        ret = std::stoi(str, &pos);
        if (str.size() > pos) {
            throw std::invalid_argument("Wrong type");
        }
    } catch (const std::invalid_argument &e) {
        throw e;
    }
    return ret;
}
template <> float from_string(const std::string &str) { return std::stof(str); }
template <> std::string from_string(const std::string &str) { return str; }

template <typename T> std::string makeFallback();

template <> std::string makeFallback<float>() { return "float"; }
template <> std::string makeFallback<int>() { return "integer"; }
template <> std::string makeFallback<std::string>() { return "string"; }

template <typename T> T input(const std::string &prompt) {
    T ret;
    std::string data;
    while (true) {
        std::cout << prompt;
        std::cin >> data;
        try {
            ret = from_string<T>(data);
            break;
        } catch (const std::invalid_argument &e) {
            std::cout << "Error: you must enter a " << makeFallback<T>()
                      << " type!" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return ret;
}

} // namespace ezlib
