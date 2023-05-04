#include <mysql.h>
#include<string>
#include<vector>
#include<iostream>
const std::string host = "localhost";
const std::string user = "root";
const std::string passwd = "xxxxx";
const std::string db = "test_db";
namespace MY_MYSQL
{
    class mysql
    {
        public:
            mysql(){;}
            ~mysql(){mysql_close(ms);}
            bool init()
            {
                 ms = mysql_init(nullptr);
                //2.连接数据库
                if(mysql_real_connect(ms, host.c_str(), user.c_str(), nullptr, db.c_str(), 3306, nullptr, 0) == nullptr)
                {
                    return false;
                }
                mysql_set_character_set(ms, "utf8"); 
                return true;
            }
            bool query(const std::string& sql,std::vector<std::vector<std::string>>& v)
            {

               std::cout<<"进入 query(v)"<<std::endl;
                if(mysql_query(ms, sql.c_str()) != 0)
                {
                    std::cout<<"mysql_query调用失败!"<<std::endl;
                   return false;
                }
                MYSQL_RES* res = mysql_store_result(ms);
                int row = mysql_num_rows(res);
                int col = mysql_num_fields(res);
                 std::cout<<"row:"<<row<<" col:"<<col<<std::endl;
                if(row==0 ||col==0)
                {
                    return false;
                }
                v.resize(row);
                
                for(int i = 0;i<row;i++)
                {
                    v[i].resize(col);
                    //获取一行数据 MYSQL ->char**
                    MYSQL_ROW _row = mysql_fetch_row(res);
                    for(int j = 0;j<col;j++)
                    {
                       v[i][j] = _row[j];
                    }
                }
                std::cout<<"与数据库对比成功!\n"<<std::endl;
                return true;
            }
            bool query(const std::string& sql)
            {
                 if(mysql_query(ms, sql.c_str()) != 0)
                {
                   return false;
                }
                return true;
            }
        private:
        MYSQL* ms;
    };
}
