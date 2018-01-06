#pragma once
#ifndef DEPANAL_H
#define DEPANAL_H
///////////////////////////////////////////////////////////////////////
// DepAnal.h -   this class uses type table to analyze dependency    //
//				 analysis.                                           //
// ver 3.3                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Type-based Dependency Analysis,                      //
// CSE687 - Object Oriented Design                                   //
// Author:      Nupur Kulkarni, Syracuse University,                 //
//              nvkulkar@syr.edu                                     //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public Dependancy Analysis class.
* DepAnal supports functionalities like analysis of dependancies between files and saving dependancy
* analysis to xml and inmemory database.
*

Public Interface:
=================
void addDependencyToGraph(const std::string &fileName1, const std::string &fileName2);  // adds dependancy to dependancy graph
void displayGraphWithDependencies(); // displays depedancy graph 
Graph<std::string, std::string>& getDependGraph() { return dependancyGraph; } //returns private member of class dependancy graph


* Build Process:
* --------------
* Required Files: Parser.h,Executive.h,itokcollection.h,ScopeStack.h,Tokenizer.h,SemiExp.h,Graph.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 03 March 2017
* - first release of new design
* ver 2.0 : 04 April 2017
* - Modified dependancy analysis function
*/

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <iostream>
#include "../Parser/Parser.h"
#include "../Analyzer/Executive.h"
#include "../SemiExp/itokcollection.h"
#include "../ScopeStack/ScopeStack.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "../Graph/Graph.h"

using namespace Scanner;

namespace CodeAnalysis
{
	class DepAnal
	{
	public:
		using Path = std::string;
		using Pattern = std::string;
		using Patterns = std::vector<Pattern>;
		using File = std::string;
		using Files = std::vector<File>;
		using Ext = std::string;
		using Options = std::vector<char>;
		using FileMap = std::unordered_map<Pattern, Files>;
		using DependMap = std::unordered_map<std::string, std::set<std::string>>;
	public:
		
		DepAnal::DepAnal() : typeTable(Repository::getInstance()->typeTable()),
			dependancyGraph(Repository::getInstance()->dependencyGraph())
		{
			pToker = new Toker;
			pToker->returnComments(false);
			pSemi = new SemiExp(pToker);
			pParser = new Parser(pSemi);
			
		}
		
		bool ProcessCommandLine(int argc, char* argv[]);
		void getSourceFiles();
		void processSourceCode();
		bool Attach(const std::string& name);
		void addVerticesToGraph();
		void addDependencyToGraph(const std::string &fileName1, const std::string &fileName2);
		void displayGraphWithDependencies();
		Graph<std::string, std::string>& getDependGraph() { return dependancyGraph; }
		DepAnal::~DepAnal()
		{
			delete pParser;
			delete pSemi;
			delete pToker;
			if (pIn != nullptr)
				pIn->close();
			delete pIn;
		}

	private:
		Path path_;
		Patterns patterns_;
		Options options_;
		FileMap fileMap_;
		Parser* pParser;
		TypeTable &typeTable;
		DependMap dependancyMap;
		std::ifstream* pIn;
		Scanner::Toker* pToker;
		Scanner::SemiExp* pSemi;
		Graph<std::string, std::string> &dependancyGraph;
		Display<std::string, std::string> displayDependancy;
	};
}
#endif