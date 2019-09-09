#ifndef PROC_HPP
#define PROC_HPP

#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <vector>

char* str_to_char_ptr(const std::string& str) {
    return const_cast<char*>(str.c_str());
}

struct proc {
  const std::string host_;
  std::string out_;
  int output_[2];
  pid_t pid_;

  proc(const std::string& host,
       const std::string& command,
       const std::vector<std::string>& args) : host_(host) {
      if (pipe(output_) == -1) {
          exit(EXIT_FAILURE);
      }

      pid_ = fork();
      if (!pid_) {
          // collect both stdout and stderr to the one pipe
          close(output_[0]);
          dup2(output_[1], STDOUT_FILENO);
          dup2(output_[1], STDERR_FILENO);
          close(output_[1]);

          std::vector<char*> vc(args.size() + 2, NULL);
          vc.push_back(str_to_char_ptr(command));
          for (size_t i = 0; i < args.size(); ++i) {
            vc.push_back(str_to_char_ptr(command));
          }

          execvp(vc[0], &vc[0]);
          // if execvp() fails, we do *not* want to call exit()
          // since that can call exit handlers and flush buffers
          // copied from the parent process
          _exit(0);
      }

      close(output_[1]);
  }

  proc(const proc&) = default;
  proc& operator=(proc&&) throw() { return *this; }
};

#endif
