/////////////////////////////////////////////////////////////////////
// TestExecutive.cpp - Directs Remote Code Publisher               //
// ver 1.4                                                         //
//-----------------------------------------------------------------//
// Jim Fawcett (c) copyright 2016                                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Dell XPS 8900, Windows 10                          //
// Application: Project #2, CSE687 - Object Oriented Design, S2015 //
// Author:      Nupur Kulkarni Syracuse University				   //
//              nvkulkar@syr.edu								   //
// Source:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
/////////////////////////////////////////////////////////////////////


/*
* Package Operations:
* -------------------
* This class demonstrates Remote Code Repository.
*
* Required Files:
* ---------------
*   - FileSystem.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 27 Feb 2017
* - demonstrates the requirements.
*/

#include <string>
#include <vector>
#include <iostream>
#include "../FileSystem/FileSystem.h"

int main() {

	std::cout << "\n\n-----------------------------Requirement 1-------------------------------------\n\n";
	std::cout << "   Used Visual Studio 2015 and its C++ Windows console projects, as provided in the ECS computer labs. \n";
	std::cout << "   Used Windows Presentation Foundation (WPF) to provide a required client Graphical User Interface (GUI).\n";
	std::cout << "\n\n-----------------------------Requirement 2-------------------------------------\n\n";
	std::cout << "   Used the C++ standard library's streams for all console I/O and new and delete for all heap-based memory management.";
	std::cout << "\n\n-----------------------------Requirement 3,4-------------------------------------\n\n";
	std::cout << "   Contains program RepositoryClass inside MsgServer package which supports publishing of";
	std::cout << "\n   C++ source code files (according to what I did in project 3) as a set of linked web pages.";
	std::cout << "\n\n-----------------------------Requirement 5-------------------------------------\n\n";
	std::cout << "   Showed on client and server consols.";
	std::cout << "\n\n-----------------------------Requirement 6-------------------------------------\n\n";
	std::cout << "   Showing message passing through sockets  a message-passing communication system, \n   based on Sockets, used to access the Repository's functionality from another process or machine\n";
	std::cout << "   using 2 clients and 1 server.";
	std::cout << "\n\n-----------------------------Requirement 7-------------------------------------\n\n";
	std::cout << "   The communication system provides support for passing HTTP style messages";
	std::cout << "\n\n   Sample HTTP Message:\n";
	std::cout << "   msg.command() = get_file_list \n";
	std::cout << "   msg.fromAddress() = [localhost]: + toStdString(ClientPortNumber)\n";
	std::cout << "   msg.toAddress() = [localhost]: + toStdString(ServerPortNumber)) \n";
	std::cout << "   msg.name() = empty";
	std::cout << "   msg.content() = no_content";
	std::cout << "   postMessage(msg.buildMessage())";
	std::cout << "\n\n-----------------------------Requirement 8-------------------------------------\n\n";
	std::cout << "   Communication channel supports sending and receiving of streams of bytes.\n";
	std::cout << "   For sending and receiving streans of bytes used following: \n";
	std::cout << "   file.open(FileSystem::File::out, FileSystem::File::binary);\n";
	std::cout << "   const size_t BlockSize = 2048;\n   Socket::byte buffer[BlockSize];\n";
	std::cout << "   streams are getting established with initial exchanges of messages as we can see on client and server consoles.";
	std::cout << "\n\n-----------------------------Requirement 10-------------------------------------\n\n";
	std::cout << "   Implemented Lazy loading strategy: when client askes for publishing of the file \n";
	std::cout << "   Requested html file and all the dependant files will be sent to client along with css and javascript files.";
	return 0;
}