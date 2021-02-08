# EBCpp

Event Based C++ (EBCpp) will be a framework that uses Events (Signal-Slot) to provide features to the user. This framework is designed to work with C++17 and without a special pre compiler. The framework is a header only framework and designed that it could be used very easily. Just include the header files and use the functionalities provided.

The events are synchronised to one thread. Therefore the events are thread-safe. You can use multible threads by setting up additional `EBEventLoop`s. This should only be used by experienced users.

## Table of Content
* [Installation](#installation)
* [Example](#example)

## Example

The following example shows the usage of an `EBTimer` that prints a message.

```C++
#include "src/EBEventLoop.hpp"
#include "src/EBTimer.hpp"

using namespace std;
using namespace EBCpp;

class ExampleTimer : public EBObject
{
public:
	ExampleTimer(EBObject* parent) : 
        EBObject(parent),
        timer(this)
	{
		timer.timeout.connect(*this, &ExampleTimer::timeout);
		timer.start( 1000 );
	}

private:
    EBTimer timer;

	EB_SLOT(timeout)
	{
		static int i = 0;
		cout << "timeout " << i++ << endl;
	}

};

int main()
{
	ExampleTimer tt(nullptr);
    EBEventLoop::getInstance().exec();
    return 0;
}
```
