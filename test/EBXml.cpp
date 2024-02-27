
#include "../src/EBUrl.hpp"
#include "../src/profile/EBLogger.hpp"

using namespace EBCpp;

#include "../src/EBTest.hpp"
#include "../src/xml/EBXmlDocument.hpp"

int main()
{
    bool ok = true;
    //TEST(test1);
    TEST(test2);
}

void test1()
{
    EBXmlDocument doc;
    CHECK(!doc.loadFromMemory("<?xml?>"));
    CHECK(!doc.loadFromMemory("<?xml version=\"1.0\"?>"));
    CHECK(doc.loadFromMemory("<?xml version=\"1.0\"?><hello />"));
    CHECK(doc.loadFromMemory("<?xml version=\"1.0\"?><xml></xml>"));
    CHECK(doc.loadFromMemory("<?xml version=\"1.0\"?><xml>Hello World!</xml>"));
    CHECK_EXCEPTION(doc.loadFromMemory("<?xml version=\"1.0\"?><xml><help></help2></xml>"));
    CHECK_EXCEPTION(doc.loadFromMemory("<?xml version=\"1.0\"?><xml><help></help></xml><xml2></xml2>"));
    CHECK(doc.loadFromMemory("<?xml version=\"1.0\"?><xml>Test<help onclick='test();' system='x86'><i>TEST</i></help><HELLO /></xml>"));
}

void test2()
{
    EBXmlDocument doc;
    CHECK(doc.loadFromFile("../test/test2.xml"));

    std::cout << doc.dump() << std::endl;
}