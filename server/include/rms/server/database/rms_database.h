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
#define RMS_DB_TYPE_FMT_FLOAT "{:f}"

#define RMS_DB_TYPE_FMT_VERSION "'{:d}.{:d}.{:d}'"
#define RMS_DB_TYPE_FMT_DATE "'{:d}-{:d}-{:d}'"

#define RMS_DB_CREATE_DATABASE "CREATE DATABASE rms_system_db;"

// Holds sys info about a computer
#define RMS_DB_CREATE_COMPUTER_TABLE \
  "CREATE TABLE computer_table("     \
  "computer_id INTEGER NOT NULL,"    \
  "system_name VARCHAR(256),"        \
  "host_name VARCHAR(256),"          \
  "os_version VARCHAR(64),"          \
  "client_version VARCHAR(32),"      \
  "cpu_name VARCHAR(256),"           \
  "cpu_vendor VARCHAR(64),"          \
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
  ");"

#define RMS_DB_UPDATE_COMPUTER_TABLE                                           \
  "UPDATE computer_table"                                                      \
  " SET system_name= " RMS_DB_TYPE_FMT_STRING                                  \
  ", host_name=" RMS_DB_TYPE_FMT_STRING                                        \
  ", os_version=" RMS_DB_TYPE_FMT_VERSION                                      \
  ", client_version=" RMS_DB_TYPE_FMT_VERSION                                  \
  ", cpu_name=" RMS_DB_TYPE_FMT_STRING ", cpu_vendor=" RMS_DB_TYPE_FMT_STRING  \
  ", cpu_core_count=" RMS_DB_TYPE_FMT_INT                                      \
  ", cpu_cache_size=" RMS_DB_TYPE_FMT_INT ", cpu_arch=" RMS_DB_TYPE_FMT_STRING \
  " WHERE computer_id = " RMS_DB_TYPE_FMT_INT ";"

#define RMS_DB_FETCH_COMPUTER_BY_ID \
  "SELECT * FROM computer_table WHERE computer_id = " RMS_DB_TYPE_FMT_INT

// Holds all storage devices mounted
#define RMS_DB_CREATE_STORAGE_TABLE       \
  "CREATE TABLE storage_table("           \
  "storage_info_id INTEGER NOT NULL,"     \
  "computer_id INT NOT NULL,"             \
  "dev_path VARCHAR(16),"                 \
  "fs_type VARCHAR(16),"                  \
  "mount_point VARCHAR(128),"             \
  "free INT,"                             \
  "total INT,"                            \
  "connected INT,"                        \
  "PRIMARY KEY(storage_info_id),"         \
  "FOREIGN KEY (computer_id) REFERENCES " \
  "computer_table(computer_id));"

#define RMS_DB_FETCH_STORAGE_BY_COMPUTER_ID \
  "SELECT * FROM storage_table WHERE computer_id = " RMS_DB_TYPE_FMT_INT ";"

#define RMS_DB_INSERT_STORAGE_TABLE                                \
  "INSERT INTO storage_table (computer_id,"                        \
  "dev_path, fs_type, mount_point, free, total, connected) VALUES" \
  "(" RMS_DB_TYPE_FMT_INT "," RMS_DB_TYPE_FMT_STRING               \
  "," RMS_DB_TYPE_FMT_STRING "," RMS_DB_TYPE_FMT_STRING            \
  "," RMS_DB_TYPE_FMT_INT "," RMS_DB_TYPE_FMT_INT ",1);"

#define RMS_DB_UPDATE_STORAGE_TABLE                                           \
  "UPDATE storage_table SET free=" RMS_DB_TYPE_FMT_INT                        \
  ", total=" RMS_DB_TYPE_FMT_INT ", connected=" RMS_DB_TYPE_FMT_INT           \
  ", mount_point=" RMS_DB_TYPE_FMT_STRING ", fs_type=" RMS_DB_TYPE_FMT_STRING \
  " WHERE storage_info_id=" RMS_DB_TYPE_FMT_INT ";"

// Holds all network device info for a device
#define RMS_DB_CREATE_NETWORK_INFO_TABLE  \
  "CREATE TABLE network_info_table("      \
  "network_info_id INTEGER NOT NULL,"     \
  "computer_id INT NOT NULL,"             \
  "connected INT,"                        \
  "PRIMARY KEY (network_info_id),"        \
  "FOREIGN KEY (computer_id) REFERENCES " \
  "computer_table(computer_id));"
#define RMS_DB_INSERT_NETWORK_INFO 0
#define RMS_DB_FETCH_NETWORK_INFO 0
#define RMS_DB_UPDATE_NETWORK_INFO 0

