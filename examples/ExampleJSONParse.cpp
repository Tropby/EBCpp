#include "../src/EBApplication.hpp"
#include "../src/json/EBJson.hpp"

class ExampleJSONParse : public EBCpp::EBObject<ExampleJSONParse>
{
public:
    ExampleJSONParse()
    {
        EBCpp::EBJsonObject obj;
        obj.parse("{\"age\": 42, \"hobby\": [\"Tennis\", \"Gaming\", \"Tetris\"], \"name\": \"Jason Smith\", \"size\": 1.94}");

        std::cout << "Name: " << obj.get("name")->dump() << std::endl;
        std::cout << "Age: " << obj.get("age")->dump() << std::endl;
 
        EBCpp::EBEventLoop::getInstance()->exit();
    }
};

EB_APPLICATION(ExampleJSONParse);
