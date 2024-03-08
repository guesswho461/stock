#define ERROR -1

#define TW_STOCK_ALL_LATEST_DAILY_URL \
  "https://www.twse.com.tw/rwd/zh/afterTrading/MI_INDEX?type=ALL"

#define TW_STOCK_LATEST_DAILY_JSON_PATH_DATAS "/tables/8/data"
#define TW_STOCK_LATEST_DAILY_JSON_PATH_FIELD "/tables/8/fields"
#define TW_STOCK_LATEST_DAILY_JSON_PATH_DATE "/date"

#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "curl/curl.h"
#include "curl/easy.h"
#include "gflags/gflags.h"
#include "json-c/json.h"
#include "sqlite3.h"
#include "stockdbhelper.h"
#include "utility.h"

DEFINE_string(db, "test.db", "stock db file");
DEFINE_string(date, "latest", "the target date to get, eg: 20240131");

std::unordered_map<std::string, std::string> fieldMappingTable = {
    {"證券代號", "tseINDEX"},    {"證券名稱", "tseNAME"},
    {"成交金額", "tseQUANTITY"}, {"開盤價", "tseOPEN"},
    {"最高價", "tseHIGHEST"},    {"最低價", "tseLOWEST"},
    {"收盤價", "tseCLOSE"}};

size_t writeFunctionCallBack(void *ptr, size_t size, size_t nmemb,
                             void *stream) {
  std::string data((const char *)ptr, (size_t)size * nmemb);
  *((std::stringstream *)stream) << data << std::endl;
  return size * nmemb;
}

int32_t WebApiGet(CURL *curl, const std::string &url, std::string &out) {
  std::stringstream str;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunctionCallBack);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
  CURLcode res = curl_easy_perform(curl);
  if (res == 0) {
    out.assign(str.str());
  }
  return res;
}

void GetValuesOfArray(json_object *obj, std::vector<std::string> &values) {
  int32_t res = 0;
  json_object *valueObj;
  int32_t len = json_object_array_length(obj);
  for (int32_t i = 0; i < len; ++i) {
    std::string valuePath = "/";
    valuePath.append(std::to_string(i));
    res = json_pointer_get(obj, valuePath.c_str(), &valueObj);
    if (res != 0) continue;
    values.push_back(json_object_get_string(valueObj));
  }
}

int32_t ParseStockData(const std::string jsonStr, StockDataTable_T &dataTable) {
  int32_t res = 0;
  std::string fullDate;
  std::vector<std::string> fields;
  json_object *dateObj, *datasObj, *fieldObj, *dataObj, *valueObj;
  json_object *jsonObj = json_tokener_parse(jsonStr.c_str());
  res = json_pointer_get(jsonObj, TW_STOCK_LATEST_DAILY_JSON_PATH_FIELD,
                         &fieldObj);
  if (res != 0) {
    json_object_put(jsonObj);
    return ERROR;
  }
  res = json_pointer_get(jsonObj, TW_STOCK_LATEST_DAILY_JSON_PATH_DATAS,
                         &datasObj);
  if (res != 0) {
    json_object_put(jsonObj);
    return ERROR;
  }
  res =
      json_pointer_get(jsonObj, TW_STOCK_LATEST_DAILY_JSON_PATH_DATE, &dateObj);
  if (res != 0) {
    json_object_put(jsonObj);
    return ERROR;
  }
  fullDate.assign(json_object_get_string(dateObj));
  auto date = ParseDateStr(fullDate);
  GetValuesOfArray(fieldObj, fields);
  const int32_t dataCnt = json_object_array_length(datasObj);
  const int32_t fieldCnt = fields.size();
  for (int32_t i = 0; i < dataCnt; ++i) {
    std::string dataPath = "/";
    dataPath.append(std::to_string(i));
    res = json_pointer_get(datasObj, dataPath.c_str(), &dataObj);
    if (res != 0) continue;
    std::vector<std::string> values;
    GetValuesOfArray(dataObj, values);
    StockData_T data;
    for (int32_t j = 0; j < fieldCnt; ++j) {
      if (fieldMappingTable.count(fields[j]) <= 0) continue;
      data[fieldMappingTable[fields[j]]] = values[j];
    }
    data["tseYEAR"] = date[0];
    data["tseMONTH"] = date[1];
    data["tseDAY"] = date[2];
    data["tseDATE"] = fullDate;
    std::string tableName = "tse";
    tableName.append(data["tseINDEX"]);
    dataTable[tableName] = data;
  }
  json_object_put(jsonObj);
  return 0;
}

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  int32_t rc = 0;
  CURL *curl = NULL;
  sqlite3 *db = NULL;
  std::string url = TW_STOCK_ALL_LATEST_DAILY_URL;
  StockDataTable_T stockDataTable;
  std::string jsonStr;

  std::cout << "to get " << FLAGS_date
            << " tw stock data from server and save to db" << std::endl;

  if (FLAGS_date != "latest") {
    url.append("&date=");
    url.append(FLAGS_date);
  }

  rc = sqlite3_open(FLAGS_db.c_str(), &db);
  if (rc) {
    std::cout << "db open failed (" << rc << ")" << std::endl;
    return ERROR;
  }

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();

  if (!curl) {
    std::cout << "curl open failed" << std::endl;
    sqlite3_close(db);
    return ERROR;
  }

  rc = WebApiGet(curl, url, jsonStr);
  if (rc) {
    std::cout << "to get tw stock data from server failed (" << rc << ")"
              << std::endl;
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    sqlite3_close(db);
    return ERROR;
  }
  jsonStr.erase(std::remove(jsonStr.begin(), jsonStr.end(), '\n'),
                jsonStr.cend());
  rc = ParseStockData(jsonStr, stockDataTable);
  if (rc) {
    std::cout << "to parse the tw stock data failed (" << rc << ")"
              << std::endl;
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    sqlite3_close(db);
    return ERROR;
  }

  std::cout << "saving to db..." << std::endl;
  SaveToDB(db, stockDataTable, NULL);

  curl_easy_cleanup(curl);
  curl_global_cleanup();
  sqlite3_close(db);

  std::cout << "done" << std::endl;
  return rc;
}
