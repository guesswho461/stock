#include "stockdbhelper.h"

#include <algorithm>
#include <iostream>

#include "utility.h"

int32_t CreateTable(sqlite3 *db, std::string tableName, StockData_T data) {
  int32_t rc = 0;
  char *zErrMsg = 0;
  std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName + " ( ";
  for (auto itr = data.begin(); itr != data.end(); ++itr) {
    sql.append(itr->first);
    sql.append(itr->first == "tseDATE" ? " TEXT NOT NULL PRIMARY KEY,"
                                       : " TEXT NOT NULL,");
  }
  if (!sql.empty()) sql.pop_back();
  sql.append(" );");
  rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
  return rc;
}

int32_t AddStockData(sqlite3 *db, std::string tableName, StockData_T data) {
  int32_t rc = 0;
  char *zErrMsg = 0;
  std::string sql = "INSERT INTO ";
  sql.append(tableName);
  sql.append(" (");
  for (auto itr = data.begin(); itr != data.end(); ++itr) {
    sql.append(itr->first);
    sql.append(",");
  }
  if (!sql.empty()) sql.pop_back();
  sql.append(") VALUES (");
  for (auto itr = data.begin(); itr != data.end(); ++itr) {
    sql.append("'");
    sql.append(itr->second);
    sql.append("',");
  }
  if (!sql.empty()) sql.pop_back();
  sql.append(");");
  rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
  return rc;
}

void SaveToDB(sqlite3 *db, const StockDataTable_T &dataTable,
              progress_callback callback) {
  const int32_t total = dataTable.size();
  int32_t cnt = 0;
  double progress = 0.0;
  for (auto table = dataTable.begin(); table != dataTable.end(); ++table) {
    CreateTable(db, table->first, table->second);
    AddStockData(db, table->first, table->second);
    progress = (double)(cnt++) / (double)total;
    if (callback) callback(progress);
  }
}

int CallBackVector(void *notUsed, int32_t argc, char **argv, char **colName) {
  std::vector<std::string> *pResult = (std::vector<std::string> *)notUsed;
  for (int32_t i = 0; i < argc; ++i) {
    pResult->emplace_back(std::string(argv[i]));
  }
  return 0;
}

int CallBackMap(void *notUsed, int32_t argc, char **argv, char **colName) {
  StockData_T *pResult = (StockData_T *)notUsed;
  for (int32_t i = 0; i < argc; ++i) {
    pResult->insert(std::make_pair<std::string, std::string>(
        std::string(colName[i]), std::string(argv[i])));
  }
  return 0;
}

int32_t GetTables(sqlite3 *db, std::vector<std::string> &tableNames) {
  int32_t rc = 0;
  char *zErrMsg = 0;
  std::string sql = "SELECT name FROM sqlite_master WHERE type = 'table';";
  rc = sqlite3_exec(db, sql.c_str(), CallBackVector, &tableNames, &zErrMsg);
  return rc;
}

int32_t GetLatestValue(sqlite3 *db, std::string tableName,
                       const std::string &date, StockData_T &value) {
  int32_t rc = 0;
  char *zErrMsg = 0;
  std::string sql = "SELECT * FROM ";
  sql.append(tableName);
  sql.append(" WHERE tseDATE = ");
  sql.append(date);
  sql.append(" ;");
  rc = sqlite3_exec(db, sql.c_str(), CallBackMap, &value, &zErrMsg);
  return rc;
}

int32_t GetLatestValue(sqlite3 *db, const std::string &date,
                       StockDataList_T &datas) {
  std::vector<std::string> tableNames;
  GetTables(db, tableNames);
  for (auto &tableName : tableNames) {
    StockData_T value;
    GetLatestValue(db, tableName, date, value);
    datas.emplace_back(value);
  }
  return 0;
}