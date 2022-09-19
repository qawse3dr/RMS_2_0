/*
 * (C) Copyright 2022 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: qawse3dr a.k.a Larry Milne
 */
#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>

namespace rms::common::platform {

enum class RmsFilePermissions { kRead, kReadWrite, kReadWriteExecute };

/**
 * @brief parent class for system configuration folder and files.
 * and will be a wrapper for files stored in .local and .config and possibly
 * /etc
 *
 */
class RmsSystemFile {
 private:
  std::fstream sys_file_ = {};
  std::string filename_;
  RmsFilePermissions perms_;

  // create the file
  bool create() {
    switch (perms_) {
      case RmsFilePermissions::kRead:
        sys_file_ = std::fstream(filename_, std::ios::in);
        break;
      case RmsFilePermissions::kReadWrite:
        if (exists()) {
          sys_file_ = std::fstream(filename_, std::ios::app);
        }
        break;
    }
    return true;
  }

 public:
  RmsSystemFile(
      const std::string& filename,
      const RmsFilePermissions& perms = RmsFilePermissions::kReadWrite,
      bool overwrite = false)
      : filename_(filename), perms_(perms) {
    if (!create()) {
      std::cerr << "Failed to create file " << filename << std::endl;
    }
  }

  /** reads the content from the system file returns an empty string
   * if the file failed to open or it doesn't exist
   * @returns context of the file or "" if it doesn't exist or failed to open.
   */
  std::string read() {
    if (sys_file_.is_open()) {
      sys_file_.seekg(std::ios::beg);
      std::stringstream ss;
      ss << sys_file_.rdbuf();
      return ss.str();
    }
    return "";
  }

  /**
   * @brief writes the content to the file overwriting whatever was in the file
   *
   * @param txt content to be written to the file
   * @return true
   * @return false
   */
  bool write(const std::string& txt) {
    create();  // create a new file
    if (sys_file_.is_open()) {
      sys_file_.seekg(std::ios::beg);
      sys_file_ << txt;
      return true;
    }
    return false;
  }

  bool append(const std::string& txt) {
    if (sys_file_.is_open()) {
      sys_file_.seekg(std::ios::end);
      sys_file_ << txt;
      return true;
    }
    return false;
  }

  /**
   * @brief erase the current file leaving it is a bad state. this object should
   * not be used after this is called and is considered UB
   *
   * @return true
   * @return false
   */
  bool erase() { return std::filesystem::remove(filename_); }

  /**
   * @brief
   *
   * @return true
   * @return false
   */

  bool exists() { return std::filesystem::exists(filename_); };

  const std::string& getFilename() { return filename_; }
  const RmsFilePermissions getPerms() { return perms_; }
};

class RmsSystemConfigFile : public RmsSystemFile {};

class RmsSystemDataFile : public RmsSystemFile {};

class RmsSystemTmpFile : public RmsSystemFile {
 private:
  bool delete_;

 public:
  RmsSystemTmpFile(const std::string& filename, bool remove)
      : RmsSystemFile((std::filesystem::temp_directory_path() +=
                       std::filesystem::path(filename))
                          .string(),
                      RmsFilePermissions::kReadWrite, true),
        delete_(remove) {}
  ~RmsSystemTmpFile() {
    if (delete_) {
      erase();
    }
  }
};

}  // namespace rms::common::platform
