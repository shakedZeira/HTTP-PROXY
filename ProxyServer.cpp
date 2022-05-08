/*
//#include "ProxySever.hpp"
#include <iostream>
#include <string>



bool ProxyServer::Start()
{
	try
	{
		WaitForClientConnection();
		_threadContext = std::thread([this]() { _asioContext.run();  });
	}
	catch(std::exception& e)
	{
		std::cerr << "Server Exception: " << e.what() << "\n";
		return false;
	}
	std::cout << " Server started! " << std::endl;
	return true;
}
void ProxyServer::Stop()
{
	_asioContext.stop();

	if (_threadContext.joinable())
	{
		_threadContext.join();
	}

	std::cout << "Server Stopped!\n";
}
void ProxyServer::WaitForClientConnection()
{
	_asioAcceptor.async_accept(
		[this](std::error_code ec, asio::ip::tcp::socket socket)
		{
			if (!ec)
			{
				std::cout << "new connection: " << socket.remote_endpoint() << "\n";
			}
			else
			{

			std::cout << "New Connection Error: " << ec.message() << "\n";

			}
			WaitForClientConnection();
		}
	);
}
void ProxyServer::MessageClient(int socket , const char* msg , int length)
{
	asio::post(_asioContext,
		[this, msg]()
		{
			asio::async_write(_socket, asio::buffer(&msg, sizeof(msg)),
				[this](std::error_code ec, std::size_t length)
				{
					if (!ec)
					{
						std::cout << "wrote to client: \n";
					}
					else
					{
						std::cout << "write error\n";
					}
				});
		});
}
void ProxyServer::MessageAllClients()
{

}
void ProxyServer::update()
{

}
bool ProxyServer::onClientConnect()
{
	return false;
}
void ProxyServer::OnClientDisconnect()
{

}
void ProxyServer::OnMessage(int socket, const char *msg , int length)
{
	asio::async_read(_socket, asio::buffer(&msg, sizeof(msg)),
		[this](std::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				std::cout << "read succecfully";
			}
			else
			{
				std::cout << "Read failed";
			}
		});
}

void main()
{

	//ProxyServer proxyserver(4000);
	//proxyserver.Start();

}

*/

