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
#include <tuple>

#include "rms/server/database/rms_database.h"

namespace rms {
namespace server {

RmsDatabase::RmsDatabase(const RmsDatabaseTypes& type) : database_type_(type) {}

void RmsDatabase::createTables() {
  RmsQueryResult res;

  // Create a way to do a transaction
  for (;;) {
    res = executeQuery(RMS_DB_CREATE_COMPUTER_TABLE, 0);
    if (!res.success) break;

    res = executeQuery(RMS_DB_CREATE_STORAGE_TABLE, 0);
    if (!res.success) break;

    res = executeQuery(RMS_DB_CREATE_NETWORK_INFO_TABLE, 0);
    if (!res.success) break;

    res = executeQuery(RMS_DB_CREATE_NETWORK_IP_TABLE, 0);
    if (!res.success) break;
    break;
  }

  if (!res.success) {
    std::cerr << "Failed to create tables exiting" << std::endl;
    exit(1);
  }
}

bool RmsDatabase::getComputerFromDB(std::shared_ptr<RmsComputer>& computer) {}

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
      0, true);
  if (res.success) {
    computer->setComputerId(res.last_id);
  }

  return res.success;
}

}  // namespace server
}  // namespace rms