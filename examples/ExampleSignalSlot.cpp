
#include "../src/EBObject.hpp"
#include "../src/EBEvent.hpp"

using namespace EBCpp;

class ExampleSignal : public EBObject<ExampleSignal>
{
public:
    ExampleSignal()
    {
    }

    EB_SIGNAL(helloWorldCalled);

    void helloWorld()
    {
        EB_EMIT(helloWorldCalled);
    }

private:
    
};

class ExampleSlot : public EBObject<ExampleSlot>
{
public:
    ExampleSlot()
    {
    }

    EB_SLOT(helloWorld)
    {
        std::cout << "Hello World!" << std::endl;
                
        EBEventLoop::getInstance()->exit();
    }

private:
};

int main()
{
    ExampleSignal signal;
    ExampleSlot slot;

    signal.helloWorldCalled.connect((&slot).get(), &ExampleSlot::helloWorld);
    signal.helloWorld();
    signal.helloWorldCalled.disconnect((&slot).get(), &ExampleSlot::helloWorld);

    EBEventLoop::getInstance()->exec();
}