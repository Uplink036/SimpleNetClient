#include <ip.h>
#define BOOST_TEST_MODULE module_ip
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE( testIPv4 )
{
  char ipv4[] = "192.156.12.3:5000";
  char* ip;
  char* port; 
  int returnValue = splitIPPortFromString(ipv4, &ip, &port);
  BOOST_TEST(returnValue == 0);
  BOOST_TEST(std::string("192.156.12.3") == ip);
  BOOST_TEST(std::string("5000") == port);
  free(ip);
  free(port);
}

BOOST_AUTO_TEST_CASE( testIPv6 )
{
  char ipv6[] = "2001:0db8:0000:0000:0000:0000:1428:07ab:5000";
  char* ip;
  char* port; 
  int returnValue = splitIPPortFromString(ipv6, &ip, &port);
  BOOST_TEST(returnValue == 0);
  BOOST_TEST(std::string("2001:0db8:0000:0000:0000:0000:1428:07ab") == ip);
  BOOST_TEST(std::string("5000") == port);
  free(ip);
  free(port);
}

BOOST_AUTO_TEST_CASE( testDNS )
{
  char dns[] = "www.google.com:5000";
  char* ip;
  char* port; 
  int returnValue = splitIPPortFromString(dns, &ip, &port);
  BOOST_TEST(returnValue == 0);
  BOOST_TEST(std::string("www.google.com") == ip);
  BOOST_TEST(std::string("5000") == port);
  free(ip);
  free(port);
}

BOOST_AUTO_TEST_CASE( testNoPort )
{
  char dns[] = "www.google.com";
  char* ip;
  char* port; 
  int returnValue = splitIPPortFromString(dns, &ip, &port);
  BOOST_TEST(returnValue < 0);
}

BOOST_AUTO_TEST_CASE( testNoIP )
{
  char noIP[] = ":5000";
  char* ip;
  char* port; 
  int returnValue = splitIPPortFromString(noIP, &ip, &port);
  BOOST_TEST(returnValue < 0);
}

BOOST_AUTO_TEST_CASE( testEmptyString )
{
  char emptyInput[] = "";
  char* ip;
  char* port; 
  int returnValue = splitIPPortFromString(emptyInput, &ip, &port);
  BOOST_TEST(returnValue < 0);
}

BOOST_AUTO_TEST_CASE( testIPIsNotInt )
{
  char ipv4[] = "192.11.11.11:abcd";
  char* ip;
  char* port; 
  int returnValue = splitIPPortFromString(ipv4, &ip, &port);
  BOOST_TEST_CONTEXT( "port is " << port)
  {
    BOOST_CHECK(returnValue < 0);
  }
  free(ip);
  free(port);
}
