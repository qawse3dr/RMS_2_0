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

enum class RmsDatabaseTypes {
  kMySQL,
  kSqlite,
};

class RmsDatabase {
 private:
  RmsDatabaseTypes database_type_;

 public:
  RmsDatabase(const RmsDatabaseTypes& type);
  ~RmsDatabase();
};

#endif  // _INCLUDE_RMS_COMMON_RMS_DATABASE_H_
