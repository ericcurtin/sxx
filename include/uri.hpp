#include <vector>

#include <boost/algorithm/string.hpp>

struct uri {
  std::string host_;
  std::string port_;

  uri(const std::string& uri_str) {
    std::vector<std::string> strs;
    std::string delimiter = uri_str.find("::") == std::string::npos
                                ? delimiter = ":"
                                : delimiter = "]:";

    boost::split(strs, uri_str, boost::is_any_of(delimiter));
    host_ = strs.front();
    boost::erase_all(host_, "[");
    if (strs.size() > 1) {
      port_ = strs.back();
      return;
    }

    port_ = "22";
  }
};
