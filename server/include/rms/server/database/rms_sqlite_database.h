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

#ifndef _INCLUDE_RMS_COMMON_SQLITE3_RMS_DATABASE_H_
#define _INCLUDE_RMS_COMMON_SQLITE3_RMS_DATABASE_H_

#include <sqlite3.h>

#include "rms/server/database/rms_database.h"
namespace rms {
namespace server {

class RmsSqliteDatabase : public RmsDatabase {
 private:
  sqlite3* db_ = nullptr;

 public:
  RmsSqliteDatabase();
  ~RmsSqliteDatabase();

  RmsQueryResult executeQuery(const char* query, int row_length,
                              bool return_insert_id = false) override;
};

}  // namespace server
}  // namespace rms

#endif  // _INCLUDE_RMS_COMMON_SQLITE3_RMS_DATABASE_H_
