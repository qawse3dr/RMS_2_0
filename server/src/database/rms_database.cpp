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
#include <sqlite3.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <tuple>

#include "rms/server/database/rms_database.h"

namespace rms {
namespace server {

RmsDatabase::RmsDatabase(const RmsDatabaseTypes& type) : database_type_(type) {}

void RmsDatabase::createTables() {
  RmsQueryResult res;

  // Create a way to do a transaction
  for (;;) {
    res = executeQuery(RMS_DB_CREATE_COMPUTER_TABLE);
    if (!res.success) break;

    res = executeQuery(RMS_DB_CREATE_STORAGE_TABLE);
    if (!res.success) break;

    res = executeQuery(RMS_DB_CREATE_NETWORK_INFO_TABLE);
    if (!res.success) break;

    res = executeQuery(RMS_DB_CREATE_NETWORK_IP_TABLE);
    if (!res.success) break;
    break;
  }

  if (!res.success) {
    std::cerr << "Failed to create tables exiting" << std::endl;
    exit(1);
  }
}

bool RmsDatabase::getComputerFromDB(std::shared_ptr<RmsComputer>& computer) {
  if (computer->getComputerId() == -1) {
    // TODO add log here
    std::cerr << "Computer id invalid not grabbing from db" << std::endl;
    return false;
  }
  auto res = executeQuery(
      fmt::format(RMS_DB_FETCH_COMPUTER_BY_ID, computer->getComputerId())
          .c_str());
  if (res.success) {
    rms::common::thrift::SystemInfo sys_info;
    for (int i = 0; i < res.column_names.size(); i++) {
      if (res.column_names[i] == "system_name") {
        sys_info.system_name = res.table_rows[0][i];
      } else if (res.column_names[i] == "host_name") {
        sys_info.host_name = res.table_rows[0][i];
      } else if (res.column_names[i] == "os_version") {
        std::stringstream stream(res.table_rows[0][i]);
        char ch;
        int major, minor, release;
        stream >> major >> ch >> minor >> ch >> release;
        sys_info.os_version.__set_major(major);
        sys_info.os_version.__set_minor(minor);
        sys_info.os_version.__set_release(release);
      } else if (res.column_names[i] == "client_version") {
        std::stringstream stream(res.table_rows[0][i]);
        char ch;
        int major, minor, release;
        stream >> major >> ch >> minor >> ch >> release;
        sys_info.client_version.__set_major(major);
        sys_info.client_version.__set_minor(minor);
        sys_info.client_version.__set_release(release);
      } else if (res.column_names[i] == "cpu_name") {
        sys_info.cpu_info.cpu_name = res.table_rows[0][i];
      } else if (res.column_names[i] == "cpu_vendor") {
        sys_info.cpu_info.cpu_vendor_name = res.table_rows[0][i];
      } else if (res.column_names[i] == "cpu_core_count") {
        sys_info.cpu_info.cpu_cores = std::stol(res.table_rows[0][i]);
      } else if (res.column_names[i] == "cpu_cache_size") {
        sys_info.cpu_info.cache_size = std::stol(res.table_rows[0][i]);
      } else if (res.column_names[i] == "cpu_arch") {
        // change to string
        if (res.table_rows[0][i] == "kX86_64") {
          sys_info.cpu_info.arch = common::thrift::Architecture::kX86_64;
        }
      }
    }
    computer->setSysInfo(sys_info);
  } else {
    std::cerr << "Failed to get computer with id " << computer->getComputerId()
              << " From database" << std::endl;
  }
  return res.success;
}

bool RmsDatabase::insertComputerIntoDB(std::shared_ptr<RmsComputer>& computer) {
  const auto& sys_info = computer->getSysInfo();
  auto res = executeQuery(
      fmt::format(RMS_DB_INSERT_COMPUTER_TABLE, sys_info.system_name,
                  sys_info.host_name, sys_info.os_version.major,
                  sys_info.os_version.minor, sys_info.os_version.release,
                  sys_info.client_version.major, sys_info.client_version.major,
                  sys_info.client_version.release, sys_info.cpu_info.cpu_name,
                  sys_info.cpu_info.cpu_vendor_name,
                  sys_info.cpu_info.cpu_cores, sys_info.cpu_info.cache_size,
                  rms::common::thrift::to_string(sys_info.cpu_info.arch))
          .c_str(),
      true);
  if (res.success) {
    computer->setComputerId(res.last_id);
  }

  return res.success;
}

}  // namespace server
}  // namespace rms