
#include "../src/EBString.hpp"
#include "../src/EBUrl.hpp"

using namespace EBCpp;

#include "EBTest.hpp"

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test7();
void test8();

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
    TEST(test8);
}

void test1()
{
    EBString test("Hallo");
    CHECK(test.length() == 5);
}

void test2()
{
    EBString test("Hallo");
    EBString test2 = test;

    CHECK(test2.length() == 5);
}

void test3()
{
    std::string str("Hallo");
    EBString test(str);

    CHECK(test.length() == 5);
}

void test4()
{
    EBString test1("Hallo");
    EBString test2("Test");
    EBString test3("Hallo");

    CHECK(test1.compare(test2) != 0);
    CHECK(test1.compare(test3) == 0);
    CHECK(test1.contains("al"));
    CHECK(test1 == test3);
    CHECK(test1 != test2);

    CHECK(test1 == "Hallo");
    CHECK(test1 != "test2");
}

void test5()
{
    EBString test("Hello ");
    test = test + "World";

    CHECK(test == "Hello World");

    test += "!";
    CHECK(test == "Hello World!");
    CHECK(test.equals("Hello World!"));
}

void test6()
{
    EBString test("Hallo ich bin ein Text!");
    EBString m1 = test.mid(6, 3);

    CHECK(m1 == "ich");
    CHECK(test.startsWith("Hallo"));
}

void test7()
{
    EBString test("Hallo");
    std::string str = test.toStdString();

    CHECK(str.compare("Hallo") == 0);
    CHECK(str.compare(test.dataPtr()) == 0);
}

void test8()
{
    EBString test("https://test.org:1234/test.html?arg1=system&arg2=system123");
    CHECK(test.indexOf(":") == 5);

    EBUrl url(test);
    EBUrl url2("https://test2.org/test.php?");
    EBUrl url3("https://test2.org/test1234.php?test:sdafhd=Multimedia/&hallo=sys=tem");

    std::cout << url << std::endl;
    std::cout << url2 << std::endl;
    std::cout << url3 << std::endl;
}