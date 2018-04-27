/*
    Copyright 2017, orcaer@yeah.net  All rights reserved.

    Author: orcaer@yeah.net

    Last modified: 2018-4-18

    Description: https://github.com/wlgq2/libuv_cpp11
*/

#include    <cstring>
#include    <iostream>

#include    "EchoServer.h"
#include    "uv/Packet.h"

using namespace uv;
using namespace std;

EchoServer::EchoServer(EventLoop* loop, SocketAddr& addr)
    :TcpServer(loop, addr)
{
    setMessageCallback(std::bind(&EchoServer::newMessage,this,placeholders::_1,placeholders::_2,placeholders::_3));
}

void EchoServer::newMessage(shared_ptr<TcpConnection> connection,const char* buf,ssize_t size)
{
   
#if       0   //直接发送
    connection->write(buf,size,nullptr);
    
#elif     0  //调用write in loop接口
    //实质会直接调用write，并不需要memcpy。
    //writeInLoop需要数据在回调中释放。
    char* data =  new  char [size]();
    memcpy(data, buf, size);
    connection->writeInLoop(data, size,
    [this](WriteInfo& info)
    {
		//write message error.
		if (0 != info.status)
		{
			cout << "Write error ：" << EventLoop::GetErrorMessage(info.status) << endl;
		}
        delete [] info.buf;
    });
#else //包接收及发送
    Packet packet;
    connection->appendToBuffer(buf, size);
    while (0 == connection->readFromBuffer(packet))
    {
        std::cout << packet.getData() << ":" << packet.DataSize() << std::endl;
        connection->write(packet.Buffer(), packet.BufferSize(), nullptr);
    }
#endif
}
