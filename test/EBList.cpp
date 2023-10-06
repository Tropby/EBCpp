
#include "../src/EBList.hpp"
#include "../src/profile/EBLogger.hpp"
#include "../src/EBTest.hpp"
using namespace EBCpp;

void test1();

int main()
{
    bool ok = true;

    TEST(test1);
}

void test1()
{
    EBList<int> list;

    CHECK(list.getSize() == 0);

    list.append(12);

    CHECK(list.getSize() == 1);

    CHECK(list.get(0) == 12);

    list.append(5);

    CHECK(list.get(1) == 5);

    list.append(6);
    list.append(7);
    list.append(8);
    list.append(9);

    EB_LOG_DEBUG("TEST ITERATOR 12, 5, 6, 7, 8, 9");
    auto first = begin(list);
    auto last = end(list);
    for (; first != last; ++first)
    {
        EB_LOG_DEBUG(first.get());
    }

    list.removeAt(1);

    EB_LOG_DEBUG("TEST ITERATOR 12, 6, 7, 8, 9");
    for (auto& test : list)
    {
        EB_LOG_DEBUG(test.get());
    }

    list.insertAt(2, 123);
    list.insertAt(0, -1);
    list.insertAt(list.getSize(), -2);

    EB_LOG_DEBUG("TEST ITERATOR -1, 12, 6, 123, 7, 8, 9, -2");
    for (auto& test : list)
    {
        EB_LOG_DEBUG(test.get());
    }

    CHECK(list.contains(123));
    CHECK(list.contains(-1));
    CHECK(list.contains(-2));
    CHECK(!list.contains(124));

    list.remove(123);
    CHECK(!list.contains(123));

    list.remove(-1);
    CHECK(!list.contains(-1));

    list.remove(-2);
    CHECK(!list.contains(-2));
}
