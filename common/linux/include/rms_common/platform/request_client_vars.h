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

#ifndef _COMMON_LINUX_INCLUDE_REQUEST_CLIENT_VARS_H_
#define _COMMON_LINUX_INCLUDE_REQUEST_CLIENT_VARS_H_

/**
 * I know this probably isn't the best way to do this
 * but is allows for a simple solution so until I learn a
 * better way in my systems programming class this is how it will stay.
 *
 * But anyways the point of this file is hold vars needed for the tcp and HTTP
 * client which will be placed as private in the RequestClient class.
 */

bool tcp_setup_;
int tcp_sockfd_;
int tcp_opt_;
struct sockaddr_in tcp_address_;

#endif  // _COMMON_LINUX_INCLUDE_REQUEST_CLIENT_VARS_H_
