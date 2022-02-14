/*
 * (C) Copyright 2021 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTINES OR CONDITIONS OF ANY KIND.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: qawse3dr a.k.a Larry Milne
 */

#include <fmt/format.h>

#include <filesystem>
#include <iostream>
#include <utility>

#include "rms/common/rms_config.h"
#include "rms/server/database/rms_sqlite_database.h"

namespace rms {
namespace server {

RmsSqliteDatabase::RmsSqliteDatabase()
    : RmsDatabase(RmsDatabaseTypes::kSqlite) {
  std::cout
      << rms::common::RmsConfig::find(RMS_SERVER_CONFIG_SQLITE_DB_PATH).c_str()
      << std::endl;

  std::string filename =
      rms::common::RmsConfig::find(RMS_SERVER_CONFIG_SQLITE_DB_PATH);

  // Create the table if the file doesn't exist
  bool createTable = !std::filesystem::exists(filename);

  int rc = sqlite3_open(filename.c_str(), &db_);
  if (rc != SQLITE_OK) {
    std::cerr << "failed to open database exiting..." << std::endl;
    sqlite3_close(db_);
    exit(1);
  }

  if (createTable) {
    createTables();
  }
}

RmsSqliteDatabase::~RmsSqliteDatabase() {
  if (db_) {
    sqlite3_close(db_);
  }
}

RmsDatabase::RmsQueryResult RmsSqliteDatabase::executeQuery(
    const char* query, int row_length, bool return_insert_id) {
  RmsQueryResult res;
  res.success = true;

  sqlite3_stmt* sql_res;

  int rc = sqlite3_prepare_v2(db_, query, -1, &sql_res, 0);
  if (rc != SQLITE_OK) {
    res.success = false;
    std::cerr << "Failed to fetch data" << sqlite3_errmsg(db_) << std::endl;
    return res;
  }

  if (row_length == 0) {
    sqlite3_step(sql_res);
    if (return_insert_id) res.last_id = sqlite3_last_insert_rowid(db_);
    sqlite3_finalize(sql_res);
    return res;
  }
  // convert into rms QueryResult

  // first get column headers
  for (int i = 0; i < row_length; i++) {
    const unsigned char* text = sqlite3_column_text(sql_res, i);
    res.column_names.emplace_back(reinterpret_cast<const char*>(text));
  }
  sqlite3_step(sql_res);
  while (rc == SQLITE_ROW) {
    std::vector<std::string> row;
    for (int i = 0; i < row_length; i++) {
      const unsigned char* text = sqlite3_column_text(sql_res, i);
      row.emplace_back(reinterpret_cast<const char*>(text));
    }
    res.table_rows.emplace_back(std::move(row));
    sqlite3_step(sql_res);
  }

  if (return_insert_id) res.last_id = sqlite3_last_insert_rowid(db_);

  sqlite3_finalize(sql_res);
  return res;
}

}  // namespace server
}  // namespace rms
