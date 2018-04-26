#include "utils.hpp"

using std::string;

#define BOOST_TEST_MODULE Test
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test) {
  string host_dir = "user@host:~/";
  BOOST_CHECK_EQUAL("user@host:", split_by_char(host_dir, ':'));
  BOOST_CHECK_EQUAL("~/", host_dir);

  host_dir = "host:~/";
  BOOST_CHECK_EQUAL("host:", split_by_char(host_dir, ':'));
  BOOST_CHECK_EQUAL("~/", host_dir);

  string user_host = "user@host";
  BOOST_CHECK_EQUAL("user@", split_by_char(user_host, '@'));
  BOOST_CHECK_EQUAL("host", user_host);

  user_host = "host";
  BOOST_CHECK_EQUAL("", split_by_char(user_host, '@'));
  BOOST_CHECK_EQUAL("host", user_host);
}
