#include "../src/EBObject.hpp"

using namespace EBCpp;

class Example : public EBObject<Example>
{
public:
    Example(int c) : c(c)
    {
    }

    void hello()
    {
        std::cout << "Hello :) c is " << c << std::endl;
    }

private:
    int c;
};

int main()
{
    {
        EBObjectPointer<Example> example = EBObjectBase::createObject<Example>(15);
        example->hello();
    }
    // Object will be destroyed at this point, because no more EBObjectPointers are pointing to it.
}