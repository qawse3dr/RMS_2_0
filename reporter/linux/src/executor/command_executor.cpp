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

#include "poll.h"
#include "rms/reporter/common/rms_reporter_client.h"
#include "rms/reporter/platform/executor/command_executor.h"

namespace rms::reporter {

CommandExecutor::CommandExecutor(int id, std::string cmd,
                                 std::vector<std::string> args)
    : Executor(id), command_(cmd), arguments_(args) {
  process_thread_ = std::thread(&CommandExecutor::runCommand, this);
}

CommandExecutor::~CommandExecutor() {
  {
    std::lock_guard<std::mutex> lck(fd_mtx_);
    // send signal to stop
    if (signal_fd_[0] != -1) {
      char zero = 0;
      write(signal_fd_[1], &zero, 1);
    }
  }
  process_thread_.join();
}

std::string CommandExecutor::getType() const { return "unix_command_executor"; }

void CommandExecutor::runCommand() {
  std::cout << "exec " << command_ << std::endl;
  // Setup all of the pipes

  int stdin_pipes[2];
  pipe(stdin_pipes);

  int stdout_pipes[2];
  pipe(stdout_pipes);

  int stderr_pipes[2];
  pipe(stderr_pipes);

  char* c_args[arguments_.size() + 1];
  auto pid = fork();
  if (pid == 0) {
    // child

    // close our fd's
    close(STDIN_FILENO);
    dup(stdin_pipes[0]);
    close(STDOUT_FILENO);
    dup(stdout_pipes[1]);
    close(STDERR_FILENO);
    dup(stderr_pipes[1]);

    // replace with the fd's we need
    // close all the pipes we don't need
    close(stdin_pipes[0]);
    close(stdin_pipes[1]);
    close(stdout_pipes[0]);
    close(stdout_pipes[1]);
    close(stderr_pipes[0]);
    close(stderr_pipes[1]);

    c_args[arguments_.size()] = nullptr;
    for (size_t idx = 0; idx < arguments_.size(); idx++) {
      c_args[idx] = arguments_[idx].data();
    }
    int ret = execvp(command_.data(), c_args);
    perror("exec");
  } else {
    if (pipe(signal_fd_) < 0) {
      // Do correct
    };
    // close the pipes we don't need
    close(stdin_pipes[0]);
    close(stdout_pipes[1]);
    close(stderr_pipes[1]);

    // set the fd's we need
    stdin_write_fd = stdin_pipes[1];
    stdout_read_fd = stdout_pipes[0];
    stderr_read_fd = stderr_pipes[0];
  }

  // be able to read in 1024 chars
  char buffer[1024];
  std::string out;
  std::string err;

  // reserve 64kb should be plenty of space for input
  out.reserve(1024 * 64);
  err.reserve(1024 * 64);

  // reading output
  bool running = true;
  while (running) {
    // set fd set
    // fd_set select_fds;
    // FD_ZERO(&select_fds);
    // FD_SET(signal_fd_[0], &select_fds);
    // FD_SET(stdout_read_fd, &select_fds);
    // FD_SET(stderr_read_fd, &select_fds);

    // Sleep to allow gathering of data
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    char buffer[1024];

    pollfd poll_fd[3] = {0};

    poll_fd[2].fd = signal_fd_[0];
    poll_fd[2].events = POLLIN;
    poll_fd[2].revents = 0;

    poll_fd[0].fd = stdout_read_fd;
    poll_fd[0].events = POLLIN;
    poll_fd[0].revents = 0;

    poll_fd[1].fd = stderr_read_fd;
    poll_fd[1].events = POLLIN;
    poll_fd[1].revents = 0;

    int ret = poll(poll_fd, 2, -1);

    if (ret == -1) {
      perror("select()");
    } else if (ret) {
      std::cout << "going into read" << std::endl;
      // check stdout
      if (poll_fd[0].revents & POLLIN) {
        ret = read(stdout_read_fd, buffer, 1024);
        if (ret < 0) {
          break;
        }
        out = buffer;
      } else if (poll_fd[0].revents & POLLRDHUP ||
                 poll_fd[0].revents & POLLERR || poll_fd[0].revents & POLLHUP) {
        // closed break
        break;
      }

      // check stderr
      if (poll_fd[1].revents & POLLIN) {
        read(stderr_read_fd, buffer, 1024);
        err = buffer;
      }
      if (poll_fd[1].revents & POLLIN || poll_fd[0].revents & POLLIN) {
        RmsReporterClient::getInstance().getRequestClient().reportExecutorData(
            id_, out, err);
        // clear contexts
        out.clear();
        err.clear();
      }

      // check signal last to make sure all data was flushed
      if (poll_fd[2].revents & POLLIN) {
        break;
      }
    }
  }

  // cleanup
  {
    std::lock_guard<std::mutex> lck(fd_mtx_);
    close(stdin_write_fd);
    close(stdout_read_fd);
    close(stderr_read_fd);
    close(signal_fd_[0]);
    close(signal_fd_[1]);
    signal_fd_[0] = -1;
  }
  std::cout << " exiting" << std::endl;
}

void CommandExecutor::input(std::string stdin_input) {
  write(stdin_write_fd, stdin_input.c_str(), stdin_input.length() + 1);
}
}  // namespace rms::reporter
