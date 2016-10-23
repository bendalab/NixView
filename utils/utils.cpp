#include "utils.hpp"

namespace nixview {
namespace util {

void remove_duplicates(QStringList &duplicate_list) {
    std::vector<QString> vec;
    for (QString s : duplicate_list) {
        vec.push_back(s);
    }
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique( vec.begin(), vec.end() ), vec.end());
    std::vector<int> removes;
    for (int i = 0; i < duplicate_list.size() ; i++){
        std::vector<QString>::iterator it = std::find(vec.begin(), vec.end(), duplicate_list[i]);
        if (it != vec.end()) {
            vec.erase(it);
        } else {
            removes.push_back(i);
        }
    }
    for (int i = (removes.size()-1); i >=0; i--)
        duplicate_list.removeAt(removes[i]);
}


std::string toLowerCase(const std::string &str) {
    std::string b;
    for (size_t i = 0; i < str.length(); i++) {
        b.push_back(std::tolower(str[i]));
    }
    return b;
}


bool stringCompare(const std::string &a, const std::string &b, bool case_sensitive, bool exact) {
    std::string str_a, str_b;

    if (!case_sensitive) {
        str_a = toLowerCase(a);
        str_b = toLowerCase(b);
    } else {
        str_a = a;
        str_b = b;
    }
    if (exact) {
        return str_a == str_b;
    } else {
        return str_a.find(str_b) != std::string::npos;
    }
}

} // util
} // nixview
