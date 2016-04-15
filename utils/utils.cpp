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

} // util
} // nixview
