/*
 * (C) Copyright 2022 Larry Milne (https://www.larrycloud.ca)
 *
 * This code is distributed on "AS IS" BASIS,
 * WITHOUT WARRANTINES OR CONDITIONS OF ANY KIND.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: qawse3dr a.k.a Larry Milne
 */


namespace cpp rms.common.thrift

// Common

struct VersionData {
  1: i32 release,
  2: i32 major,
  3: i32 minor
}

struct RmsHeader {
  1: i64 timestamp,
  2: i32 data_count
}


// CPU

struct CpuUsageData {
  1: i8 core_count,
  2: list<double> usage
}

enum Architecture {
  kX86_64 = 0,
  kX86,
  kArm,
  kArm64,
  kUnknown
}

struct CpuInfo {
  1: i32 cpu_cores,
  2: i16 cache_size,
  3: Architecture arch
  4: string cpu_name,
  5: string cpu_vendor_name,
}


// Network

struct NetworkInfo {
  1: string interface_name,
  2: bool is_ipv6,
  3: string ip,
}


// Ram

struct RamData {
  1: i64 main_total,
  2: i64 main_free,
  3: optional i64 swap_total,
  4: optional i64 swap_free

}

struct Script {
  1: string filename_,
  2: string script_data_
}

// Response
enum RmsResponseTypes {
  kSendSystemInfo = 0,
  kRunCommand,
  kRunScript
}


union RmsReponseValues {
  1: i64 long_,
  2: string cmd_,
  3: Script script_
}

struct RmsResponseData {
  1 : required RmsResponseTypes data_type,
  2 : required RmsReponseValues data
}

struct RmsResponse {
  1: RmsHeader header,
  2: list<RmsResponseData> data
}


// sys info

struct StorageInfo {
  1: string dev,
  2: string fs_type,
  3: i64 free,
  4: i64 total,
  5: string mount_point
}

// Temp
struct TemperatureInfo {
 1: string name,
 2: string path
}


struct SystemInfo {
  // Uptime
  1: i64 uptime,

  // System names
  2: string system_name,
  3: string host_name,

  // System versions
  4: VersionData os_version,
  5: VersionData client_version,

  // CPU Info
  6: CpuInfo cpu_info,

  // List of system info
  7: list<StorageInfo> storage_info,
  8: list<NetworkInfo> network_info,
  9: list<TemperatureInfo> temp_info
}


// Request

enum RmsRequestTypes {
   // Ram Info
  kRamUsage = 100,  // struct RamData

  // CPU Info
  kCpuUsage = 200,  // struct CpuUsageData

  // System Info
  kSysInfo = 300,

  // Network Info
  kNetworkUsage,


  kUnknown = 1000,
}

union RmsRequestValues {
  1: RamData ram_data,
  2: CpuUsageData cpu_usage_data,

  // multi-use types
  3: string str_,
  4: i64 long_

}

struct RmsRequestData {
  1 : required RmsRequestTypes data_type,
  // one of these data types should be set
  2 : required RmsRequestValues data
}

struct RmsRequest {
  1: RmsHeader header,
  2: list<RmsRequestData> data
}


service RmsReporterService {

  /**
   * creates the computer in the server
   */
  i64 handshake(1: i64 id, 2: SystemInfo sys_info),

  /**
   * Sends the sys info seperate from the rest
   * as it is much bigger so it increases the union
   * size. also will not be polled and only sent every so
   * often
  */
  oneway void sendSysInfo(1:SystemInfo sys_info),

  /**
   * Reports info to the server and gives back
   * a response request
   */
  RmsResponse report(1:RmsRequest request),


}
