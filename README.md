# EBCpp

Event Based C++ (EBCpp) will be a framework that uses Events (Signal-Slot) to provide features to the user. This framework is designed to work with C++11 and without a special pre compiler. 
The events are synchronised to one thread. Therefore the events are thread-safe. You can use multible threads by setting up additional `EBEventLoop`s. This should only be used by experienced users.

## Installation

EBCpp is developed with Eclipse (2020-06). This installation manual is 

## Example

The following example shows the usage of an `EBTimer` that prints a message.

```c++
#include "ebtimer.h"

using namespace std;
using namespace EBCpp;

class TimerTest
{
public:
	TimerTest()
	{
		timer.timeout.connect(std::bind( &TimerTest::timeout, this ));
		timer.start( 1000 );
	}

private:
    EBTimer timer;

	void timeout()
	{
		static int i = 0;
		cout << "timeout " << i++ << endl;
	}

};

int main()
{
	TimerTest tt;

    return EBEventLoop::getMainLoop().finished();
}
```