/*
#include <iostream>
#include <chrono>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include <fstream>
#include <Windows.h>
#include <fstream>
#include <mysql.h> 
#include <stdio.h>
#include <stdlib.h>
#include "ProxyServer.hpp"



std::vector<char> bytesBuffer(20 * 1024);
int bytesIn;
void ProxyServer::GrabData(asio::ip::tcp::socket& socket)
{
    socket.async_read_some(asio::buffer(bytesBuffer.data(), bytesBuffer.size()),
        [&](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::cout << "\n\nRead " << length << " bytes\n\n";
                bytesIn = length;
                for (int i = 0; i < length; i++)
                {
                    std::cout << bytesBuffer[i];
                }
                GrabData(socket);
            }
        }
    );
}


void ProxyServer2::finish_with_error(MYSQL* con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

std::vector<std::string>ProxyServer2::parseFileType(std::string fileName)
{
    std::string parsedHtml;
    std::vector<std::string> tokens;
    std::stringstream check1(fileName);
    while (getline(check1, parsedHtml, '.'))
    {
        tokens.push_back(parsedHtml);
    }
    return tokens;
}

void ProxyServer2::UpdatePostRequests(std::string Username, int CounterPost, MYSQL* conn, std::string updateQuery)
{
    for (int i = 0; i < CounterPost; i++)
    {
        std::string result = updateQuery + Username;
        const char* chResult = result.c_str();
        if (mysql_query(conn, chResult))
        {
            finish_with_error(conn);
        }
    }
}

void ProxyServer2::UpdateGetRequests(std::string Username, int CounterGet, MYSQL* conn, std::string updateQuery)
{
    for (int i = 0; i < CounterGet; i++)
    {
        std::string result = updateQuery + Username;
        const char* chResult = result.c_str();
        if (mysql_query(conn, chResult))
        {
            finish_with_error(conn);
        }
    }
}

void ProxyServer2::ConnectToDataBase()
{
    connection = mysql_init(0);
    connection = mysql_real_connect(connection, "localhost", "root", "Lotussuper777", "testdb2", 3306, NULL, 0);
    if (connection)
    {
        std::cout << "connected to the data base!" << std::endl;
    }
    else
    {
        std::cout << "couldnt connect";
    }
}

void ProxyServer2::init()
{
    ConnectToDataBase();
    asio::io_context::work fakework(context);
    //asio::ip::tcp::socket socket(context);
    //std::thread threadContext = std::thread([&]() { context.run(); });
    threadContext = std::thread([&]() { context.run(); });
    asio::ip::tcp::endpoint Server(asio::ip::make_address("127.0.0.1", ErrorCode), 4200);
    try
    {
        io_service.run();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2000ms);
        socket.connect(Server, ErrorCode);
        if (!ErrorCode)
        {
            std::cout << "Connected to Server!" << std::endl;
        }
        else
        {
            std::cout << "Couldnt connect\n" << ErrorCode.message() << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void ProxyServer2::onClientConnected()
{
    asio::ip::tcp::acceptor acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 4100));
    asio::ip::tcp::socket socket(io_service);
    acceptor.accept(socket);
    _clientCount++;
    if (_clientCount <= 1)
    {
        std::cout << "Connected to client!" << std::endl;
        std::cout << "Please enter you UserName: ";
        getline(std::cin, userName);
        userName = "'" + userName + "'";
        std::string finduser;
        finduser = UsersQuery + userName;
        const char* chfinduser = finduser.c_str();
        if (mysql_query(connection, chfinduser))
        {
            //finish_with_error(conn);
            //ADD TO DATABASE ::::::::
            // 
            ////
        }
        res = mysql_store_result(connection);


    }
    IsConnected = true;
}

void ProxyServer2::Run()
{
    onClientConnected();
    while (1)
    {
        //
        if (socket.is_open())
        {
            GrabData(socket);
            if (sent == false)
            {
                std::string sRequest = "GET /app.component.html HTTP/1.1\r\n"
                    "Host: localhost\r\n"
                    "Connection: Keep-Alive\r\n\r\n";

                socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ErrorCode);
            }
            sent = true;



            using namespace std::chrono_literals;
            std::this_thread::sleep_for(2000ms);

            context.stop();
            if (threadContext.joinable())
            {
                threadContext.join();
            }

        }
        //

        SetCurrentDirectoryA("D:\\angular\\my-app\\src\\app\\");
        std::string HtmlFile = "/app.component.html";
        std::ifstream f("..\\app" + HtmlFile);

        if (f.good())
        {
            std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            strF = str;

        }

        std::string charBufTosString(bytesBuffer.begin(), bytesBuffer.end());
        std::istringstream iss(charBufTosString);
        std::vector<std::string> parsed((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
        std::string content = "<h2>404 Not Found</h2>";
        int errorcode = 404;
        if (parsed.size() >= 3 && parsed[0] == "GET")
        {
            counterGet++;
            HtmlFile = parsed[1];
            std::cout << "parsed[1]: " << parsed[1] << "\r\n";
            if (HtmlFile == "/")
            {
                HtmlFile = "/app.component.html";
            }
        }
        tokens = parseFileType(HtmlFile);
        if (tokens[1] == "jpeg")
        {
            std::ifstream img("..\\app" + HtmlFile, std::ios::binary);
            if (img.good())
            {
                std::string str((std::istreambuf_iterator<char>(img)), std::istreambuf_iterator<char>());
                content = str;
                errorcode = 200;
            }
            img.close();
        }
        else
        {
            std::ifstream f("..\\app" + HtmlFile);
            if (f.good())
            {
                std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
                content = str;
                errorcode = 200;
            }
            f.close();
        }


        std::string type = "text/html";
        tokens = parseFileType(HtmlFile);
        if (tokens[1] == "css")
        {
            type = "text/css";
        }
        if (tokens[1] == "js")
        {
            type = "text/javascript";
        }

        if (parsed.size() >= 3)
        {
            if (parsed[0] == "GET")
            {
                //write the document back to the client
                std::ostringstream oss;
                oss << "HTTP/1.1 " << errorcode << " OK\r\n";
                oss << "Cache-Control: no-cache , private\r\n";
                if (type == "image/jpeg")
                {
                    oss << "Content-Type: image/jpeg \r\n";
                    oss << "Content-Transfer-Encoding: binary\r\n";
                }
                else
                {
                    oss << "Content-Type: " << type << "\r\n";
                }
                oss << "Content-Length: " << content.size() << "\r\n";
                //to work on::::::::::::::::::::::::::::::::::::: all
                oss << "\r\n";
                oss << content;
                std::cout << "size: " << content.size() << "\r\n";
                std::string output = oss.str();
                int size = output.size() + 1;
                asio::write(socket, asio::buffer(output));
            }
            else if (parsed[0] == "POST" && parsed[1] == "/button")
            {
                counterPost++;
                std::string input;
                std::cout << "please enter what you would like to post: ";
                std::getline(std::cin, input);
                std::ostringstream postreq;
                postreq << "<h1>" << input << "</h1>";
                std::string str_postreq;
                str_postreq = postreq.str();
                //write the document back to the client
                std::ostringstream oss;
                oss << "HTTP/1.1 " << errorcode << "OK\r\n";
                oss << "Cache-Control: no-cache , private\r\n";
                oss << "Content-Type: " << type << "\r\n";
                oss << "Content-Length: " << str_postreq.size() << "\r\n";
                //to work on::::::::::::::::::::::::::::::::::::: all
                oss << "\r\n";
                oss << str_postreq;
                std::string output = oss.str();
                std::cout << "output: " << output;
                int size = output.size() + 1;
                std::cout << "content: \r\n" << content;
                //sendtoClient(clientSocket, output.c_str(), size);
                asio::write(socket, asio::buffer(output));
            }
            else if (parsed[0] == "POST" && parsed[1] == "/User-info")
            {
                counterPost++;

                //write the document back to the client
                std::string strsock;
                strsock = std::to_string(4200);
                std::ostringstream Infostr;
                Infostr << "<h1>" << "Client Port number : " << strsock << "</h1>";
                Infostr << "<h1>" << "Client user Name: " << userName << "</h1>";
                strsock = Infostr.str();
                std::ostringstream oss;
                oss << "HTTP/1.1 " << errorcode << "OK\r\n";
                oss << "Cache-Control: no-cache , private\r\n";
                oss << "Content-Type: " << type << "\r\n";
                oss << "Content-Length: " << strsock.size() << "\r\n";
                //to work on::::::::::::::::::::::::::::::::::::: all
                oss << "\r\n";
                oss << strsock;
                std::string output = oss.str();
                std::cout << "output: \r\n" << output;
                int size = output.size() + 1;
                std::cout << "content: \r\n" << content;
                //sendtoClient(clientSocket, output.c_str(), size);
                asio::write(socket, asio::buffer(output));
            }
        }
        UpdatePostRequests(userName, counterPost, connection, updatePostQuery);
        UpdateGetRequests(userName, counterGet, connection, updateGetQuery);
    }

}

*/