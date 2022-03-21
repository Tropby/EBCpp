
#include <EBObject.hpp>
#include <EBTimer.hpp>

// Example class that is inherited from the EBObject class
class Example : public EBCpp::EBObject<Example>
{
public:
    // Constructor that creates the connection from the timer timeout to the timeout slot of the Example class.
    Example() : timeoutCounter(0)
    {
        // Connect the signal (timeout) with the lot Example::timeout
        timer.timeout.connect(this, &Example::timeout);

        // Start the timer with an 1 s repeating timeout.
        timer.start(1000);
    }

private:

    // The timer instance
    EBCpp::EBTimer timer;

    int timeoutCounter;

    // The slot that is called by the timer
    EB_SLOT(timeout)
    {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::cout << "Timeout " << timeoutCounter++ << " @ " << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << std::endl;

        // Exit after 5 timeouts
        if( timeoutCounter == 20 )
        {
            EBCpp::EBEventLoop::getInstance()->exit();
        }
    }

};

int main()
{
    // Create an instance of the example class
    Example example;

    // Execute the main event loop
    EBCpp::EBEventLoop::getInstance()->exec();

    return 0;
}
