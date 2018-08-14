#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <fstream>
#include <iostream>

#include <termios.h>

#include <Poco/Glob.h>
#include <Poco/String.h>

#include "proc.hpp"

std::vector<std::string> get_hosts(const std::string& host_grp) {
  std::ifstream is("/etc/sxx/hosts");
  std::vector<std::string> hosts;
  for (char c; is.get(c);) {
    if (c == '#') {
      while (is.get(c) && c != '\n') {
      }
    }

    if (c == '[') {
      std::string cur_host_grp;
      for (; is.get(c) && c != ']' && c != '\n'; cur_host_grp += c) {
      }
      Poco::trim(cur_host_grp);

      Poco::Glob glob(host_grp);
      if (!glob.match(cur_host_grp)) {
        continue;
      }

      for (std::string host; is.get(c); host += c) {
        if (c == '[') {
          is.unget();
          break;
        }

        if (isspace(c)) {
          Poco::trim(host);
          host.erase(remove(host.begin(), host.end(), '\n'), host.end());
          if (!host.empty()) {
            hosts.push_back(host);
            host.clear();
            continue;
          }
        }
      }
    }
  }

  return hosts;
}

void set_stdin_echo(const bool enable) {
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  if (enable) {
    tty.c_lflag |= ECHO;
  } else {
    tty.c_lflag &= ~ECHO;
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

enum id { ssh, scp, rsync, term, list, ssh_copy_id };

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
    } else {
      std::cerr << "Invalid type '" << type_str << "'\n";
      exit(-1);
    }
  }
};

void grp_cmd(const type& type,
             const std::vector<std::string>& hosts,
             const std::string& usr,
             const std::string& arg1,
             const std::string& arg2 = "") {
  std::string password;
  if (type.id_ == list) {
    for (const std::string& host : hosts) {
      std::cout << host << '\n';
    }
    return;
  } else if (type.id_ == ssh_copy_id) {
    std::cout << "Password: ";
    set_stdin_echo(false);
    std::cin >> password;
    set_stdin_echo(true);
    std::cout << '\n';
  }

  std::vector<proc> procs;
  for (const std::string& host : hosts) {
    std::vector<std::string> args;
    std::string cmd = type.name_;
    if (type.id_ == ssh) {
      args = {usr + host, arg1};
    } else if (type.id_ == term) {
      std::string ssh_command = "ssh " + usr + host;
      if (!arg1.empty()) {
        ssh_command += " '" + arg1 + '\'';
      }
      args = {"-e", ssh_command};
      cmd = getenv("COLORTERM");
    } else if (type.id_ == ssh_copy_id) {
      cmd = "sshpass";
      args = {"-p" + password, type.name_, usr + host};
    } else {  // it's an scp or an rsync
      args = {arg1, usr + host + arg2};
    }
    proc proc(host, cmd, args);
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
    std::cout << color << proc.host_ << " | "
              << "es=" << es << "\033[m\n";
    std::cout << out << '\n';
    std::cerr << err << '\n';
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
