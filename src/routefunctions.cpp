#include "routefunctions.h"

#include <regex>

#include "nlohmann/json.hpp"
#include "stockdbhelper.h"
#include "utility.h"

using json = nlohmann::json;

const std::regex pattern("^\\d+$");

int32_t GetMaxQuantity(sqlite3* db, const struct mg_str& input,
                       std::string& output) {
  int32_t first = 50;
  std::string date = R"(20240201)";
  StockDataList_T quantities;
  const std::string str(input.ptr, input.len);
  if (json::accept(str)) {
    const auto json_obj = json::parse(str);
    if (json_obj.count("first") == 1) {
      const auto first_str = json_obj["first"].template get<std::string>();
      if (std::regex_match(first_str, pattern)) first = stoi(first_str);
    }
    if (json_obj.count("date") == 1) {
      date.assign(json_obj["date"].template get<std::string>());
    }
  }
  int32_t rc = GetLatestValue(db, date, quantities);
  if (rc) {
    output.assign("get stock data from db failed: ");
    output.append(std::to_string(rc));
    return 500;
  }
  SortByQuantity(quantities, false);
  json result;
  for (auto q = quantities.begin(); q < quantities.begin() + first; ++q) {
    json j_umap(*q);
    result.emplace_back(j_umap);
  }
  output.assign(result.dump());
  return 200;
}
