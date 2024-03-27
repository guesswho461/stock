#define ERROR -1

#include <algorithm>
#include <iostream>
#include <vector>

#include "gflags/gflags.h"
#include "mongoose/mongoose.h"
#include "routefunctions.h"
#include "sqlite3.h"
#include "stockdbhelper.h"
#include "utility.h"

sqlite3* db = NULL;
int32_t rc = 0;
std::string ret_msg;

DEFINE_string(db, "test.db", "stock db file");
DEFINE_string(port, "8080", "port to listen");
DEFINE_int32(log_level, 3, "log level, 0: no log, 4: most detailed");

const std::unordered_map<std::string,
                         std::tuple<std::string, route_fn_prototype>>
    route_fn_table = {{"/MaxQuantity", {"POST", GetMaxQuantity}}};

static void callback(struct mg_connection* c, int ev, void* ev_data) {
  if (ev != MG_EV_HTTP_MSG || db == NULL) return;
  const struct mg_http_message* hm = (const struct mg_http_message*)ev_data;
  const std::string key(hm->uri.ptr, hm->uri.len);
  if (route_fn_table.count(key) == 1) {
    const auto value = route_fn_table.at(key);
    const auto method = std::get<0>(value);
    const auto route_fn = std::get<1>(value);
    if (mg_match(hm->method, mg_str("OPTIONS"), NULL)) {
      rc = 200;
    } else if (mg_match(hm->method, mg_str(method.c_str()), NULL)) {
      ret_msg.clear();
      rc = route_fn(db, hm->body, ret_msg);
    } else {
      rc = 405;
      ret_msg.assign("unsupported method");
    }
  } else {
    rc = 404;
    ret_msg.assign("unsupported url");
  }
  mg_http_reply(c, rc,
                "Content-Type: application/json\n"
                "Access-Control-Allow-Origin:*\n"
                "Access-Control-Allow-Headers: X-Requested-With, Content-Type, "
                "Accept\r\n",
                ret_msg.append("\r\n").c_str());
}

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  rc = sqlite3_open(FLAGS_db.c_str(), &db);
  if (rc) {
    std::cout << "db open failed (" << rc << ")" << std::endl;
    return ERROR;
  }

  std::string addr = "http://0.0.0.0:";
  addr.append(FLAGS_port);

  std::cout << "get tw stock data from db server start to listen on " << addr
            << std::endl;

  struct mg_mgr mgr;
  mg_log_set(FLAGS_log_level);
  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, addr.c_str(), callback, NULL);
  for (;;) mg_mgr_poll(&mgr, 1000);
  mg_mgr_free(&mgr);

  std::cout << "get tw stock data from db server closed" << std::endl;

  return 0;
}