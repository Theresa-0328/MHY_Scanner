#include "ThreadLocalServer.h"

ThreadLocalServer::ThreadLocalServer()
	: QThread()
{

}

ThreadLocalServer::~ThreadLocalServer()
{

}

void ThreadLocalServer::run()
{
    svr.Post("/ret", [&](const httplib::Request& req, httplib::Response& resp) 
        {
        body = req.body;
        std::cout << "验证码：" << body << std::endl;
        server_running = false;
        });

    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) 
        {
        std::string html_content;

        // 读取 HTML 文件的内容
        std::ifstream html_file("./Geetest/index.html");
        if (html_file.is_open()) {
            std::stringstream buffer;
            buffer << html_file.rdbuf();
            html_content = buffer.str();
            html_file.close();
        }
        else {
            // 如果无法打开 HTML 文件，返回错误响应
            res.status = 404;
            res.set_content("File not found", "text/plain");
            return;
        }

        // 设置响应头和正文内容
        res.set_header("Content-Type", "text/html");
        res.set_content(html_content, "text/html");
        });

    svr.Get("/geetest", [](const httplib::Request& req, httplib::Response& res) 
        {
        std::string html_content;

        // 读取 HTML 文件的内容
        std::ifstream html_file("./Geetest/geetest.html");
        if (html_file.is_open()) 
        {
            std::stringstream buffer;
            buffer << html_file.rdbuf();
            html_content = buffer.str();
            html_file.close();
        }
        else 
        {
            // 如果无法打开 HTML 文件，返回错误响应
            res.status = 404;
            res.set_content("File not found", "text/plain");
            return;
        }

        // 设置响应头和正文内容
        res.set_header("Content-Type", "text/html");
        res.set_content(html_content, "text/html");
        });

	svr.listen("127.0.0.1", 12983);
}

void ThreadLocalServer::stop()
{
    svr.stop();
}

std::string ThreadLocalServer::reCaptcha()
{
    return body;
}
