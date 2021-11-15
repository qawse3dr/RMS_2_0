'''
  * (C) Copyright 2021 Larry Milne (https://www.larrycloud.ca)
  *
  * This code is distributed on "AS IS" BASIS,
  * WITHOUT WARRANTINES OR CONDITIONS OF ANY KIND.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  * @author: qawse3dr a.k.a Larry Milne
'''
# This is a tcp server written in python used for testing the
# sent data from reporters.
# This should not be use in any way in production cause
# it will be slow as balls and lack core functionality of rms 2.0


import socket

import ctypes


# Data classes
class RamData(ctypes.Structure):
  _fields_ = [("is_swap_", ctypes.c_bool), ("total_", ctypes.c_long), ("free_", ctypes.c_long)]
class CpuInfo(ctypes.Structure):
  _fields_ = [("cpu_cores_", ctypes.c_ubyte), ("cache_size_", ctypes.c_ushort), ("arch_", ctypes.c_int)]
class CpuUsageData(ctypes.Structure):
  _fields_ = [("core_num_", ctypes.c_ubyte), ("usage_", ctypes.c_float)] 
class StorageInfo(ctypes.Structure):
  _fields_ = [("dev_", ctypes.c_char * 12),("fs_type_", ctypes.c_char * 8),("free_", ctypes.c_long), ("total_", ctypes.c_long)] 
class IP(ctypes.Union):
  _fields_ = [("ip", ctypes.c_long), ("ipv6", ctypes.c_int * 4)]
class NetworkInfo(ctypes.Structure):
  _fields_ = [("interface_name_", ctypes.c_char * 16), ("is_ipv6_", ctypes.c_bool), ("ip",IP)]
class TemperatureInfo(ctypes.Structure):
  _fields_ = [("name", ctypes.c_char * 16), ("path", ctypes.c_char * 16)] 
class VersionData(ctypes.Structure):
  _fields_ = [("release", ctypes.c_ubyte), ("major", ctypes.c_ubyte), ("minor", ctypes.c_ubyte)] 



class RequestHeader(ctypes.Structure):
  _fields_ = [("timestamp", ctypes.c_long), ("data_count", ctypes.c_int)]

class DataUnion(ctypes.Union):
  _fields_ = [("ram_data",RamData), ("cpu_info",CpuInfo), ("cpu_usage_data",CpuUsageData),("storage_info", StorageInfo), ("network_info",NetworkInfo),("temp_info",TemperatureInfo), ("version", VersionData), ("str_", ctypes.c_char * 32), ("long_", ctypes.c_long)]

class RequestData(ctypes.Structure):
  _fields_ = [("type", ctypes.c_uint), ("data", DataUnion)]




def printRequestData(req):
  if( req.type == 100): # kRamUsage
    print("----------------------")
    print("Ram Usage")
    print("Is Swap: %s" % (req.data.ram_data.is_swap_))
    print("Usage: %ldKb/%ldKb" % (req.data.ram_data.free_, req.data.ram_data.total_))
    print("----------------------")
  elif ( req.type == 200):
    print("----------------------")
    print("Cpu Usage")
    print("Core Number: %hd" % (req.data.cpu_usage_data.core_num_))
    print("Usage: %f%%" % (req.data.cpu_usage_data.usage_))
    print("----------------------")
  elif ( req.type == 201):
    print("----------------------")
    print("Cpu Info")
    print("CPU core count: %hd" % (req.data.cpu_info.cpu_cores_))
    print("Cpu Cache Size: %ldKb" % (req.data.cpu_info.cache_size_))
    print("Architecture: %d" % (req.data.cpu_info.arch_))
    print("----------------------")
  elif ( req.type == 202):
    print("----------------------")
    print("Cpu Vendor Name")
    print("CPU Vendor: %s" % (str(req.data.str_)))
    print("----------------------")
  elif ( req.type == 203):
    print("----------------------")
    print("Cpu Name")
    print("CPU Name: %s" % (str(req.data.str_)))
    print("----------------------")
  elif ( req.type == 301):
    print("----------------------")
    print("Sys Uptime")
    print("Sys Uptime: %d" % (req.data.long_))
    print("----------------------")
  elif ( req.type == 302):
    print("----------------------")
    print("Storage")
    print("Dev : %s" % (str(req.data.storage_info.dev_)))
    print("fs_type : %s" % (str(req.data.storage_info.fs_type_)))
    print("Usage: %ldKb/%ldKb" % (req.data.storage_info.free_, req.data.storage_info.total_))
    print("----------------------")
  elif ( req.type == 304):
    print("----------------------")
    print("Sys name")
    print("Name : %s" % (str(req.data.str_)))
    print("----------------------")
  elif ( req.type == 305):
    print("----------------------")
    print("host name")
    print("Name : %s" % (str(req.data.str_)))
    print("----------------------")
  elif ( req.type == 400):
    print("----------------------")
    print("Network Interface")
    print("Name : %s" % (str(req.data.network_info.interface_name_)))
    if (req.data.network_info.is_ipv6_):  
      ip = "ipv6 currently not supported"
      #ip = socket.inet_ntop(socket.AF_INET6, req.data.network_info.ipv6)
    else:
      ip = "ip4 not currenly supported"
      #ip = socket.inet_ntop(socket.AF_INET, req.data.network_info.ip)
    print("IP: %s" % (ip))
    print("----------------------")
  elif ( req.type == 500):
    print("Os Version")
    print("----------------------")
    print("OS version: (%d.%d.%d)" % (req.data.version.major, req.data.version.minor, req.data.version.release))
    print("----------------------")
  elif ( req.type == 501):
    print("reporter Version")
    print("----------------------")
    print("OS version: (%d.%d.%d)" % (req.data.version.major, req.data.version.minor, req.data.version.release))
    print("----------------------")

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(("0.0.0.0", 8080))
server.listen(5)


client, addr = server.accept()
print("Connection from {}:{}".format(addr[0],addr[1]))

import time
try:
  while(True):
    data = client.recv(ctypes.sizeof(RequestHeader));
    if(len(data) > 0):
      header = RequestHeader.from_buffer_copy(data)

      print("Incoming Packet")

      print("HEADER")
      print("-----------------------------------")
      print("Timestamp {}".format(header.timestamp))
      print("DataCount {}".format(header.data_count))
      print("-----------------------------------")

      for i in range(0, header.data_count, 1):
        data = client.recv(ctypes.sizeof(RequestData))
        requestData = RequestData.from_buffer_copy(data)
        printRequestData(requestData)
except Exception as e:
  print(e)

client.close()


