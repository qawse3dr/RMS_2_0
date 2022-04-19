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

    res = executeQuery(RMS_DB_CREATE_USAGE_TABLE);
    if (!res.success) break;

    break;
  }

  if (!res.success) {
    std::cerr << "Failed to create tables exiting" << std::endl;
    exit(1);
  }
}

}  // namespace server
}  // namespace rms