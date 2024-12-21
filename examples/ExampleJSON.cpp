#include "../src/EBApplication.hpp"
#include "../src/json/EBJson.hpp"

class ExampleJSON : public EBCpp::EBObject<ExampleJSON>
{
public:
    ExampleJSON()
    {
        EBCpp::EBJsonObject obj;
        obj.set("name", "Jason Smith");
        obj.set("age", 42);
        obj.set("size", 1.94);

        EBCpp::EBJsonArray hobby;
        hobby.append("Tennis");
        hobby.append("Gaming");
        hobby.append("Tetris");

        obj.set("hobby", hobby);

        std::cout << obj.dump() << std::endl;

        EBCpp::EBEventLoop::getInstance()->exit();
    }
};

EB_APPLICATION(ExampleJSON);
