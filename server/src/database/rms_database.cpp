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

#include "rms/server/database/rms_database.h"

#include <fmt/format.h>
#include <sqlite3.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <tuple>

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
  std::cout << "FINISHED QUERY" << std::endl;
  if (res.success) {
    rms::common::thrift::CpuInfo cpu_info;
    for (int i = 0; i < res.column_names.size(); i++) {
      if (res.column_names[i] == "system_name") {
        computer->setSysName(res.table_rows[0][i].c_str());
      } else if (res.column_names[i] == "host_name") {
        computer->setHostName(res.table_rows[0][i].c_str());
      } else if (res.column_names[i] == "os_version") {
        std::stringstream stream(res.table_rows[0][i]);
        char ch;
        int major, minor, release;
        stream >> major >> ch >> minor >> ch >> release;
        rms::common::thrift::VersionData os_verison;
        os_verison.__set_major(major);
        os_verison.__set_minor(minor);
        os_verison.__set_release(release);
        computer->setOSVersion(os_verison);
      } else if (res.column_names[i] == "client_version") {
        std::stringstream stream(res.table_rows[0][i]);
        char ch;
        int major, minor, release;
        stream >> major >> ch >> minor >> ch >> release;
        rms::common::thrift::VersionData client_version;
        client_version.__set_major(major);
        client_version.__set_minor(minor);
        client_version.__set_release(release);
        computer->setClientVersion(client_version);
      } else if (res.column_names[i] == "cpu_name") {
        computer->setCpuName(res.table_rows[0][i]);
      } else if (res.column_names[i] == "cpu_vendor") {
        computer->setCpuVendor(res.table_rows[0][i]);
      } else if (res.column_names[i] == "cpu_core_count") {
        cpu_info.cpu_cores = std::stol(res.table_rows[0][i]);
      } else if (res.column_names[i] == "cpu_cache_size") {
        cpu_info.cache_size = std::stol(res.table_rows[0][i]);
      } else if (res.column_names[i] == "cpu_cache_size") {
        // change to string
        cpu_info.arch = rms::common::thrift::Architecture::kX86_64;
      }
    }
    computer->setCpuInfo(cpu_info);
  } else {
    std::cerr << "Failed to get computer with id " << computer->getComputerId()
              << " From database" << std::endl;
  }
  return res.success;
}

bool RmsDatabase::insertComputerIntoDB(std::shared_ptr<RmsComputer>& computer) {
  auto& os_version = computer->getOSVersion();
  auto& client_version = computer->getClientVersion();

  auto res = executeQuery(
      fmt::format(RMS_DB_INSERT_COMPUTER_TABLE, computer->getSysName(),
                  computer->getHostName(), os_version.major, os_version.minor,
                  os_version.release, client_version.major,
                  client_version.major, client_version.release,
                  computer->getCpuName(), computer->getCpuVendor(),
                  computer->getCpuCoreCount(), computer->getCpuCacheSize(),
                  computer->getCpuArch())
          .c_str(),
      true);
  if (res.success) {
    computer->setComputerId(res.last_id);
  }

  return res.success;
}

}  // namespace server
}  // namespace rms