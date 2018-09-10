#ifndef PROC_HPP
#define PROC_HPP

#include <Poco/PipeStream.h>
#include <Poco/Process.h>

struct proc {
  const std::string host_;
  Poco::Pipe in_pipe_;
  Poco::Pipe out_pipe_;
  Poco::Pipe err_pipe_;
  Poco::ProcessHandle proc_hand_;

  proc(const std::string& host,
       const std::string& command,
       const std::vector<std::string>& args,
       const std::string& file_contents)
      : host_(host),
    proc_hand_(
        Poco::Process::launch(command, args, &in_pipe_, &out_pipe_, &err_pipe_)) {
    Poco::PipeOutputStream ostr(in_pipe_);
    ostr << file_contents;
    ostr.close();
  }
};

#endif
