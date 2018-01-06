#pragma once
#ifndef CONVERTTOHTML_H
#define CONVERTTOHTML_H
///////////////////////////////////////////////////////////////////////////////
// ConvertToHtml.h -   this class converts C++ source file to HTML file      //
//                     using CSS for styling and JavaScript for expanding    //
//                     and collapsing classes,functions and global functions //
// ver 3.3                                                                   //
// Language:    C++, Visual Studio 2015                                      //
// Application: Dependency-Based CodePublisher,                              //
// CSE687 -     Object Oriented Design                                       //
// Author:      Nupur Kulkarni, Syracuse University,                         //
//              nvkulkar@syr.edu                                             //
///////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a ConvertToHtml class.
* ConvertToHtml takes source C++ files as input and publishes those files into HTML format
* 
*

Public Interface:
=================
void SetFileName(std::string FileName) //sets the file name of the file which is currently being processed 
void FileInitialization(); //Generates HTML head part containing all references to CSS and JavaScript files 
                             and dependancies which are included in html file.
void EnterCode(std::string FileName); //process source code and generates corresponding HTML code
void FileClose(); //Ends HTML file.
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to); // Any markup
                       character which is to be escaped will be replaced by appropriate exit code.
void CreateJS(); // creates JavaScript file
void CreateCSS(); // creates CSS file.
bool FileExists(const std::string& name); // this function checks if css and javascript files 
                                             are already created and prevents creation of files every time.
bool CheckForClassFunGlfn(std::string line, std::string line1); //this function checks if current token 
                                        is class function or global function used for expand and collapse.
void CreateHomePage(std::vector<std::string> files); //creates home page listing all files and dependancies
void SetDestDirName(std::string destDirName); //sets destination folder path in which HTM files are stored
void OpenCommandLineFile(int argc, char* argv[]); //opens htm file stated in command line
void InsertPrologue(); //inserts prologue in each HTM file
bool ChkBrOnlyLine(std::string line); // function used to check if line contains only opening bracket
std::string ReplaceTab(std::string str, const std::string& from, const std::string& to); //replaces tab with
                                                                                           exit code.


* Build Process:
* --------------
* Required Files: Graph.h, ActionsAndRules.h, TypeTable.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 04 April 2017
* - first release of new design
*/


#include <string>
#include "../Graph/Graph.h"
#include "../Parser/ActionsAndRules.h"
#include "../TypeTable/TypeTable.h"

using namespace std;

namespace CodeAnalysis {

	class ConvertToHtml {
	private:
		std::string SourceFileName;
		std::string DestDirName;
		std::string FileString;
		Graph<std::string, std::string> &depgraph;
		size_t totalVertices;
		TypeTable &typeTable;

	public:
		ConvertToHtml::ConvertToHtml() :depgraph(Repository::getInstance()->dependencyGraph()),
										typeTable(Repository::getInstance()->typeTable())
		{
			totalVertices = depgraph.size();
		}
		void SetFileName(std::string FileName);
		void FileInitialization();
		void EnterCode(std::string FileName);
		void FileClose();
		std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);
		void CreateJS();
		void CreateCSS();
		bool FileExists(const std::string& name);
		bool CheckForClassFunGlfn(std::string line, std::string line1);
		void CreateHomePage(std::vector<std::string> files);
		void SetDestDirName(std::string destDirName);
		void OpenCommandLineFile(int argc, char* argv[]);
		void InsertPrologue();
		bool ChkBrOnlyLine(std::string line);
		std::string ReplaceTab(std::string str, const std::string& from, const std::string& to);
	};

}
#endif

