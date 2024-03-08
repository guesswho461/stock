#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

const std::vector<std::string> ParseDateStr(std::string date);
void SortByQuantity(
    std::vector<std::unordered_map<std::string, std::string>> &datas,
    bool isAscending);
const std::vector<std::string> Split(const std::string &str,
                                     const std::string &pattern);
const std::vector<std::string> GetDates(const std::string &start,
                                        const std::string &end);

#endif