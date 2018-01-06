#ifndef HTTPMESSAGE_H
#define HTTPMESSAGE_H

///////////////////////////////////////////////////////////////////////
// HTTPMessage.h - Defines HTTP style messages to be used for		 //
//					client-server interaction						 //
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
* This package provides construction or parsing of HTTP style messages.
*
*
* Public Interface:
* --------------------
* buildMessage()	: returns the HTTPMessage as a std::string
* parseMessage()	: parses a std::string into an HTTPMessage
* swapAdresses()	: swaps the to and from address fields of the message
*
* The other methods are getters and setters for the various message fields,
* as named
*
* Build Process:
* --------------
* Required Files:
*   HTTPMessage.h, HTTPMessage.cpp
*
* Build Command: devenv RemoteRepository.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 25 Apr 2016
* - first release
*/


#include <string>
#include <vector>
#include <sstream>

class HTTPMessage
{
public:
	HTTPMessage() {}
	HTTPMessage(const HTTPMessage& message);
	HTTPMessage& operator=(const HTTPMessage& message);
	std::string& command();
	std::string& fromAddress();
	std::string& toAddress();
	std::string& name();
	std::string& content();
	std::string& contentLength();
	void setContentLength(int len);
	size_t getContentLength();
	void swapAddresses();
	std::string buildMessage();
	void setBody(std::vector<std::string> list);
	std::vector<std::string> getBody();
	bool parseMessage(std::string msg);
private:
	std::string msg_;
	std::string command_;
	std::string fromAddr_;
	std::string toAddr_;
	std::string name_;
	std::string content_;
	std::string contentLength_;
	std::vector<std::string> body_;
};

#endif