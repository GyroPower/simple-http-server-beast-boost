#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <sstream>


namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;


std::string read_file(const std::string& filePath)
{
    std::ifstream file(filePath);

    if(!file.is_open())
    {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void set_responseHtml(http::response<http::string_body>& res, 
    const std::string& file_nameLoc)
{
    std::string html_context = read_file("index.html");
    
    res.result(http::status::ok);
    res.set(http::field::server, "Beast");
    res.set(http::field::content_type, "text/html");
    res.body() = html_context;
    

}

void main_view(http::request<http::string_body>& req,
    http::response<http::string_body>& res)
{
    set_responseHtml(res, "index.htlm");
    res.version(req.version());
    res.keep_alive(req.keep_alive());
    res.prepare_payload();
}

void get_request(http::request<http::string_body>& req, 
    http::response<http::string_body>& res)
{
    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");
    res.body() = R"({"message": "This is a GET request"})";
    res.version(req.version());
    res.keep_alive(req.keep_alive());
    res.prepare_payload();
}

void post_request(http::request<http::string_body>& req,
    http::response<http::string_body>& res)
{
    res.result(http::status::ok);
    res.set(http::field::content_type, "application/json");
    res.body() = R"({"message": "This is a POST request"})";
    res.version(req.version());
    res.keep_alive(req.keep_alive());
    res.prepare_payload();   
}

void handle_request(beast::string_view doc_root, 
    http::request<http::string_body> req, 
    http::response<http::string_body>& res)
{

    if(req.method() == http::verb::get && req.target() == "/")
    {
        main_view(req, res);
    }
    else if(req.method() == http::verb::get && req.target() == "/api")
        get_request(req, res);
    else if(req.method() == http::verb::post && req.target() == "/api")
        post_request(req, res);

    
}

void do_session(tcp::socket& socket)
{
    bool close = false;
    boost::system::error_code ec;
    try{
        beast::flat_buffer buffer;
        http::request<http::string_body> req;

        http::read(socket, buffer, req, ec);


        if(ec == http::error::end_of_stream)
            close = true;
        else if(ec)
        {
            std::cerr<<"Read Error: " << ec.message() << "\n";
            return;
        }


        http::response<http::string_body> res;
        handle_request(".",req,res);

        http::write(socket, res, ec);

        if(ec)
        {
            std::cerr << "Write Error: " << ec.message() << "\n";
            return;
        }

        if(res.need_eof())
            close = true;

        if(close)
        {
            boost::system::error_code ec_close;
            socket.shutdown(tcp::socket::shutdown_send, ec_close);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}


int main()
{
    try{
        net::io_context io_context;
        tcp::acceptor acceptor(io_context, {tcp::v4(), 5050});

        for(;;)
        {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            do_session(socket);
        }
    }
    catch(std::exception& e){
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}