#ifndef _ROUTE_FUNCTIONS_H_
#define _ROUTE_FUNCTIONS_H_

#include <string>

#include "mongoose/mongoose.h"
#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

typedef int32_t (*route_fn_prototype)(sqlite3*, const struct mg_str&,
                                      std::string&);

int32_t GetMaxQuantity(sqlite3* db, const struct mg_str& input,
                       std::string& output);

#endif