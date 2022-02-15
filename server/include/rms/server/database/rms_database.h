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

#ifndef _INCLUDE_RMS_COMMON_RMS_DATABASE_H_
#define _INCLUDE_RMS_COMMON_RMS_DATABASE_H_

#include <memory>
#include <string>
#include <vector>

#include "rms/server/rms_computer.h"

namespace rms {
namespace server {

enum class RmsDatabaseTypes {
  kMySQL,
  kSqlite,
};

class RmsDatabase {
 private:
  RmsDatabaseTypes database_type_;

 public:
  /**
   * @brief it will be a little slower converting
   * sqlite and mysql data to another result,
   * but I don't want to write a different result parser
   * for every query of every db type
   */
  struct RmsQueryResult {
    bool success;  // if true column and rows will be filled
    int last_id;   // optional
    std::vector<std::string> column_names;
    std::vector<std::vector<std::string>> table_rows;
  };

  explicit RmsDatabase(const RmsDatabaseTypes& type);

  virtual RmsQueryResult executeQuery(const char* query,
                                      bool return_insert_id = false) = 0;

  void createTables();

  bool getComputerFromDB(std::shared_ptr<RmsComputer>& computer);
  bool insertComputerIntoDB(std::shared_ptr<RmsComputer>& computer);
};

}  // namespace server
}  // namespace rms

/**
 * definitions of all querys with the assumption you are going to use
 * fmt lib
 *
 */

#define RMS_DB_TYPE_FMT_STRING "'{:s}'"
#define RMS_DB_TYPE_FMT_INT "{:d}"
#define RMS_DB_TYPE_FMT_VERSION "'{:d}.{:d}.{:d}'"

#define RMS_DB_CREATE_DATABASE "CREATE DATABASE rms_system_db;"

#define RMS_DB_COMPUTER_TABLE_ROW_LENGTH 10
#define RMS_DB_CREATE_COMPUTER_TABLE \
  "CREATE TABLE computer_table("     \
  "computer_id INTEGER NOT NULL,"    \
  "system_name VARCHAR(32),"         \
  "host_name VARCHAR(32),"           \
  "os_version VARCHAR(32),"          \
  "client_version VARCHAR(32),"      \
  "cpu_name VARCHAR(32),"            \
  "cpu_vendor VARCHAR(32),"          \
  "cpu_core_count INT,"              \
  "cpu_cache_size INT,"              \
  "cpu_arch VARCHAR(10),"            \
  "PRIMARY KEY (computer_id));"

#define RMS_DB_INSERT_COMPUTER_TABLE                                         \
  "INSERT INTO computer_table("                                              \
  "system_name, host_name, os_version,"                                      \
  "client_version, cpu_name, cpu_vendor,"                                    \
  "cpu_core_count, cpu_cache_size, cpu_arch)"                                \
  "values ( " RMS_DB_TYPE_FMT_STRING "," RMS_DB_TYPE_FMT_STRING              \
  "," RMS_DB_TYPE_FMT_VERSION "," RMS_DB_TYPE_FMT_VERSION                    \
  "," RMS_DB_TYPE_FMT_STRING "," RMS_DB_TYPE_FMT_STRING                      \
  "," RMS_DB_TYPE_FMT_INT "," RMS_DB_TYPE_FMT_INT "," RMS_DB_TYPE_FMT_STRING \
  ")"

#define RMS_DB_FETCH_COMPUTER_BY_ID \
  "SELECT * FROM computer_table WHERE computer_id = " RMS_DB_TYPE_FMT_INT
#define RMS_DB_CREATE_STORAGE_TABLE       \
  "CREATE TABLE storage_table("           \
  "storage_info_id INTEGER NOT NULL,"     \
  "computer_id INT NOT NULL,"             \
  "dev_path VARCHAR(16),"                 \
  "fs_type VARCHAR(16),"                  \
  "free INT,"                             \
  "total INT,"                            \
  "connected INT,"                        \
  "PRIMARY KEY(storage_info_id),"         \
  "FOREIGN KEY (computer_id) REFERENCES " \
  "computer_table(computer_id));"

#define RMS_DB_CREATE_NETWORK_INFO_TABLE  \
  "CREATE TABLE network_info_table("      \
  "network_info_id INTEGER NOT NULL,"     \
  "computer_id INT NOT NULL,"             \
  "connected INT,"                        \
  "PRIMARY KEY (network_info_id),"        \
  "FOREIGN KEY (computer_id) REFERENCES " \
  "computer_table(computer_id));"

#define RMS_DB_CREATE_NETWORK_IP_TABLE        \
  "CREATE TABLE network_ip_table("            \
  "network_ip_id INTEGER NOT NULL,"           \
  "network_info_id INT,"                      \
  "ipv6 INT,"                                 \
  "ip VARCHAR(50),"                           \
  "PRIMARY KEY (network_ip_id),"              \
  "FOREIGN KEY (network_info_id) REFERENCES " \
  "network_info_table(network_info_id));"

#endif  // _INCLUDE_RMS_COMMON_RMS_DATABASE_H_
