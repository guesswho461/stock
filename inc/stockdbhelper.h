#ifndef _STOCKDBHELPER_H_
#define _STOCKDBHELPER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

typedef void (*progress_callback)(double);

typedef std::unordered_map<std::string, std::string> StockData_T;
typedef std::unordered_map<std::string, StockData_T> StockDataTable_T;
typedef std::vector<StockData_T> StockDataList_T;

void SaveToDB(sqlite3 *db, const StockDataTable_T &dataTable,
              progress_callback callback);
int32_t GetLatestValue(sqlite3 *db, const std::string &date,
                       StockDataList_T &datas);

#endif