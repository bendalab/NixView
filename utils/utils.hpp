#ifndef NIXVIEW_UTIL_HPP
#define NIXVIEW_UTIL_HPP

#include <iostream>
#include <vector>
#include <QStringList>

namespace nixview {
namespace util {

void remove_duplicates(QStringList &duplicate_list);


/**
 * @brief converts string toLowerCase string. Function ignores locale.
 *
 * @param str the string
 * @return the lower case string
 */
std::string toLowerCase(const std::string &str);


/**
 * @brief stringCompare: compares two strings.
 * @param a: the string in which we look for a match
 * @param b: the string that should match
 * @param case_sensitive: if true, upper or lower case are relevant, default: false
 * @param exact: defines whether the match must be exact or whether b may be a substring of a, default: false
 * @return bool
 */
bool stringCompare(const std::string &a, const std::string &b, bool case_sensitive=false, bool exact=false);

} //namespace util
} //namespace nixview


#endif // NIXVIEW_UTIL_HPP
