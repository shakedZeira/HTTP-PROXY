#pragma once
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
#include "netCommon.hpp"

class ProxyServer
{

	public:
		
		ProxyServer(int port)
			: _asioAcceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
		{

		}
		virtual ~ProxyServer()
		{
			Stop();
		}
		void Run();
		void Stop();
		void finish_with_error(MYSQL* connection);
		std::vector<std::string> parseFileType(std::string fileName);
		void UpdatePostRequests(std::string Username, int CounterPost, MYSQL* conn, std::string updateQuery);
		void UpdateGetRequests(std::string Username, int CounterGet, MYSQL* conn, std::string updateQuery);
		void ConnectToDataBase();
		void AddToDataBase(std::string username);
		std::string checkIfOverTheLimit();
		asio::ip::tcp::socket init();

	protected:
		virtual asio::ip::tcp::socket onClientConnected();
		virtual void GrabData(asio::ip::tcp::socket & socket);

	private:
		int _port;
		int _socket;
		int errorcode = 404;
		asio::io_service io_service;
		std::thread threadContext;
		int _clientCount = 0;
		int counterGet = 0;
		int counterPost = 0;
		MYSQL* connection;
		MYSQL_ROW row;
		MYSQL_RES* res;
		asio::error_code ErrorCode;
		std::string userName;
		std::string UsersQuery = "SELECT name FROM Userr WHERE name = ";
		std::string updatePostQuery = "UPDATE Userr SET PostRequests = PostRequests + 1 WHERE name = ";
		std::string updateGetQuery = "UPDATE Userr SET GetRequests = GetRequests + 1 WHERE NAME = ";
		std::string InsertQuery = "INSERT INTO Userr(name,PostRequests,GetRequests,ISMarked) VALUES(";
		std::string GetPostQuery = "SELECT PostRequests FROM Userr WHERE name = ";
		std::string GetGETQuery = "SELECT GetRequests FROM Userr WHERE name = ";
		std::string AndQuery = "AND IsMarked = 0";
		std::string AllQuery = "SELECT * FROM Userr;";
		bool IsConnected = false;
		bool sent = false;
		std::string strF;
		asio::streambuf buf;
		std::vector<std::string> tokens;
		//asio::ip::tcp::socket socket;
		std::thread _threadContext;
		asio::ip::tcp::acceptor _asioAcceptor;
		asio::io_context _asioContext;
		

	

};