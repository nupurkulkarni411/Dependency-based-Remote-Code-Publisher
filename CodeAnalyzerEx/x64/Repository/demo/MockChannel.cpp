/////////////////////////////////////////////////////////////////////////////
// MockChannel.cpp -                                                       //
// Acts as sender as well as receiver for sending HTTP Messages to server  //
// and receive HTTP messages from server and act according to it           //
// Responsible for creation of mock channel                                //
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

#define IN_DLL
#include "MockChannel.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../HTTPMessage/HTTPMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <thread>
#include <iostream>
#include <future>
#include <windows.h>
#include <ShellAPI.h>

using BQueue = Async::BlockingQueue < Message >;
using namespace Utilities;

/////////////////////////////////////////////////////////////////////////////
// ServerHandler class
// - Helper class to handle receiving of messages sent by the remote server
//
class ServerHandler
{
public:
	ServerHandler(Async::BlockingQueue<std::string>* pBQ) : clientRecvQ_(*pBQ) {}
	void operator()(Socket& socket_);
private:
	bool recvFile(const std::string& filename, size_t fileSize, Socket& socket,const std::string& fromAddr);
	Async::BlockingQueue<std::string>& clientRecvQ_;
	void openRequestedWebPage(const std::string& toAddr,const std::string& filename);
};


// -- functor definiton for handling messages from the server -- //

void ServerHandler::operator()(Socket& socket_)
{
	while (true)
	{
		std::string msg = socket_.recvString();
		if (msg.empty())
			continue;
		HTTPMessage httpMsg;
		httpMsg.parseMessage(msg);
		if (httpMsg.command() == "publish")
		{
			std::cout << ("\n\n  Receiving File : " + httpMsg.name() + "\n");
			for (int i = 0; i < std::stoi(httpMsg.content()); i++)
			{
				msg = socket_.recvString();
				HTTPMessage fTransHttpMsg;
				fTransHttpMsg.parseMessage(msg);
				if (fTransHttpMsg.command() != "file_transfer")
				{
					std::cout << ("\n\n  Check in error : did not receive file\n\n");
					break;
				}
				recvFile(fTransHttpMsg.name(), fTransHttpMsg.getContentLength(), socket_,fTransHttpMsg.toAddress());
			}
		}
		clientRecvQ_.enQ(httpMsg.buildMessage());
		if (httpMsg.command() == "open_home")
			openRequestedWebPage(httpMsg.toAddress(),httpMsg.name());
		if (httpMsg.command() == "quit")
			break;
	}
}

// -- helper function to receive files during the file publish process -- //

bool ServerHandler::recvFile(const std::string& filename, size_t fileSize, Socket& socket, const std::string& toAddr)
{
	std::string fqname;
	fqname = "../ClientDump/" + filename;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	size_t bytesToRead;
	if (!file.isGood())
	{
		while (!(fileSize < BlockSize))
		{
			if (fileSize > BlockSize)
				bytesToRead = BlockSize;
			else
				bytesToRead = fileSize;
			socket.recv(bytesToRead, buffer);
			fileSize -= BlockSize;
		}
		bytesToRead = fileSize;
		socket.recv(bytesToRead, buffer);
		std::cout << ("\n\n  can't open file " + fqname);
		return false;
	}
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}

// -- helper function to open browser for dispaying requested home page -- //

void ServerHandler::openRequestedWebPage(const std::string& toAddr,const std::string& filename)
{
	std::string homeLink;
	homeLink = FileSystem::Path::getFullFileSpec("..//ClientDump//"+filename + ".htm");
	std::wstring wHomeLink = std::wstring(homeLink.begin(), homeLink.end());
	LPCWSTR sw = wHomeLink.c_str();
	ShellExecute(0, 0, sw, 0, 0, SW_SHOW);
}

/////////////////////////////////////////////////////////////////////////////
// Sendr class
// - accepts messages from client for consumption by MockChannel
//
class Sendr : public ISendr
{
public:
	void postMessage(const Message& msg);
	BQueue& queue();
private:
	BQueue sendQ_;
};

void Sendr::postMessage(const Message& msg)
{
	sendQ_.enQ(msg);
}

BQueue& Sendr::queue() { return sendQ_; }

/////////////////////////////////////////////////////////////////////////////
// Recvr class
// - accepts messages from MockChanel for consumption by client
//
class Recvr : public IRecvr
{
public:
	Message getMessage();
	BQueue& queue();
private:
	BQueue recvQ_;
};

Message Recvr::getMessage()
{
	return recvQ_.deQ();
}

BQueue& Recvr::queue() { return recvQ_; }

/////////////////////////////////////////////////////////////////////////////
// MockChannel class
// - reads messages from Sendr and writes messages to Recvr
//
class MockChannel : public IMockChannel
{
public:
	MockChannel(std::string addr, size_t port);
	void postMessage(Message msg);
	Message getMessage();
	void start();
	void stop();
	bool connect(std::string addr, size_t port);
	void manualExecution();
private:
	bool startSender();
	bool startReceiver();
	bool sendProc();
	bool sendFile(const std::string& fileName);
	bool waitForSender();
	void done();
	SocketSystem ss;
	SocketConnecter sc;
	SocketListener sl;
	ServerHandler sh;
	std::string address_;
	size_t port_;
	size_t serverPort_;
	std::vector<std::string> fileList_;
	Async::BlockingQueue<std::string> sendQ_;
	Async::BlockingQueue<std::string> recvQ_;
	std::future<bool> senderDone_;
};

