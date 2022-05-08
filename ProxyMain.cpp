#include "ProxyServer2.hpp"

void main()
{
	ProxyServer proxyserver(4100);
	//proxyserver.init();
	proxyserver.Run();
	system("pause");

	
}