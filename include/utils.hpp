#ifndef UTILS_HPP
#define UTILS_HPP

#include <fnmatch.h>
#include <termios.h>

#include <algorithm>
#include <fstream>
#include <map>

#include <boost/algorithm/string.hpp>

#include "proc.hpp"
#include "uri.hpp"

std::map<std::string, std::string> get_hosts(const std::string& host_grp) {
  std::ifstream is("/etc/sxx/hosts");

  // map<host, port>
  std::map<std::string, std::string> hosts;
  std::string section;
  for (std::string line; getline(is, line);) {
    boost::trim(line);
    if (!line.empty() && line[0] != '#') {
      if (host_grp.empty()) {
        printf("%s\n", line.c_str());
        continue;
      }

      size_t end;
      if (line[0] == '[' && (end = line.find(']')) != std::string::npos) {
        section = line.substr(1, end - 1);
        if (fnmatch(host_grp.c_str(), section.c_str(), 0)) {
          section = "";
        }
      } else if (!section.empty()) {
        const uri uri(line);
        hosts[uri.host_] = uri.port_;
      }
    }
  }

  if (hosts.empty()) {
    const uri uri(host_grp);
    hosts[uri.host_] = uri.port_;
  }

  return hosts;
}

void set_stdin_echo(const bool enable) {
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  if (enable) {
    tty.c_lflag |= static_cast<tcflag_t>(ECHO);
  } else {
    tty.c_lflag &= ~static_cast<tcflag_t>(ECHO);
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

enum id { ssh, scp, rsync, term, list, ssh_copy_id, exe };

struct type {
  id id_;
  std::string name_;

  type() : id_(ssh), name_("ssh") {}

  type(const std::string& type_str) {
    name_ = type_str;
    if (type_str == "scp") {
      id_ = scp;
    } else if (type_str == "rsync") {
      id_ = rsync;
    } else if (type_str == "term") {
      id_ = term;
    } else if (type_str == "list") {
      id_ = list;
    } else if (type_str == "ssh-copy-id") {
      id_ = ssh_copy_id;
    } else if (type_str == "ssh") {
      id_ = ssh;
    } else if (type_str == "exe") {
      id_ = exe;
    } else {
      fprintf(stderr, "Invalid type '%s'\n", type_str.c_str());
      exit(-1);
    }
  }
};

void grp_cmd(const type& type,
             const std::map<std::string, std::string>& hosts,
             const std::string& usr,
             const std::string& arg1,
             const std::string& arg2 = "") {
  std::string password;
  if (type.id_ == list) {
    for (const auto& host : hosts) {
      std::string list = host.first;
      if (host.second != "22") {
        list += ":" + host.second;
      }

      printf("%s\n", list.c_str());
    }
    return;
  } else if (type.id_ == ssh_copy_id) {
    printf("Password: ");
    set_stdin_echo(false);
    char* password_c_str = NULL;
    size_t len = 0;
    if (getline(&password_c_str, &len, stdin) == -1) {
      fprintf(stderr, "getline\n");
    }

    password = password_c_str;
    free(password_c_str);
    set_stdin_echo(true);
    printf("\n");
  }

  std::vector<proc> procs;
  for (const auto& host : hosts) {
    std::vector<std::string> args;
    std::string cmd = type.name_;
    std::string file_contents;
    if (type.id_ == ssh) {
      args = {"-p", host.second, usr + host.first, arg1};
    } else if (type.id_ == term) {
      std::string ssh_command =
          "ssh -p " + host.second + ' ' + usr + host.first;
      if (!arg1.empty()) {
        ssh_command += " '" + arg1 + '\'';
      }
      args = {"-e", ssh_command};
      cmd = getenv("COLORTERM");
    } else if (type.id_ == ssh_copy_id) {
      cmd = "sshpass";
      args = {"-p" + password, type.name_, "-p", host.second, usr + host.first};
    } else if (type.id_ == exe) {
      std::ifstream is(arg1);
      std::string interpreter;
      getline(is, interpreter);
      interpreter.substr(2);

      for (std::string line; getline(is, line);) {
        file_contents += line;
      }

      args = {"-p", host.second, usr + host.first, interpreter};
    } else if (type.id_ == scp) {
      args = {"-P", host.second, arg1, usr + host.first + arg2};
    } else {  // it's an rsync
      args = {"-e", "ssh -p " + host.second, arg1, usr + host.first + arg2};
    }

    proc proc(host.first, cmd, args, file_contents);
    procs.push_back(proc);
  }

  for (const proc& proc : procs) {
    Poco::PipeInputStream out_stream(proc.out_pipe_);
    std::string out;
    for (int c = out_stream.get(); c != -1; c = out_stream.get()) {
      out += static_cast<char>(c);
    }

    Poco::PipeInputStream err_stream(proc.err_pipe_);
    std::string err;
    for (int c = err_stream.get(); c != -1; c = err_stream.get()) {
      err += static_cast<char>(c);
    }

    const int es = proc.proc_hand_.wait();
    const std::string color = es ? "\033[;31m" : "\033[;32m";
    printf("%s%s | es=%d\033[m\n", color.c_str(), proc.host_.c_str(), es);
    printf("%s", out.c_str());
    fprintf(stderr, "%s", err.c_str());
    printf("\n");
  }
}

std::string split_by_char(std::string& arg, char c) {
  const std::string arg_unaltered = arg;
  size_t pos = arg_unaltered.find(c);
  if (pos == std::string::npos) {
    return "";
  }
  ++pos;

  arg = arg_unaltered.substr(pos, arg_unaltered.size());

  return arg_unaltered.substr(0, pos);
}

#endif