#define RMS_DB_CREATE_NETWORK_IP_TABLE        \
  "CREATE TABLE network_ip_table("            \
  "network_ip_id INTEGER NOT NULL,"           \
  "network_info_id INT,"                      \
  "ipv6 INT,"                                 \
  "ip VARCHAR(50),"                           \
  "PRIMARY KEY (network_ip_id),"              \
  "FOREIGN KEY (network_info_id) REFERENCES " \
  "network_info_table(network_info_id));"
#define RMS_DB_INSERT_NETWORK_IP 0
#define RMS_DB_REMOVE_NETWORK_IP 0
#define RMS_DB_FETCH_NETWORK_IP 0

// Holds all programs installed on machine
#define RMS_DB_CREATE_PKG_TABLE 0

// Holds all devices
#define RMS_DB_CREATE_DEVICES_TABLE 0

// usage will have 24 (from 0-23) ticks a day each with the max
// and the usage for that tick. Usage has no idea what data is stored
// in here just the an associated ID(+ an optional identifier) and double value
// (much like a can bus).
#define RMS_DB_CREATE_USAGE_TABLE \
  "CREATE TABLE usage_table("     \
  "date DATE NOT NULL,"           \
  "data_type INTEGER NOT NULL,"   \
  "data_id INTEGER,"              \
  "tick0 FLOAT,"                  \
  "tick1 FLOAT,"                  \
  "tick2 FLOAT,"                  \
  "tick3 FLOAT,"                  \
  "tick4 FLOAT,"                  \
  "tick5 FLOAT,"                  \
  "tick6 FLOAT,"                  \
  "tick7 FLOAT,"                  \
  "tick8 FLOAT,"                  \
  "tick9 FLOAT,"                  \
  "tick10 FLOAT,"                 \
  "tick11 FLOAT,"                 \
  "tick12 FLOAT,"                 \
  "tick13 FLOAT,"                 \
  "tick14 FLOAT,"                 \
  "tick15 FLOAT,"                 \
  "tick16 FLOAT,"                 \
  "tick17 FLOAT,"                 \
  "tick18 FLOAT,"                 \
  "tick19 FLOAT,"                 \
  "tick20 FLOAT,"                 \
  "tick21 FLOAT,"                 \
  "tick22 FLOAT,"                 \
  "tick23 FLOAT);"

#define RMS_DB_INSERT_USAGE_TABLE \
  "INSERT INTO usage_table ("     \
  "date, data_type) VALUES "      \
  "(" RMS_DB_TYPE_FMT_DATE ", " RMS_DB_TYPE_FMT_INT ");"

#define RMS_DB_INSERT_USAGE_TABLE_WITH_ID                                   \
  "INSERT INTO usage_table ("                                               \
  "date, data_type, data_id) VALUES "                                       \
  "(" RMS_DB_TYPE_FMT_DATE ", " RMS_DB_TYPE_FMT_INT "," RMS_DB_TYPE_FMT_INT \
  ");"

#define RMS_DB_UPDATE_USAGE_TABLE                          \
  "UPDATE usage_table SET tick{:d}=" RMS_DB_TYPE_FMT_FLOAT \
  " WHERE date=" RMS_DB_TYPE_FMT_DATE "and data_type=" RMS_DB_TYPE_FMT_INT ";"

#define RMS_DB_UPDATE_USAGE_TABLE_WITH_ID                                   \
  "UPDATE usage_table SET tick{:d}=" RMS_DB_TYPE_FMT_FLOAT                  \
  " WHERE date=" RMS_DB_TYPE_FMT_DATE " and data_type=" RMS_DB_TYPE_FMT_INT \
  " and data_id=" RMS_DB_TYPE_FMT_INT ";"

#define RMS_DB_DOES_USAGE_ENTRY_EXIST      \
  "SELECT COUNT(*) FROM usage_table WHERE" \
  " date=" RMS_DB_TYPE_FMT_DATE " and data_type=" RMS_DB_TYPE_FMT_INT ";"

#define RMS_DB_DOES_USAGE_ENTRY_EXIST_WITH_ID                         \
  "SELECT COUNT(*) FROM usage_table WHERE"                            \
  " date=" RMS_DB_TYPE_FMT_DATE " and data_type=" RMS_DB_TYPE_FMT_INT \
  " and data_id=" RMS_DB_TYPE_FMT_INT ";"

#define RMS_DB_FETCH_USAGE_TABLE_ON_DATE

#define RMS_DB_FETCH_USAGE_TABLE_ON_DATE_WITH_ID

#endif  // _INCLUDE_RMS_COMMON_RMS_DATABASE_H_
