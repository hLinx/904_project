#include<jsoncpp/json/json.h>
#include<string>
namespace until
{
    //json
    bool json_read(const std::string& json,Json::Value& root)
    {
        Json::Reader re;
        if(!re.parse(json,root,true))
        {
            return false;
        }
        std::cout<<"json成功!"<<std::endl;
        return true;
    }
    void json_write(std::string& json,Json::Value& root)
    {
       
        json = root.toStyledString();
    }

}   
