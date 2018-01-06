///////////////////////////////////////////////////////////////////////
// HTTPMessage.cpp - Defines HTTP style messages to be used for      //
//					 client-server interaction                       //
// Platform:    Lenovo Ideapad 500, Windows 10, AMD A10 x64          //
// Application: Remote Code Publisher, Project 4                     //
//              CSE687 - Object Oriented Design                      //
// Author:		Nupur Kulkarni, Syracuse University                  //
//				nvkulkar@syr.edu                                     //
// Source:      Jim Fawcett, CST 2-187, Syracuse University          //
//              (315) 443-3948, jfawcett@twcny.rr.com                //
///////////////////////////////////////////////////////////////////////

#include "HTTPMessage.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// -- copy constructor -- //

HTTPMessage::HTTPMessage(const HTTPMessage& message)
{
	msg_ = message.msg_;
	command_ = message.command_;
	fromAddr_ = message.fromAddr_;
	toAddr_ = message.toAddr_;
	name_ = message.name_;
	content_ = message.content_;
	contentLength_ = message.contentLength_;
	body_.clear();
	for (auto line : message.body_)
		body_.push_back(line);
}

// -- assignment operator -- //

HTTPMessage& HTTPMessage::operator=(const HTTPMessage& message)
{
	if (this == &message)
		return *this;
	msg_ = message.msg_;
	command_ = message.command_;
	fromAddr_ = message.fromAddr_;
	toAddr_ = message.toAddr_;
	name_ = message.name_;
	content_ = message.content_;
	contentLength_ = message.contentLength_;
	body_.clear();
	for (auto line : message.body_)
		body_.push_back(line);
	return *this;
}

// -- returms a reference to the command field -- //

std::string& HTTPMessage::command()
{
	return command_;
}

// -- returns a reference to the fromAddress field -- //

std::string& HTTPMessage::fromAddress()
{
	return fromAddr_;
}

// -- returns a reference to the toAddress field -- //

std::string& HTTPMessage::toAddress()
{
	return toAddr_;
}

// -- returns a reference to the name field -- //

std::string& HTTPMessage::name()
{
	return name_;
}

// -- returns a reference to the content field -- //

std::string& HTTPMessage::content()
{
	return content_;
}

// -- returns a reference to the contentLength field -- //

std::string& HTTPMessage::contentLength()
{
	return contentLength_;
}

// -- swaps the to and from address fields of the message -- //

void HTTPMessage::swapAddresses()
{
	std::string temp = fromAddr_;
	fromAddr_ = toAddr_;
	toAddr_ = temp;
}

// -- sets the contentLength to the specified integer value -- //

void HTTPMessage::setContentLength(int len)
{
	contentLength_ = std::to_string(len);
}

// -- returns the contentLength field in unsigned int format -- //

size_t HTTPMessage::getContentLength()
{
	return (size_t)std::stoi(contentLength_);
}

// -- returns the HTTPMessage as a std::string -- //

std::string HTTPMessage::buildMessage()
{
	msg_ = command_ + "\n" + fromAddr_ + "\n" + toAddr_ + "\n" + name_ + "\n" + content_ + "\n" + contentLength_ + "\n" + "\n";
	for (auto line : body_)
		msg_ += line + "\n";
	return msg_;
}

// -- sets the body to the specified vector of strings -- //

void HTTPMessage::setBody(std::vector<std::string> list)
{
	body_.clear();
	for (auto item : list)
		body_.push_back(item);
	setContentLength((int)body_.size());
}

// -- returns the body as a vector of strings -- //

std::vector<std::string> HTTPMessage::getBody()
{
	return body_;
}

// -- parses a std::string into an HTTPMessage -- //

bool HTTPMessage::parseMessage(std::string msg)
{
	std::stringstream msgStream_;
	msgStream_ << msg;
	msgStream_.seekg(0);
	try
	{
		std::string line;
		std::getline(msgStream_, command_);
		std::getline(msgStream_, fromAddr_);
		std::getline(msgStream_, toAddr_);
		std::getline(msgStream_, name_);
		std::getline(msgStream_, content_);
		std::getline(msgStream_, contentLength_);
		std::getline(msgStream_, line);
		for (size_t i = 0; i < getContentLength(); i++)
		{
			body_.push_back("");
			std::getline(msgStream_, body_[i]);
		}
		return true;
	}
	catch (std::exception ex)
	{
		std::cout << "\n\n  Exception caught : " << ex.what() << "\n\n";
		return false;
	}
}

// -- Test Stub -- //

#ifdef TEST_HTTPMESSAGE

int main()
{
	std::cout << "\n  Testing HTTPMessage package\n\n";
	HTTPMessage msg;
	msg.command() = "check_in";
	msg.fromAddress() = "[::1]:8051";
	msg.toAddress() = "[::1]:8080";
	msg.name() = "D:\\MS\\cert";
	msg.content() = "dependencies";
	std::vector<std::string> dependencies{ ".\\TestFile1.h", ".\\TestFile1.cpp", ".\\TestFile2.h", ".\\Testfile2.cpp" };
	msg.setBody(dependencies);
	std::string recvdMsg = msg.buildMessage();
	std::cout << "\n  Showing built message : \n\n";
	std::cout << recvdMsg;
	HTTPMessage rMsg;
	rMsg.parseMessage(recvdMsg);
	std::cout << "\n  Showing parsed message : \n\n";
	std::cout << rMsg.command() + "\n" << rMsg.fromAddress() + "\n" << rMsg.toAddress() + "\n"
		<< rMsg.name() + "\n" << rMsg.content() + "\n" << rMsg.contentLength() + "\n" << "\n";
	std::vector<std::string> msgBody = rMsg.getBody();
	for (auto line : msgBody)
		std::cout << line + "\n";
	std::cout << "\n\n  Done testing\n\n";
	return 0;
}

#endif