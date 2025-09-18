#include <netparser.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( TestNetParserSuit )

BOOST_AUTO_TEST_CASE( testGetProtocolTCP )
{
    char protocol[9];
    char input[] = "TCP://...";
    getProtocol(input, protocol);
    BOOST_CHECK_EQUAL(std::string("TCP"), protocol);
}


BOOST_AUTO_TEST_CASE( testGetProtocolUDP )
{
    char protocol[9];
    char input[] = "UDP://...";
    getProtocol(input, protocol);
    BOOST_CHECK_EQUAL(std::string("UDP"), protocol);
}

BOOST_AUTO_TEST_CASE( testGetProtocolFull )
{
    char protocol[9];
    char input[] =  "UDP://myserver.com:5000/text";
    getProtocol(input, protocol);
    BOOST_CHECK_EQUAL(std::string("UDP"), protocol);
}

BOOST_AUTO_TEST_CASE( testGetAPIText )
{
    char api[9];
    char input[] = "UDP://myserver.com:5000/text";
    getAPI(input, api);
    BOOST_CHECK_EQUAL(std::string("TEXT"), api);
}


BOOST_AUTO_TEST_CASE( testGetAPIBinary)
{
    char api[9];
    char input[] = "UDP://myserver.com:5000/binary";
    getAPI(input, api);
    BOOST_CHECK_EQUAL(std::string("BINARY"), api);
}

BOOST_AUTO_TEST_CASE( testGetIPnPort)
{
    char *ip;
    char *port;
    char input[] = "UDP://myserver.com:5000/binary";
    getIPnPORT(input,  &ip, &port);
    BOOST_CHECK_EQUAL(std::string("myserver.com"), ip);
    BOOST_CHECK_EQUAL(std::string("5000"), port);
    free(ip);
    free(port);
}


BOOST_AUTO_TEST_SUITE_END()