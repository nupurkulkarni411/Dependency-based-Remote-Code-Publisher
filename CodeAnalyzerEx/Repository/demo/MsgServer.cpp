///////////////////////////////////////////////////////////////////////
// RemoteRepository.cpp - Server that implements the remote			 //
//							repository functionality				 //
// Platform:    Lenovo Ideapad 500, Windows 10, AMD A10 x64			 //
// Application: Remote Code Publisher, Project 4					 //
//              CSE687 - Object Oriented Design						 //
// Author:		Nupur Kulkarni, Syracuse University					 //
//				nvkulkar@syr.edu									 //
// Source:      Jim Fawcett, CST 2-187, Syracuse University		     //
//              (315) 443-3948, jfawcett@twcny.rr.com				 //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a RepositoryServer class that implements all the
* functionalities required of the remote code publisher server, as
* described in the requirements and purpose section.
*
* This package also has a ClientHandler class, which is a helper class
* for the server. It handles receiving of messages sent by remote clients.
*
*
* Public Interface:
* --------------------
* startSender()					: starts the server send processing to reply to client requests
* startListener()				: starts the server socket Listener to receive client requests
* startServerProcessing()		: starts the processing of received requests
* closeServer()					: shuts the server down
*
* Build Process:
* --------------
* Required Files:
*   BlockingQueue.h, BlockingQueue.cpp, Utilities.h, Utilities.cpp
*   Sockets.h, Sockets.cpp, Logger.h, Logger.cpp,
*   HTTPMessage.h, HTTPMessage.cpp, FileMgr.h, IFileMgr.h,
*   FileSystem.h, FileSystem.cpp
*
* Build Command: devenv RemoteRepository.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 30 Apr 2016
* - first release
*/


#include "../Sockets/Sockets.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../FileSystem/FileSystem.h"
#include "../HTTPMessage/HTTPMessage.h"
#include <unordered_map>
#include <future>
#include <string>
#include <iostream>
#include <algorithm>
#include "../TypeAnalysis/TypeAnal.h"
#include "../DependancyAnalysis/DepAnal.h"
#include "../COnvertToHtml/ConvertToHtml.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Analyzer/Executive.h"

using Show = Logging::StaticLogger<1>;
using namespace Utilities;
using namespace CodeAnalysis;
using namespace Async;

/////////////////////////////////////////////////////////////////////////////
// ClientHandler class
// - Helper class to handle receiving of messages sent by remote clients
//
class ClientHandler
{
public:
	ClientHandler(BlockingQueue<std::string>* pBQ) : serverRecvQ(*pBQ) {}
	void operator()(Socket& socket_);
private:
	bool recvFile(const std::string& folder, const std::string& filename, size_t fileSize, Socket& socket);
	BlockingQueue<std::string>& serverRecvQ;
};

// -- functor definiton for handling messages from clients -- //

void ClientHandler::operator()(Socket& socket_)
{
	while (true)
	{
		std::string msg = socket_.recvString();
		if (msg.empty())
			continue;
		HTTPMessage httpMsg;
		httpMsg.parseMessage(msg);
		std::cout << ("\n  server received message : " + httpMsg.command() + "  from client : " + httpMsg.fromAddress() + "\n");
		if (httpMsg.command() == "upload")
		{
			for (int i = 0; i < std::stoi(httpMsg.content()); i++)
			{
				msg = socket_.recvString();
				HTTPMessage fTransHttpMsg;
				fTransHttpMsg.parseMessage(msg);
				std::string tempDir = "../Repository/" + httpMsg.name();
				FileSystem::Directory::create(tempDir);
				if (fTransHttpMsg.command() != "file_transfer")
				{
					Show::write("\n\n  Check in error : did not receive file\n\n");
					break;
				}
				recvFile(httpMsg.name(), fTransHttpMsg.name(), fTransHttpMsg.getContentLength(), socket_);
			}
		}
		serverRecvQ.enQ(httpMsg.buildMessage());
		if (httpMsg.command() == "quit")
			break;
	}
}
// -- helper function to receive files during the file publish process -- //

