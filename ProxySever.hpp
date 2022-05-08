/*
#pragma once

#include "netCommon.hpp"
#include "mysql.h"

class ProxyServer
{
public:
	ProxyServer(int port)
		: _asioAcceptor(_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	{

	}

	virtual ~ProxyServer()
	{
		Stop();
	}
	bool Start();
	void Stop();
	void WaitForClientConnection();
	void MessageClient(int socket , const char *msg , int legnth);
	void MessageAllClients();
	void update();
	void finish_with_error(MYSQL* connection);
	std::vector<std::string> parseFileType(std::string fileName);
	void UpdatePostRequests(std::string Username, int CounterPost, MYSQL* conn, std::string updateQuery);
	void UpdateGetRequests(std::string Username, int CounterGet, MYSQL* conn, std::string updateQuery);
	void WaitForClientConnection();
	void MessageClient(int socket, const char* msg, int legnth);
	void MessageAllClients();
	void update();
	void ConnectToDataBase();
	void init();
protected:
	virtual bool onClientConnect();
	virtual void OnClientDisconnect();
	virtual void OnMessage(int socket, const char* msg, int legnth);
	
private:
	int _port;
	int _socket;
	asio::io_context _asioContext;
	std::thread _threadContext;
	asio::ip::tcp::acceptor _asioAcceptor;
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
	std::string InsertQuery = "INSERT INTO Userr(name, PostRequests,  GetRequests, ISMarked) VALUES(";
	bool IsConnected = false;
	bool sent = false;
	std::string strF;
	asio::streambuf buf;
	std::vector<std::string> tokens;
};

*/