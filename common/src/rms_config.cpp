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

#include "rms/common/rms_config.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace rms {
namespace common {

// Define static vars
std::unordered_map<std::string, std::string> RmsConfig::config_;
std::string RmsConfig::config_path_ = "";
std::string RmsConfig::empty_str_ = "";
/**
 * Find given value for key
 * if RmsConfig is not set up or the key is not found returns null
 */
const std::string& RmsConfig::find(const std::string& key) {
  if (!config_path_.empty()) return config_[key];
  return empty_str_;
}

/**
 * Find given value for key
 * if RmsConfig is not set up or the key is not found returns null
 */
int RmsConfig::findInt(const std::string& key) {
  const auto& str = find(key);
  if(str.empty()) return -1;
  else return std::stod(str); // Assume this will work, crash if it doesn't
}

/** save the config to config_path_. if its not setup nothing will happen
 * but msg will be logged
 */
void RmsConfig::save() {
  std::fstream config_file;
  config_file.open(config_path_, std::ios::out);
  if (config_file.is_open()) {
    std::unordered_map<std::string, std::string>::iterator it = config_.begin();
    while (it != config_.end()) {
      config_file << it->first << " " << it->second << std::endl;
      it++;
    }
    config_file.close();
  } else {
    std::cerr << "Failed to open file for writing" << std::endl;
  }
}

/** loads the config from the config_path*/
void RmsConfig::load(const std::string& config_path) {
  config_path_ = config_path;

  std::fstream config_file;
  config_file.open(config_path_, std::ios::in);
  if (config_file.is_open()) {
    std::string config_line;
    while (std::getline(config_file, config_line)) {
      std::string key;
      std::string value;
      std::stringstream config_line_stream(config_line);
      config_line_stream >> key;
      config_line_stream >> value;

      std::cout << "Config found: " << key << "=" << value << std::endl;
      config_.emplace(std::move(key), std::move(value));
    }
    config_file.close();
  } else {
    config_path_ = "";
    std::cerr << "Failed to open config exiting..." << std::endl;
    exit(-1);
  }
}

/** replace value in config */
void RmsConfig::replace(const std::string& key, const std::string& value) {
  if (config_path_.empty()) {
    std::cout << "whoops" << std::endl;
    return;  // TODO: add log here
  }
  config_[key] = value;
}

}  // namespace common
}  // namespace rms