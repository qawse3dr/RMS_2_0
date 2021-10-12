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

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(("0.0.0.0", 1234))
server.listen(5)


client, addr = server.accept()
print("Connection from {},{}".format(addr[0],addr[1]))

data = client.recv(1024);
print( data)
client.close()


