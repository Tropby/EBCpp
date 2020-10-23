# EBCpp

Event Based C++ (EBCpp) will be a framework that uses Events (Signal-Slot) to provide features to the user. This framework is designed to work with C++11 and without a special pre compiler. 
The events are synchronised to one thread. Therefore the events are thread-safe. You can use multible threads by setting up additional `EBEventLoop`s. This should only be used by experienced users.

## Table of Content
* [Installation](#installation)
* [Example](#example)


## Installation

EBCpp is developed with Eclipse (2020-06). This installation manual shows how to setup a project with the EBCpp classes.

Create a new C++ Managed Project

![Step1](https://github.com/Tropby/EBCpp/blob/master/img/installation_1.PNG)

Create an empty Project
I will create a Cross GCC setup. You can use other Toolchains

![Step2](https://github.com/Tropby/EBCpp/blob/master/img/installation_2.PNG)

Setup for Release and Debug

![Step3](https://github.com/Tropby/EBCpp/blob/master/img/installation_3.PNG)

I will not use the cross compiler settings.
Therefore its empty and will use the standard gcc and g++.

![Step4](https://github.com/Tropby/EBCpp/blob/master/img/installation_4.PNG)

Create a new folder for your dependencies (I have named it "dep")

![Step5](https://github.com/Tropby/EBCpp/blob/master/img/installation_5.PNG)

Copy the EBCpp folder to this "dep" folder.

![Step7](https://github.com/Tropby/EBCpp/blob/master/img/installation_7.PNG)

Setup your project "Project" -> "Properties" -> "C/C++ Build" -> "Settings" -> "Tool Settings"
Set the G++ Compiler Dialect to "ISO C++11"

![Step8](https://github.com/Tropby/EBCpp/blob/master/img/installation_8.PNG)

Add to the G++ Compiler Includes the "src" direrctory of the EBCpp

![Step9](https://github.com/Tropby/EBCpp/blob/master/img/installation_9.PNG)

For Windows you have to set the WINNT Version in Preprocessor settings.
Set it to "_WIN32_WINNT=0x0600". This is needed for some functions that are not availible in older versions of windows NT.

![Step10](https://github.com/Tropby/EBCpp/blob/master/img/installation_10.PNG)

"G++ Linker" -> "Libraries"
For windows you need to setup the linker to link to the "wsock32" and "ws2_32" libraries.
You need to add "pthread" as well. 

![Step11](https://github.com/Tropby/EBCpp/blob/master/img/installation_11.PNG)

Create a new file "main.cpp" in your project root directory.
You can start to use the EBCpp classes.

```C++
#include <EBEventLoop.h>
#include <EBTimer.h>

using namespace EBCpp;

void timeout()
{
	std::cout << "timeout" << std::endl;
}

int main()
{
	EBTimer timer;
	timer.timeout.connect( std::bind( timeout ) );
	timer.start( 1000 );
	return EBEventLoop::getMainLoop().exec();
}
```


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

    return EBEventLoop::getMainLoop().exec();
}
```
