#include "utility.h"

#include <algorithm>

const std::vector<std::string> ParseDateStr(std::string date) {
  std::vector<std::string> dateVec;
  dateVec.push_back(date.substr(0, 4));
  dateVec.push_back(date.substr(4, 2));
  dateVec.push_back(date.substr(6, 2));
  return dateVec;
}

void SortByQuantity(
    std::vector<std::unordered_map<std::string, std::string>> &datas,
    bool isAscending) {
  std::sort(
      datas.begin(), datas.end(), [isAscending](const auto &x, const auto &y) {
        if (x.count("tseQUANTITY") <= 0) return isAscending;
        if (y.count("tseQUANTITY") <= 0) return !isAscending;
        std::string xx(x.at("tseQUANTITY").size(), '0');
        std::string yy(y.at("tseQUANTITY").size(), '0');
        xx.erase(std::remove_copy(x.at("tseQUANTITY").begin(),
                                  x.at("tseQUANTITY").end(), xx.begin(), ','),
                 xx.end());
        yy.erase(std::remove_copy(y.at("tseQUANTITY").begin(),
                                  y.at("tseQUANTITY").end(), yy.begin(), ','),
                 yy.end());
        double a = std::stod(xx);
        double b = std::stod(yy);
        return isAscending ? (a < b) : (a > b);
      });
}

const std::vector<std::string> Split(const std::string &str,
                                     const std::string &pattern) {
  std::vector<std::string> result;
  std::string::size_type begin, end;

  end = str.find(pattern);
  begin = 0;

  while (end != std::string::npos) {
    if (end - begin != 0) {
      result.push_back(str.substr(begin, end - begin));
    }
    begin = end + pattern.size();
    end = str.find(pattern, begin);
  }

  if (begin != str.length()) {
    result.push_back(str.substr(begin));
  }
  return result;
}

int32_t GetDaysFromDate(int32_t y, int32_t m, int32_t d) {
  m = (m + 9) % 12;
  y = y - m / 10;
  return 365 * y + y / 4 - y / 100 + y / 400 + (m * 306 + 5) / 10 + (d - 1);
}

const std::vector<int32_t> GetDateFromDays(int32_t g) {
  int32_t y = (10000 * g + 14780) / 3652425;
  int32_t ddd = g - (365 * y + y / 4 - y / 100 + y / 400);
  if (ddd < 0) {
    y = y - 1;
    ddd = g - (365 * y + y / 4 - y / 100 + y / 400);
  }
  int32_t mi = (100 * ddd + 52) / 3060;
  int32_t mm = (mi + 2) % 12 + 1;
  y = y + (mi + 2) / 12;
  int32_t dd = ddd - (mi * 306 + 5) / 10 + 1;
  return std::vector<int32_t>{y, mm, dd};
}

const std::vector<std::string> GetDates(const std::string &start,
                                        const std::string &end) {
  std::vector<std::string> dates;
  auto startDateStrs = ParseDateStr(start);
  auto endDateStrs = ParseDateStr(end);
  auto startDateDays =
      GetDaysFromDate(std::stoi(startDateStrs[0]), std::stoi(startDateStrs[1]),
                      std::stoi(startDateStrs[2]));
  auto endDateDays =
      GetDaysFromDate(std::stoi(endDateStrs[0]), std::stoi(endDateStrs[1]),
                      std::stoi(endDateStrs[2]));
  if (endDateDays > startDateDays) {
    for (auto i = startDateDays; i <= endDateDays; ++i) {
      std::string date;
      for (auto &s : GetDateFromDays(i)) {
        date.append(std::to_string(s));
      }
      dates.emplace_back(date);
    }
  }
  return dates;
}