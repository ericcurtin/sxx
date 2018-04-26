#include <iostream>
#include <regex>
#include <fstream>

#include <termios.h>

#include <Poco/Process.h>
#include <Poco/PipeStream.h>
#include <Poco/String.h>
#include <Poco/Glob.h>

using std::vector;
using std::string;
using std::ifstream;
using std::cout;
using std::cin;
using std::cerr;

using Poco::Pipe;
using Poco::Process;
using Poco::ProcessHandle;
using Poco::trim;
using Poco::Glob;

vector<string> get_hosts(const string& host_grp) {
  ifstream is("/etc/sxx/hosts");
  vector<string> hosts;
  for (char c; is.get(c);) {
    if (c == '#') {
      while (is.get(c) && c != '\n') {
      }
    }

    if (c == '[') {
      string cur_host_grp;
      for (; is.get(c) && c != ']' && c != '\n'; cur_host_grp += c) {
      }
      trim(cur_host_grp);

      Glob glob(host_grp);
      if (!glob.match(cur_host_grp)) {
        continue;
      }

      for (string host; is.get(c); host += c) {
        if (c == '[') {
          is.unget();
          break;
        }

        if (isspace(c)) {
          trim(host);
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

struct proc {
  const string host_;
  Pipe out_pipe_;
  Pipe err_pipe_;
  const ProcessHandle proc_hand_;

  proc(const string& host, const string& command, const vector<string>& args)
      : host_(host),
        proc_hand_(
            Process::launch(command, args, nullptr, &out_pipe_, &err_pipe_)) {}
};

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

enum id { ssh, scp, rsync, term, list, copy_id };

struct type {
  id id_;
  string name_;

  type() : id_(ssh), name_("ssh") {}

  type(const string& type_str) {
    name_ = type_str;
    if (type_str == "scp") {
      id_ = scp;
    } else if (type_str == "rsync") {
      id_ = rsync;
    } else if (type_str == "term") {
      id_ = term;
    } else if (type_str == "list") {
      id_ = list;
    } else if (type_str == "copy-id") {
      id_ = copy_id;
    } else if (type_str == "ssh") {
      id_ = ssh;
    } else {
      cerr << "Invalid type '" << type_str << "'\n";
      exit(-1);
    }
  }
};

void grp_cmd(const type& type,
             const string& arg1,
             const string& arg2,
             const vector<string>& hosts,
             const string& arg3 = string()) {
  string password;
  if (type.id_ == list) {
    for (const string& host : hosts) {
      cout << host << '\n';
    }
    return;
  } else if (type.id_ == copy_id) {
    cout << "Password: ";
    set_stdin_echo(false);
    cin >> password;
    set_stdin_echo(true);
  }

  vector<proc> procs;
  for (const string& host : hosts) {
    vector<string> args;
    string cmd = type.name_;
    if (type.id_ == ssh) {
      args = {arg1 + host, arg2};
    } else if (type.id_ == term) {
      string ssh_command = "ssh " + arg1 + host;
      if (!arg2.empty()) {
        ssh_command += " '" + arg2 + '\'';
      }
      args = {"-e", ssh_command};
      cmd = getenv("COLORTERM");
    } else if (type.id_ == copy_id) {
      cmd = "sshpass";
      args = {"-p" + password, "ssh-copy-id", arg1 + host};
    } else {  // it's an scp or an rsync
      args = {arg1, arg2 + host + arg3};
    }
    proc proc(host, cmd, args);
    procs.push_back(proc);
  }

  for (const proc& proc : procs) {
    Poco::PipeInputStream out_stream(proc.out_pipe_);
    string out;
    for (int c = out_stream.get(); c != -1; c = out_stream.get()) {
      out += static_cast<char>(c);
    }

    Poco::PipeInputStream err_stream(proc.err_pipe_);
    string err;
    for (int c = err_stream.get(); c != -1; c = err_stream.get()) {
      err += static_cast<char>(c);
    }

    const int es = proc.proc_hand_.wait();
    const string color = es ? "\033[;31m" : "\033[;32m";
    cout << color << proc.host_ << " | "
         << "es=" << es << "\033[m\n";
    cout << out << '\n';
    cerr << err << '\n';
  }
}

vector<string> split_arg_by_at(string& arg) {
  const string arg_unaltered = arg;
  size_t at_pos = arg_unaltered.find('@');
  if (at_pos == string::npos) {
    at_pos = 0;
  } else {  // no user specified
    arg = arg_unaltered.substr(0, ++at_pos);
  }

  const size_t end = arg_unaltered.size();

  const string host_grp = arg_unaltered.substr(at_pos, end - at_pos);
  return get_hosts(host_grp);
}

string split_arg_by_colon(string& arg) {
  const string arg_unaltered = arg;
  const size_t colon_pos = arg_unaltered.find(':');

  arg = arg_unaltered.substr(0, colon_pos);

  const size_t end = arg_unaltered.size();

  return arg_unaltered.substr(colon_pos, end - colon_pos);
}

int main(const int argc, const char* argv[]) {
  if (argc == 1) {
    cout << "Usage: 1. sxx [mode] [user@]hostname [command]\n"
            "  or   2. sxx [mode] [user@]hostname [command]\n"
            "1. if mode is ssh, list, term or copy id\n"
            "2. if mode is scp or rsync\n"
            "default mode is ssh if command is provided, term otherwize\n";
    return 0;
  }

  vector<string> args(argv + 1, argv + argc);
  type type(args[0]);

  const string& shell = args.size() < 3 ? "" : args[2];

  vector<string> hosts;
  if (type.id_ == ssh || type.id_ == list || type.id_ == term ||
      type.id_ == copy_id) {
    hosts = split_arg_by_at(args[1]);
    const string& usr = args[1];
    grp_cmd(type, usr, shell, hosts);
  } else {  // it's an scp or an rsync
//    const string arg3 = split_arg_by_colon(arg2);
//    hosts = split_arg_by_at(arg2);
//    grp_cmd(type, arg1, arg2, hosts, arg3);
  }

  return 0;
}

