#ifndef MOCKCHANNEL_H
#define MOCKCHANNEL_H
/////////////////////////////////////////////////////////////////////////////
// MockChannel.h - Demo for CSE687 Project #4, Spring 2015                 //
// - build as DLL to show how C++\CLI client can use native code channel   //
// - MockChannel reads from sendQ and writes to recvQ                      //
// ver 1.0                                                                 //
// Language:    C++, Visual Studio 2015                                    //
// Platform:    Lenovo Ideapad 500, Windows 10, AMD A10 x64			       //
// Application: Remote Code Publisher, Project 4					       //
//              CSE687 - Object Oriented Design						       //
// Author:		Nupur Kulkarni, Syracuse University					       //
//				nvkulkar@syr.edu									       //
// Source:      Jim Fawcett, CST 2-187, Syracuse University		           //
//              (315) 443-3948, jfawcett@twcny.rr.com				       //
/////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package defines an IMockChannel interface, that the MockChannel
* class implements. This Communication Channel can be used to
* interact with the remote code publisher server. It accepts and returns
* HTTPMessages in string format, which will be parsed to get the required
* information. Sending and receiving of files and messages is done via
* SocketConnectors and Listeners that run on child threads.
*
* An Object factory is defined, which is used to get a pointer to the
* IMockChannel interface.
*
* This package also provides a ServerHandler class, which is a helper class
* for the client. It handles receiving of messages sent by the remote server
*
*
* Public Interface:
* --------------------
* createMockChannel(std::string addr, size_t port)		: returns a pointer to an instance of IMockChannel
* start()												: starts the send thread and socket listener
* stop()												: stops the send thread, socket listener and receive threads
* connect(std::string addr, size_t port)				: connects the mock channel to the specified address and port
* postMessage(HTTPMessage hMsg)							: enqueues a message for sending
* getMessage()											: dequeues a message that was received
*
*
* Build Process:
* --------------
* Required Files:
*   MockChannel.h, MockChannel.cpp
*   BlockingQueue.h, BlockingQueue.cpp, Utilities.h, Utilities.cpp
*   Sockets.h, Sockets.cpp, FileSystem.h, FileSystem.cpp
*   HTTPMessage.h, HTTPMessage.cpp, FileMgr.h, IFileMgr.h,
*
* Build Command: devenv RemoteRepository.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 30 Apr 2016
* - first release
*/

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif

#include <string>
using Message = std::string;

struct ISendr
{
	virtual void postMessage(const Message& msg) = 0;
};

struct IRecvr
{
	virtual std::string getMessage() = 0;
};

struct IMockChannel
{
public:
	virtual void postMessage(Message msg) = 0;
	virtual Message getMessage() = 0;
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual bool connect(std::string addr, size_t port) = 0;
	virtual void manualExecution(std::string testsuit) = 0;
};

extern "C" {
	struct ObjectFactory
	{
		DLL_DECL IMockChannel* createMockChannel(std::string addr, size_t port);
	};
}

#endif


