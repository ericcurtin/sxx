#include <iostream>
#include <regex>
#include <fstream>

#include <Poco/Process.h>
#include <Poco/PipeStream.h>
#include <Poco/String.h>
#include <Poco/Glob.h>

using std::vector;
using std::string;
using std::ifstream;
using std::cout;

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
  const ProcessHandle proc_hand_;

  proc(const string& host, const string& type, const vector<string>& args)
      : host_(host),
        proc_hand_(Process::launch(type, args, nullptr, &out_pipe_, nullptr)) {}
};

void grp_cmd(const string& type,
             const string& arg1,
             const string& arg2,
             const vector<string>& hosts,
             const string& arg3 = string()) {
  if (type == "list") {
    for (const string& host : hosts) {
      cout << host << '\n';
    }
    return;
  }

  vector<proc> procs;
  for (const string& host : hosts) {
    vector<string> args;
    if (type == "ssh") {
      args = {arg1 + host, arg2};
    } else {
      args = {arg1, arg2 + host + arg3};
    }
    proc proc(host, type, args);
    procs.push_back(proc);
  }

  for (const proc& proc : procs) {
    Poco::PipeInputStream i_str(proc.out_pipe_);
    string out;
    for (int c = i_str.get(); c != -1; c = i_str.get()) {
      out += static_cast<char>(c);
    }

    const int es = proc.proc_hand_.wait();
    const string color = es ? "\033[;31m" : "\033[;32m";
    cout << color << proc.host_ << " | "
         << "es=" << es << "\033[m\n"
         << out << "\n\n";
  }
}

vector<string> split_arg_by_at(string& arg) {
  const string arg_unaltered = arg;
  size_t at_pos = arg_unaltered.find('@');
  if (at_pos == string::npos) {
    at_pos = 0;
  } else {
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
  vector<string> args(argv + 1, argv + argc);

  string type = args.empty() ? "" : args[0];

  if (type == "ssh" || type == "scp" || type == "rsync" || type == "term" ||
      type == "list") {
    args.erase(args.begin());
  } else {
    type = "ssh";
  }

  vector<string>::const_iterator i = args.begin();
  for (; i < args.end() - 2; ++i) {
  }

  string arg1 = *(i);
  string arg2 = *(++i);

  vector<string> hosts;
  if (type == "ssh" || type == "list") {
    hosts = split_arg_by_at(arg1);
    grp_cmd(type, arg1, arg2, hosts);
  } else {
    const string arg3 = split_arg_by_colon(arg2);
    hosts = split_arg_by_at(arg2);
    grp_cmd(type, arg1, arg2, hosts, arg3);
  }

  return 0;
}
