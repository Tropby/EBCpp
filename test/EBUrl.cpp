
#include "../src/EBUrl.hpp"
#include "../src/profile/EBLogger.hpp"

using namespace EBCpp;

#include "EBTest.hpp"


int main()
{
    bool ok = true;

    TEST(test1);
    TEST(test2);
    TEST(test3);
    TEST(test4);
    TEST(test5);
    TEST(test6);
    TEST(test7);
}

void test1()
{
    EBUrl url("http://example.org");
    CHECK(url.getProtocol() == "HTTP");
    CHECK(url.getHost() == "example.org");
    CHECK(url.getPort() == 80);
}

void test2()
{
    EBUrl url("http://example.org/");
    CHECK(url.getProtocol() == "HTTP");
    CHECK(url.getHost() == "example.org");
    CHECK(url.getPort() == 80);
}

void test3()
{
    EBUrl url("//example.org");
    CHECK(url.getProtocol() == "UNKNOWN");
    CHECK(url.getHost() == "example.org");
    CHECK(url.getPort() == 0);
}

void test4()
{
    EBUrl url("http://example.org/path1");
    CHECK(url.getProtocol() == "HTTP");
    CHECK(url.getHost() == "example.org");
    CHECK(url.getPort() == 80);
    CHECK(url.getPath() == "/path1");
}

void test5()
{
    EBUrl url("urn:example:animal:ferret:nose");
    CHECK(url.getProtocol() == "URN");
    CHECK(url.getPath() == "example:animal:ferret:nose");
}

void test6()
{
    EBUrl url("http://example.org/path1/path2?query=10");
    CHECK(url.getProtocol() == "HTTP");
    CHECK(url.getPath() == "/path1/path2");
    CHECK(url.getArgCount() == 1);
    CHECK(url.getArg("query") == "10" );
}

void test7()
{
    EBUrl url;
    
    url.setHost("example.org");
    CHECK(url.getHost() == "example.org");
    CHECK(url.toString() == "://example.org/");
    
    url.setProtocol("HTTP");
    CHECK(url.getProtocol() == "HTTP");
    CHECK(url.toString() == "http://example.org/");
    
    url.setPort(8080);
    CHECK(url.getPort() == 8080);
    CHECK(url.toString() == "http://example.org:8080/");

    url.setPath("path1");
    CHECK(url.getPath() == "path1");
    CHECK(url.toString() == "http://example.org:8080/path1");

    url.setArg("key1", "value1");
    CHECK(url.getArgCount() == 1);
    CHECK(url.getArg("key1") == "value1");
    CHECK(url.toString() == "http://example.org:8080/path1?key1=value1");

    url.setUsername("root");
    CHECK(url.getUsername() == "root");
    CHECK(url.toString() == "http://root@example.org:8080/path1?key1=value1");

    url.setPassword("my!pw");
    CHECK(url.getPassword() == "my!pw");
    CHECK(url.toString() == "http://root:my!pw@example.org:8080/path1?key1=value1");
}