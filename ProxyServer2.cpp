// Proxy2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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

#include "ProxyServer2.hpp"


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

void ProxyServer::Stop()
{
    std::cout << "server stopped";
}
void ProxyServer::finish_with_error(MYSQL* con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

std::vector<std::string>ProxyServer::parseFileType(std::string fileName)
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

void ProxyServer::UpdatePostRequests(std::string Username , int CounterPost, MYSQL* conn , std::string updateQuery)
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

void ProxyServer::UpdateGetRequests(std::string Username, int CounterGet, MYSQL* conn, std::string updateQuery)
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

void ProxyServer::ConnectToDataBase()
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

void ProxyServer::AddToDataBase(std::string username)
{
    std::string addUser;
    addUser = InsertQuery + username;
    std::cout << addUser << "\r\n";
    addUser = addUser + ",0 , 0, false);";
    std::cout << addUser << "\r\n";
    const char* chaddUser = addUser.c_str();
    if (mysql_query(connection, chaddUser))
    {
        finish_with_error(connection);
    }
}

asio::ip::tcp::socket ProxyServer::init()
{
    ConnectToDataBase();
    asio::io_context::work fakework(_asioContext);
    threadContext = std::thread([&]() { _asioContext.run(); });
    asio::ip::tcp::endpoint Server(asio::ip::make_address("127.0.0.1", ErrorCode), 4200);
    asio::ip::tcp::socket socket(_asioContext);
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
    return socket;
}

asio::ip::tcp::socket ProxyServer::onClientConnected()
{
    asio::ip::tcp::socket socket(io_service);
    _asioAcceptor.accept(socket);
    _clientCount++;
    if (_clientCount <= 1)
    {
        std::cout << "Connected to client!" << std::endl;
        std::cout << "Please enter you UserName: ";
        getline(std::cin, userName);
        std::string insertUserName;
        insertUserName = "\"" + userName + "\"";
        userName = "'" + userName + "'";
        std::string finduser;
        finduser = UsersQuery + userName;
        const char* chfinduser = finduser.c_str();
        mysql_query(connection, chfinduser);
        res = mysql_store_result(connection);
        if (mysql_fetch_row(res) == NULL)
        {
            std::cout << "is here" << std::endl;
            AddToDataBase(insertUserName);
        }
        else
        {
            finduser = finduser + AndQuery;
            chfinduser = finduser.c_str();
            mysql_query(connection, chfinduser);
            res = mysql_store_result(connection);
            if (mysql_fetch_row(res) == NULL)
            {
                std::cout << "youre user has been marked! , closing..." << std::endl;
                socket.close();
                _asioAcceptor.close();
                
            }
        }
    }
    IsConnected = true;
    GrabData(socket);
    return socket;
}

void ProxyServer::Run()
{
    asio::ip::tcp::socket socket = init();
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

            _asioContext.stop();
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
        
        asio::ip::tcp::socket clientSocket = onClientConnected();
        std::string charBufTosString(bytesBuffer.begin(), bytesBuffer.end());
        std::istringstream iss(charBufTosString);
        std::vector<std::string> parsed((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
        std::string content = "<h2>404 Not Found</h2>";
        
        if (parsed.size() >= 3 && parsed[0] == "GET")
        {
            counterGet++;
            HtmlFile = parsed[1];
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
        else if (tokens[1] == "js")
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
                oss << "\r\n";
                oss << content;
                std::string output = oss.str();
                int size = output.size() + 1;
                asio::write(clientSocket, asio::buffer(output));
            }
            else if (parsed[0] == "POST" && parsed[1] == "/Upload-a-Picture")
            {
                //std::cout << "pic" << std::endl;
                counterPost++;
                std::string input;
                std::cout << "please enter the name of the picture you would like to upload: ";
                std::getline(std::cin, input);
                input = "\"" + input + "\"";
                std::ostringstream postreq;
                postreq << "<img src=" << input << "/>";
                postreq << "\r\n";
                std::string str_postreq;
                str_postreq = postreq.str();
                //write the document back to the client
                std::ostringstream oss;
                oss << "HTTP/1.1 " << errorcode << "OK\r\n";
                oss << "Cache-Control: no-cache , private\r\n";
                oss << "Content-Type: " << type << "\r\n";
                oss << "Content-Length: " << str_postreq.size() << "\r\n";
                oss << "\r\n";
                oss << str_postreq;
                std::string output = oss.str();
                int size = output.size() + 1;
                asio::write(clientSocket, asio::buffer(output));
            }
            else if (parsed[0] == "POST" && parsed[1] == "/User-info")
            {
                counterPost++;
                //write the document back to the client
                std::string strsock;
                strsock = std::to_string(4100);
                std::ostringstream Infostr;
                Infostr << "<h1>" << "Client Port number : " << strsock << "</h1>";
                Infostr << "<h1>" << "Client user Name: " << userName << "</h1>";
                std::string GetPostQueryy = GetPostQuery + userName;
                GetPostQueryy = GetPostQueryy + ";";
                const char* chGetPostQuery = GetPostQueryy.c_str();
                mysql_free_result(res);
                mysql_query(connection, chGetPostQuery);
                res = mysql_store_result(connection);
                row = mysql_fetch_row(res);
                if (row != NULL)
                {
                    Infostr << "<h1>" << "Client Post Requests : " << row[0] << "</h1>";
                }
                mysql_free_result(res);
                std::string GetgetQueryy = GetGETQuery + userName;
                GetgetQueryy = GetgetQueryy + ";";
                const char* chGetgetQuery = GetgetQueryy.c_str();
                mysql_query(connection, chGetgetQuery);
                res = mysql_store_result(connection);
                row = mysql_fetch_row(res);
                if (row != NULL)
                {
                    Infostr << "<h1>" << "Client Get Requests : " << row[0] << "</h1>";
                }
                strsock = Infostr.str();
                std::ostringstream oss;
                oss << "HTTP/1.1 " << errorcode << "OK\r\n";
                oss << "Cache-Control: no-cache , private\r\n";
                oss << "Content-Type: text/html \r\n";
                oss << "Content-Length: " << strsock.size() << "\r\n";
                oss << "\r\n";
                oss << strsock;
                std::string output = oss.str();
                int size = output.size() + 1;
                asio::write(clientSocket, asio::buffer(output));
            }
            else if (parsed[0] == "POST" && parsed[1] == "/Screenshot")
            {
            keybd_event(VK_MENU, 0, 0, 0);
            keybd_event(VK_SNAPSHOT, 0, 0, 0);

            keybd_event(VK_SNAPSHOT, 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);

            std::ostringstream oss;
            oss << "<h1> Screenshot taken! </h1>";
            std::string strsock = oss.str();
            std::ostringstream osss;
            osss << "HTTP/1.1 " << errorcode << "OK\r\n";
            osss << "Cache-Control: no-cache , private\r\n";
            osss << "Content-Type: text/html \r\n";
            osss << "Content-Length: " << strsock.size() << "\r\n";
            osss << "\r\n";
            osss << strsock;
            std::string output = osss.str();
            asio::write(clientSocket, asio::buffer(output));

            }
            else if (parsed[0] == "POST" && parsed[1] == "/Statistics")
            {
                counterPost++;
                std::ostringstream osss;
                osss << "<style>\n table, tr , td {\n\t border:1px solid black;\n }\n </style>\n";
                osss << "<table>\r\n\t";
                osss << "<tr>\n\t\t";
                osss << "<td>\n\t\t\t";
                osss << "<h1>User Name :</h1>\n\t\t\t";
                osss << "<h1>Post Requests :</h1>\n\t\t\t";
                osss << "<h1>Get Requests :</h1>\n\t\t\t";
                osss << "<h1>IsMarked :</h1>\n\t\t\t";
                osss << "\n\t\t</td>\n\t\t\t";
                mysql_free_result(res);
                const char* chAllQuery = AllQuery.c_str();
                mysql_query(connection, chAllQuery);
                res = mysql_store_result(connection);
                while (row = mysql_fetch_row(res))
                {
                    osss << "\n\t\t<td>\n\t\t\t";
                    osss << "<h1>" << row[0] <<  "</h1>" << "\n\t\t";
                    osss << "<h1>" << row[1] << "</h1>" << "\n\t\t";
                    osss << "<h1>" << row[2] << "</h1>" << "\n\t\t";
                    osss << "<h1>" << row[3] << "</h1>" << "\n\t\t";
                    osss << "\n\t\t</td>\n\t\t\t";
                }
                osss << "\n\t\t</td>\n";
                osss << "\t</tr>\n";
                osss << "</table>\n";
                std::string strsock;
                strsock = osss.str();
                std::ostringstream oss;
                oss << "HTTP/1.1 " << errorcode << "OK\r\n";
                oss << "Cache-Control: no-cache , private\r\n";
                oss << "Content-Type: text/html \r\n";
                oss << "Content-Length: " << strsock.size() << "\r\n";
                oss << "\r\n";
                oss << strsock;
                std::string output = oss.str();
                int size = output.size() + 1;
                asio::write(clientSocket, asio::buffer(output));
            }
            
        }
        UpdatePostRequests(userName, counterPost, connection, updatePostQuery);
        UpdateGetRequests(userName, counterGet, connection, updateGetQuery);
    }

}