bool ClientHandler::recvFile(const std::string& folder, const std::string& filename, size_t fileSize, Socket& socket)
{
	std::string fqname = "../Repository/" + folder + "/" + filename;
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
		Show::write("\n\n  can't open file " + fqname);
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
///////////////////////////////////////////////////////////////////////////////////
// RepositoryServer class
// - Helper class for handling client's requests and sending appropriate responses
//
class RepositoryServer
{
public:
	using File = std::string;
	using Files = std::vector<File>;
	RepositoryServer(std::string addr, size_t port);
	bool startListener();
	void startServerProcessing();
	bool startSender();
	bool closeServer();
private:
	HTTPMessage getMessage();
	void postMessage(std::string msg);
	bool connect(std::string addr, size_t port, SocketConnecter& sc);
	void doPublishWithDep(std::vector<std::string> visited, std::string fileName, std::string toAddr);
	std::vector<std::string> getDependencies(std::string pkgName, std::string filename);
	bool serverProc();
	bool sendProc();
	bool sendFile(const std::string& fileName, SocketConnecter& sc, std::string fromAddr, std::string toAddr);
	bool closeSender_ = false;
	bool closeServer_ = false;
	SocketSystem ss;
	SocketListener sl;
	std::string address_;
	size_t port_;
	std::vector<std::string> fileList_;
	BlockingQueue<std::string> sendQ_;
	BlockingQueue<std::string> recvQ_;
	std::unordered_map<std::string, SocketConnecter> connections_;
	std::future<bool> senderDone_;
	std::future<bool> serverDone_;
	ClientHandler ch;
	void generateHTML(HTTPMessage);
};


// -- reads file and returns file names of dependant files -- //

std::vector<std::string> RepositoryServer::getDependencies(std::string pkgName,std::string filename)
{
	std::string htmPath = FileSystem::Path::getFullFileSpec("../Repository/" + pkgName + "/" + filename + ".htm");
	std::vector<std::string> result;
	try
	{
		std::ifstream in(htmPath);
		std::string line,file;
		while (in.good()) {
			std::getline(in, line);
			file = file + line + "\n";
		}
		file = file.substr(file.find("Dependencies:"));
		file = file.substr(0, file.find("</div>"));
		while (file.find("=") != std::string::npos)
		{
			std::string file1 = file;
			std::string filename = file.substr(file.find("=") + 2);
			std::string filename1;
			for (int i = 0; filename[i] != '>'; i++)
				filename1 = filename1 + filename[i];
			result.push_back(filename1);
			file = file1.substr(file1.find(("</a>")) + 1);
		}
		return result;
	}
	catch (std::exception ex)
	{
		std::cout << "\n\n  **  Could not read file to send, exception caught : " << ex.what();
		return result;
	}
}

// -- Construct the server with the specified IP address and port number -- //

RepositoryServer::RepositoryServer(std::string addr, size_t port) : ss(), address_(addr), port_(port), sl(port), ch(&recvQ_) {}

bool RepositoryServer::connect(std::string addr, size_t port, SocketConnecter& sc)
{
	int tries = 0;
	while (!sc.connect(addr, port))
	{
		tries++;
		std::cout << "\n server attempt to connect - " << tries << "\n";
		if (tries == 10)
		{
			std::cout << "\n  could not connect\n";
			return false;
		}
		::Sleep(100);
	}
	std::cout << "\n  connected to client - [" << addr << "]:" << port << "  \n";
	return true;
}

// -- start the server listener to accept client requests -- //

bool RepositoryServer::startListener()
{
	sl.start(ch);
	return true;
}

// -- dequeue a received messagge for processing -- //

HTTPMessage RepositoryServer::getMessage()
{
	HTTPMessage httpMsg;
	std::string msg = recvQ_.deQ();
	httpMsg.parseMessage(msg);
	return httpMsg;
}

// -- enqueue a message for sending -- //

void RepositoryServer::postMessage(std::string msg)
{
	HTTPMessage hmsg;
	hmsg.parseMessage(msg);
	sendQ_.enQ(msg);
}

// -- start the send thread of the server -- //

bool RepositoryServer::startSender()
{
	senderDone_ = std::async(std::launch::async, [&]()-> bool {return sendProc(); });
	return true;
}

// -- helper function to send files for publish request -- //

bool RepositoryServer::sendFile(const std::string& fileName, SocketConnecter& sc, std::string fromAddr, std::string toAddr)
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
	msg.fromAddress() = fromAddr;
	msg.toAddress() = toAddr;
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

// -- helper function for generating HTML pages of files which are present in repository -- //

void RepositoryServer::generateHTML(HTTPMessage hMsg)
{
	std::string dfpath = FileSystem::Path::getFullFileSpec("../Repository");
	std::vector<std::string> patterns = { "*.h","*.cpp" };
	CodeAnalysisExecutive exec;
	exec.setPath(dfpath);
	exec.setPattern(patterns);
	exec.setDisplayModes();
	exec.getSourceFiles();
	exec.processSourceCode(true);
	TypeAnal ta;
	ta.doTypeAnal();   //type analysis
	DepAnal depAnal;
	depAnal.ProcessCmdLine(patterns,dfpath);
	depAnal.getSourceFiles();
	depAnal.processSourceCode();    // dependancy analysis
	for(auto dir: FileSystem::Directory::getDirectories(dfpath))
	{ 
		std::string Intpath = FileSystem::Path::getFullFileSpec(dfpath + "//" + dir);
		for (auto file : FileSystem::Directory::getFiles(Intpath, "*.h"))
		{
			ConvertToHtml cth;
			file = FileSystem::Path::getFullFileSpec(Intpath + "//" + file);
			cth.SetFileName(file);
			cth.SetDestDirName(Intpath);
			cth.InsertPrologue();
			cth.FileInitialization();
			cth.EnterCode(file);
			cth.FileClose();
		}
		for (auto file : FileSystem::Directory::getFiles(Intpath, "*.cpp"))
		{
			ConvertToHtml cth1;
			file = FileSystem::Path::getFullFileSpec(Intpath + "//" + file);
			cth1.SetFileName(file);
			cth1.SetDestDirName(Intpath);
			cth1.InsertPrologue();
			cth1.FileInitialization();
			cth1.EnterCode(file);
			cth1.FileClose();
		}
	}
}

// -- defiens the processing of the send thread -- //

bool RepositoryServer::sendProc()
{
	while (!closeSender_)
	{
		HTTPMessage hMsg;
		std::string msg = sendQ_.deQ();
		hMsg.parseMessage(msg);
		if (hMsg.command() == "close")
			continue;
		std::string toAddr = hMsg.toAddress();
		if (connections_.find(toAddr) == connections_.end())
		{
			std::string ip = toAddr.substr(toAddr.find("[") + 1, toAddr.find("]") - toAddr.find("[") - 1);
			size_t port = std::stoi(toAddr.substr(toAddr.find("]") + 2));
			SocketConnecter sc;
			connections_.insert({ toAddr, std::move(sc) });
			connect(ip, port, connections_.find(toAddr)->second);
		}
		if (hMsg.command() == "publish")
		{
			generateHTML(hMsg);
			std::string filePath;
			std::string dirPath;
			std::vector<std::string> dirs = FileSystem::Directory::getDirectories("..//Repository");
			for (auto dir : dirs){
				if (dir != "." && dir != "..")
				{
					std::vector<std::string> file_list = FileSystem::Directory::getFiles("..//Repository//" + dir);
					for (auto f : file_list) {
						if (f == hMsg.name()){
							filePath = "..//Repository//" + dir + "/" + f;
							dirPath = "..//Repository//" + dir;
						}
					}
				}
			}
			hMsg.content() = std::to_string(1);
			connections_.find(toAddr)->second.sendString(hMsg.buildMessage());
			sendFile(filePath, connections_.find(toAddr)->second, hMsg.fromAddress(), hMsg.toAddress());
		}
		else
			connections_.find(toAddr)->second.sendString(hMsg.buildMessage());
		std::cout << "\n  server sent msg : " << hMsg.command() << "  to client : " << hMsg.toAddress() << "\n";
		if (hMsg.command() == "quit")
		{
			std::cout << "\n\n  server deleting socket connector to client : " << hMsg.toAddress() << "\n";
			connections_.erase(toAddr);
		}
	}
	return true;
}


// -- close the send thread and shut down the server processing -- //

bool RepositoryServer::closeServer()
{
	closeSender_ = true;
	if (sendQ_.size() == 0)
	{
		HTTPMessage closeMsg;
		closeMsg.command() = "close";
		closeMsg.contentLength() = "0";
		sendQ_.enQ(closeMsg.buildMessage());
	}
	senderDone_.get();
	std::cout << "\n\n  Sender closed\n";
	closeServer_ = true;
	if (recvQ_.size() == 0)
	{
		HTTPMessage closeMsg;
		closeMsg.command() = "close";
		closeMsg.contentLength() = "0";
		recvQ_.enQ(closeMsg.buildMessage());
	}
	std::cout << "\n\n  Server closed\n\n";
	return serverDone_.get();
}

// -- helper function to handle publish request and suppors functionality of lazy loading -- //

void RepositoryServer::doPublishWithDep(std::vector<std::string> visited_, std::string fileName, std::string toAddr)
{
	bool visit = false; std::string originalFile = fileName;
	std::string pkgName; HTTPMessage httpMsg; std::list<std::string> queue;
	visited_.push_back(fileName); queue.push_back(fileName); std::list<std::string>::iterator i;
	while (!queue.empty()) {
		fileName = queue.front();
		httpMsg.command() = "publish";
		httpMsg.fromAddress() = "[localhost]:" + std::to_string(port_);
		httpMsg.toAddress() = toAddr;
		httpMsg.name() = fileName + ".htm";
		httpMsg.contentLength() = "0";
		postMessage(httpMsg.buildMessage());
		queue.pop_front();
		std::vector<std::string> dirs = FileSystem::Directory::getDirectories("..//Repository");
		for (auto dir : dirs) {
			if (dir != "." && dir != "..") {
				std::vector<std::string> file_list = FileSystem::Directory::getFiles("..//Repository//" + dir);
				for (auto f : file_list) {
					if (f == fileName) {
						pkgName = dir; break;
					}
				}
			}
		}
		std::vector<std::string> deps = getDependencies(pkgName, fileName);
		for (auto dep : deps) {
			visit = false;
			for (auto iter = visited_.begin(); iter != visited_.end(); iter++){
				if ((*iter) == dep.substr(0, dep.find_last_of(".")))
					visit = true;
			}
			if (visit == false) {
				visited_.push_back(dep.substr(0,dep.find_last_of(".")));
				queue.push_back(dep.substr(0, dep.find_last_of(".")));
			}
		}
	}
	httpMsg.command() = "publish";
	httpMsg.fromAddress() = "[localhost]:" + std::to_string(port_); httpMsg.toAddress() = toAddr;
	httpMsg.contentLength() = "0";
	httpMsg.name() = "CSSFile.css"; // sending CSS file
	postMessage(httpMsg.buildMessage());
	httpMsg.name() = "JSFile.js";  //sending javascript file
	postMessage(httpMsg.buildMessage());
	httpMsg.command() = "open_home";
	httpMsg.name() = originalFile;  //done with file transfer so open the requested page in browser
	postMessage(httpMsg.buildMessage());
	httpMsg.command() = "published";  //done with publishing the file
	postMessage(httpMsg.buildMessage());
}
// -- start processing client requests -- //

void RepositoryServer::startServerProcessing()
{
	serverDone_ = std::async(std::launch::async, [&]()-> bool {return serverProc(); });
}

// -- defines processing to handle client requests -- //

bool RepositoryServer::serverProc()
{
	int count_files = 0;
	while (!closeServer_) {
		HTTPMessage httpMsg; httpMsg = getMessage();
		if (httpMsg.command() == "upload") {
			std::cout << ("\n  server processing upload of File : " + httpMsg.getBody()[count_files].substr(httpMsg.getBody()[count_files].find_last_of('\\') + 1) + "  for client : " + httpMsg.fromAddress() + "\n");
			count_files++; if (count_files >= httpMsg.getContentLength()) { count_files = 0; }
		}
		else if (httpMsg.command() == "get_package_list") {
			std::cout << ("\n  server processing get_package_list request for client : " + httpMsg.fromAddress() + "\n");
			std::vector<std::string> dirs = FileSystem::Directory::getDirectories("..//Repository");
			std::vector<std::string> packages;
			for (auto dir : dirs) {
				if (dir != "." && dir != "..")
					packages.push_back(dir); }
			httpMsg.command() = "package_list"; httpMsg.content() = "packages"; httpMsg.setBody(packages);}
		else if (httpMsg.command() == "get_file_list") {
			std::cout << ("\n  server processing get_file_list request for client : " + httpMsg.fromAddress() + "\n");
			std::vector<std::string> dirs = FileSystem::Directory::getDirectories("..//Repository");std::vector<std::string> packages;
			for (auto dir : dirs) {
				if (dir != "." && dir != "..") {
					packages.push_back("<> " + dir);
					std::vector<std::string> file_list = FileSystem::Directory::getFiles("..//Repository//" + dir);
					for (auto f : file_list) {
						if (f.find(".css") == std::string::npos && f.find(".js") == std::string::npos) {
								packages.push_back(f);
						}}}}httpMsg.command() = "file_list"; httpMsg.content() = "files"; httpMsg.setBody(packages);}
		else if (httpMsg.command() == "delete_file") {
			std::cout << ("\n  server processing delete_file request for client : " + httpMsg.fromAddress() + "\n");
			std::vector<std::string> dirs = FileSystem::Directory::getDirectories("..//Repository");
			for (auto dir : dirs) {
				if (dir != "." && dir != "..") {
					std::vector<std::string> file_list = FileSystem::Directory::getFiles("..//Repository//" + dir);
					for (auto f : file_list) {
						if (f == httpMsg.name())
							FileSystem::File::remove(FileSystem::Path::getFullFileSpec("..//Repository//" + dir + "//" + f));
					}}}httpMsg.command() = "file_deleted"; httpMsg.content() = "delete";}
		else if (httpMsg.command() == "open_web_page") {
			std::cout << ("\n  server processing get_package_list request for client : " + httpMsg.fromAddress() + "\n");
			std::vector<std::string> visited{}; doPublishWithDep(visited, httpMsg.name().substr(0,httpMsg.name().find_last_of(".")), httpMsg.fromAddress());
			continue;}
		else if (httpMsg.command() == "publish") {
			generateHTML(httpMsg);
			std::cout << ("\n  server processing file publish : " + httpMsg.name() + "  for client : " + httpMsg.fromAddress() + "\n");
			std::vector<std::string> visited{}; doPublishWithDep(visited,httpMsg.name(), httpMsg.fromAddress());
			continue;
		}
		httpMsg.swapAddresses(); postMessage(httpMsg.buildMessage());
	}return true;
}


int main(int argc, char* argv[])
{
	::SetConsoleTitle(L"Repository Server");
	std::cout<<"\n  Remote Repository Server started on port : ";
	try
	{
		std::cout << std::stoi(argv[1]);
		RepositoryServer server("localhost", std::stoi(argv[1]));
		server.startSender();
		server.startListener();
		server.startServerProcessing();
		Show::write("\n --------------------\n  press enter key to exit: \n --------------------");
		std::cout.flush();
		std::cin.get();
		server.closeServer();
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}