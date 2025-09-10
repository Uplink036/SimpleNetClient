#include <ip.h>
#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE( test_split_ip_port_from_string_ipv4 )
{
  char ipv4[] = "192.156.12.3:5000";
  char* ip;
  char* port; 
  int returnValue = split_ip_port_from_string(ipv4, &ip, &port);
  BOOST_TEST(returnValue == 0);
  BOOST_TEST(std::string("192.156.12.3") == ip);
  BOOST_TEST(std::string("5000") == port);
}

BOOST_AUTO_TEST_CASE( test_split_ip_port_from_string_ipv6 )
{
  char ipv6[] = "2001:0db8:0000:0000:0000:0000:1428:07ab:5000";
  char* ip;
  char* port; 
  int returnValue = split_ip_port_from_string(ipv6, &ip, &port);
  BOOST_TEST(returnValue == 0);
  BOOST_TEST(std::string("2001:0db8:0000:0000:0000:0000:1428:07ab") == ip);
  BOOST_TEST(std::string("5000") == port);
}

BOOST_AUTO_TEST_CASE( test_split_ip_port_from_string_dns )
{
  char dns[] = "www.google.com:5000";
  char* ip;
  char* port; 
  int returnValue = split_ip_port_from_string(dns, &ip, &port);
  BOOST_TEST(returnValue == 0);
  BOOST_TEST(std::string("www.google.com") == ip);
  BOOST_TEST(std::string("5000") == port);
}

BOOST_AUTO_TEST_CASE( test_split_ip_port_from_string_no_port )
{
  char dns[] = "www.google.com";
  char* ip;
  char* port; 
  int returnValue = split_ip_port_from_string(dns, &ip, &port);
  BOOST_TEST(returnValue < 0);
}