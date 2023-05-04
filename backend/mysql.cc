
#pragma once
#include"cpp_http/httplib.h"
#include"log.hpp"
#include"until.hpp"
#include"Mysql.hpp"
#include<jsoncpp/json/json.h>
#include<functional>
#include<unordered_map>
#include<vector>
#define Login "/mi/sigin"
#define Register "/mi/sigup"
class TcpServer
{
public:
    using func_t = std::function<void(const httplib::Request&, httplib::Response&)>;
    TcpServer(const std::string& ip,int port)
    :_ip(ip),_port(port){
       _log.init();//启动日志功能
    }
    void init()
    {
      if(!_mysql.init())
      {
        _log.log(FATAL,"数据库连接失败!");
        exit(-1);
      }
      //注册处理请求的方法
      set_func();
    }
    void listen()
    {
        server.listen(_ip.c_str(), _port);
    }
private:
    httplib::Server server;//服务器
    std::string _ip;
    int _port;
    MY_LOG _log;
   MY_MYSQL:: mysql _mysql;
    void set_func()
    {
        func_t func = std::bind(&TcpServer::_login,this,std::placeholders::_1,std::placeholders::_2);
        server.Post(Login,func);
        server.Get(Login,func);
        func = std::bind(&TcpServer::_register,this,std::placeholders::_1,std::placeholders::_2);
        server.Post(Register,func);
        server.Get(Register,func);
    }
    void _register(const httplib::Request& req, httplib::Response&rep)//注册
    {
        std::string body = req.body;
        Json::Value root;
        _log.log(DEBUG,"用户进行注册中");
        if(!until::json_read(body,root))
        {
            _log.log(DEBUG,"status:400 json 序列化错误!");
            rep.status = 400;
            return;
        }
        std::string password = root.get("password","null").asString();
        std::string name = root.get("name","null").asString();
        if(password=="null" || name=="null")
        {
            rep.status = 400;
            return;
        }
        std::cout<<"name:"<<name<<" password:"<<password<<std::endl;
        body = "(\'" + name + "\'," + password + ")";
        std::string _query = "insert into users(name,password) values"+body;
        if(!_mysql.query(_query.c_str()))
        {
            rep.status = 500;
            _log.log(DEBUG,"插入失败 status:500");
            return;
        }
        //数据插入完毕 
        //得到数据库生产的id 返回给前端
        Json::Value send;
         std::vector<std::vector<std::string>> v;
        _query = "select max(id) from users";
        if(!_mysql.query(_query.c_str(),v))
        {
             rep.status = 500;
            _log.log(DEBUG,"获取ID失败 status:500");
        }
        std::string id = v[0][0];
        send["id"] = id;
        rep.status = 200;
        std::string s;
        until::json_write(s,send);
        rep.set_content(s.c_str(),"application/json; charset=UTF-8");
        _log.log(DEBUG,"200 注册成功!");
    }
    void _login(const httplib::Request& req, httplib::Response&rep)// /登录
    {
        
        //     std::vector<std::vector<std::string>> v;
        //     if(!_mysql.query("select * from users",v))
        //     {
        //         _log.log(DEBUG,"query失败!");
        //         return;
        //     }
        //     _log.log(DEBUG,"query成功!");
        // int row = v.size();
        // Json::Value root;
        // for(int i = 0;i<row;i++)
        // {
        //     Json::Value obj;
        //     obj["id"] = v[i][0];
        //     obj["name"] = v[i][1];
        //     obj["password"] = v[i][2];
        //     root.append(obj);
        // }
        // std::string json = root.toStyledString();
        // rep.status = 200;   
        // rep.set_content(json.c_str(),"application/json; charset=UTF-8");
        //序列化
        std::string body = req.body;
        Json::Value root;
        if(!until::json_read(body,root))
        {
            _log.log(DEBUG,"json_read 错误!");
        }
        //Json::Value& _root = root.get("name");
       
        std::string id = root.get("id","null").asString();
        std::string password = root.get("password","null").asString();
        std::vector<std::vector<std::string>> v;
        std::string _query = "select id from users where id=" +id;
        _log.log(DEBUG,"id:%s ",id.c_str());
        //与数据库进行比对
        if(!_mysql.query(_query.c_str(),v))
        {
            rep.status = 201;
            _log.log(DEBUG,"status:201");
            return;
        }
        std::cout<<"v[0][0]:"<<v[0][0]<<std::endl;
        if(v[0][0]!=id)
        {
            rep.status = 201;
         _log.log(DEBUG,"id 错误!");
         return;
        }
        _query = "select password from users where password="+password;
        v.clear();
        if(!_mysql.query(_query.c_str(),v))
        {
            rep.status = 201;
           _log.log(DEBUG,"status:201");
            return;
        }
        std::cout<<"密码:"<<password<<std::endl;
        if(v[0][0]!=password)
        {
            rep.status = 201;
            _log.log(DEBUG,"status:404");
         return;
        }
        rep.status = 200;
       _log.log(DEBUG,"status:200");
    }
};
int main()
{
    TcpServer server("0.0.0.0",9000);
    server.init();
    server.listen();
}
