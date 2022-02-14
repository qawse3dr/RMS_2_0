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

#ifndef _RMS_COMMON_RMS_CONFIG_H_
#define _RMS_COMMON_RMS_CONFIG_H_

#include <string>
#include <unordered_map>

namespace rms {
namespace common {

// maybe i should make this a singleton but I think a static class should be
// fine for now
class RmsConfig {
 private:
  static std::unordered_map<std::string, std::string> config_;
  static std::string config_path_;
  static std::string empty_str_;
  RmsConfig();

 public:
  /**
   * Find given value for key
   * if RmsConfig is not set up or the key is not found returns null
   */
  static const std::string& find(const std::string& key);
  static int findInt(const std::string& key);
  /** save the config to config_path_. if its not setup nothing will happen
   * but msg will be logged
   */
  static void save();

  /** loads the config from the config_path*/
  static void load(const std::string& config_path);

  /** replace value in config */
  static void replace(const std::string& key, const std::string& value);
};

/***************** Static vars for config **********************/

// Reporter configs
#define RMS_REPORTER_CONFIG_COMPUTER_ID "computer_id"
#define RMS_REPORTER_CONFIG_SERVER_IP "server_ip"
#define RMS_REPORTER_CONFIG_SERVER_PORT "server_port"
#define RMS_REPORTER_CONFIG_TIMEOUT "timeout"

// Server Configs
#define RMS_SERVER_CONFIG_DB_TYPE "db_type"
#define RMS_SERVER_CONFIG_SQLITE_DB_PATH "db_path"
// Client Configs (this may not exist idk yet)

}  // namespace common
}  // namespace rms

#endif  // _RMS_COMMON_RMS_CONFIG_H_