//----< pass pointers to Sender and Receiver >-------------------------------

MockChannel::MockChannel(std::string addr, size_t port) :ss(), address_(addr), port_(port), sl(port), sh(&recvQ_) {}

// -- enqueues a message for sending -- //

void MockChannel::postMessage(Message msg)
{
	sendQ_.enQ(msg);
}

// -- gets a message that was received -- //

Message MockChannel::getMessage()
{
	return recvQ_.deQ();
}

// -- starts the send thread and socket listener -- //

void MockChannel::start()
{
	startReceiver();
	startSender();
}
// -- stops the send thread, socket listener and receive threads -- //

void MockChannel::stop()
{
	done();
	waitForSender();
}

// -- connects the CommChannel sender to the specified foreign IP address and port number -- //

bool MockChannel::connect(std::string addr, size_t port)
{
	int tries = 0;
	while (!sc.connect(addr, port))
	{
		tries++;
		std::cout << "\n client attempt to connect - " << tries << "\n";
		if (tries == 10)
		{
			std::cout << "\n  could not connect\n";
			return false;
		}
		::Sleep(100);
	}
	std::cout << ("\n  client connected to server - [" + addr + "]:" + std::to_string(port) + "\n");
	serverPort_ = port;
	return true;
}

void MockChannel::manualExecution()
{
	HTTPMessage msg;
	msg.command() = "upload";
	msg.fromAddress() = ("[" + address_ + "]:" + std::to_string(port_));
	msg.toAddress() = ("[localhost]:" + std::to_string(serverPort_));
	msg.name() = "demo";
	msg.content() = std::to_string(1);
	int index = 0;
	std::vector<std::string> dep;
	std::vector<std::string> files = FileSystem::Directory::getFiles("..//TestSuit//");
	for (auto i:files)
		dep.push_back(FileSystem::Path::getFullFileSpec("..//TestSuit//" + i));
	msg.setBody(dep);
	msg.setContentLength(dep.size());
	postMessage(msg.buildMessage());
}

// -- starts the client send thread -- //

bool MockChannel::startSender()
{
	senderDone_ = std::async(std::launch::async, [&]()-> bool {return sendProc(); });
	return true;
}

// -- starts the socket listener to receive messages -- //

bool MockChannel::startReceiver()
{
	try
	{
		sl.start(sh);
		return true;
	}
	catch (std::exception ex)
	{
		std::cout << "\n  Exception in startReceiver() caught : " << ex.what() << "\n\n";
		return false;
	}
}

// -- waits for the send thread to finish its processing -- //

bool MockChannel::waitForSender()
{
	return senderDone_.get();
}

// -- enqueues a quit message to terminate the send thread processing and to notify the server that client has finished -- //

void MockChannel::done()
{
	HTTPMessage msg;
	msg.command() = "quit";
	msg.fromAddress() = ("[" + address_ + "]:" + std::to_string(port_));
	msg.toAddress() = ("[localhost]:" + std::to_string(serverPort_));
	msg.name() = "empty";
	msg.content() = "no_content";
	std::vector<std::string> vec = {};
	msg.setBody(vec);
	sendQ_.enQ(msg.buildMessage());
}

// -- send thread processing -- //

bool MockChannel::sendProc()
{
	while (true)
	{
		std::string msg = sendQ_.deQ();
		HTTPMessage hMsg;
		hMsg.parseMessage(msg);
		if (hMsg.command() == "upload" && hMsg.name() != "") //if command is upload what actions to be taken by client
		{
			std::vector<std::string> files_;
			files_ = hMsg.getBody();
			std::string pkg_name;
			
			for (auto file : files_)
			{
				sc.sendString(hMsg.buildMessage());
				std::cout << ("\n\n  Sending message from sendProc - file name : " + file.substr(file.find_last_of('\\')+1) + "\n");
				sendFile(file);
			}
		}
		else
		{
			sc.sendString(hMsg.buildMessage());
		}
		if (hMsg.command() == "quit")
			break;
	}
	return true;
}

// -- Helper function to send files for upload  -- //

bool MockChannel::sendFile(const std::string& fileName)
{
	// assumes that socket is connected

	FileSystem::FileInfo fi(fileName);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fileName);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HTTPMessage msg;
	msg.command() = "file_transfer";
	msg.fromAddress() = ("[" + address_ + "]:" + std::to_string(port_));
	msg.toAddress() = ("[localhost]:" + std::to_string(serverPort_));
	msg.name() = fi.name();
	msg.content() = "size";
	msg.contentLength() = sizeString;
	sc.sendString(msg.buildMessage());
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		sc.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}

//----< factory functions >--------------------------------------------------

IMockChannel* ObjectFactory::createMockChannel(std::string addr, size_t port)
{
	return new MockChannel(addr, port);
}

#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
	ObjectFactory objFact;
	ISendr* pSendr = objFact.createSendr();
	IRecvr* pRecvr = objFact.createRecvr();
	IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
	pMockChannel->start();
	pSendr->postMessage("Hello World");
	pSendr->postMessage("CSE687 - Object Oriented Design");
	Message msg = pRecvr->getMessage();
	std::cout << "\n  received message = \"" << msg << "\"";
	msg = pRecvr->getMessage();
	std::cout << "\n  received message = \"" << msg << "\"";
	pSendr->postMessage("stopping");
	msg = pRecvr->getMessage();
	std::cout << "\n  received message = \"" << msg << "\"";
	pMockChannel->stop();
	pSendr->postMessage("quit");
	std::cin.get();
}
#endif


