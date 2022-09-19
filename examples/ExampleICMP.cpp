#include "../src/EBApplication.hpp"
#include "../src/EBTimer.hpp"
#include "../src/socket/raw/EBICMP.hpp"

using namespace EBCpp;

class ExampleICMP : public EBObject<ExampleICMP>
{
public:
    ExampleICMP()
    {
        if (EBApplication::getArgumentCount() != 3)
        {
            EB_LOG_INFO("Start this application with \"multiPing.exe [start ip] [end ip]\"");
            okay = false;
            return;
        }

        EBString startIp = EBApplication::getArgument(1);
        startAddress = EBUtils::ipToInt(startIp);

        EBString endIp = EBApplication::getArgument(2);
        endAddress = EBUtils::ipToInt(endIp);

        timer.timeout.connect(this, &ExampleICMP::timeout);

        for (uint32_t i = startAddress; i <= endAddress; i++)
        {
            EBString ip = EBUtils::intToIp(i);
            EBICMP* icmp = new EBICMP();
            icmps.append(icmp);

            icmp->setDestination(EBString() + "icmp://" + ip + "/");
            icmp->finished.connect(this, &ExampleICMP::icmpFinished);
            icmp->error.connect(this, &ExampleICMP::icmpError);
        }

        this->ping();
        okay = true;

        timer.start(5000);
    }

    bool isOkay()
    {
        return okay;
    }

private:
    bool okay;

    EBList<EBICMP*> icmps;
    uint32_t address;
    uint32_t startAddress;
    uint32_t endAddress;

    EBTimer timer;

    void ping()
    {
        for (auto icmp : icmps)
        {
            icmp.get()->ping();
        }
    }

    EB_SLOT(icmpError)
    {
        // EB_LOG_ERROR(sender->cast<EBICMP>()->getDestination().toString() << ": " <<
        // sender->cast<EBICMP>()->getLastError());
    }

    EB_SLOT_WITH_ARGS(icmpFinished, EBICMP::ICMP_RESULT result)
    {
        if (result.status == 0)
        {
            EB_LOG_INFO(result.host << " >> " << result.roundtrip << " ms");
        }
    }

    EB_SLOT(timeout)
    {
        this->ping();
    }
};

EBObjectPointer<ExampleICMP> exampleIcmp = nullptr;
bool EBInit()
{
    exampleIcmp = EBObjectBase::createObject<ExampleICMP>();
    return exampleIcmp->isOkay();
}

void EBShutdown()
{
}
