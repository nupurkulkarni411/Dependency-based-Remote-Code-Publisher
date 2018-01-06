#pragma once
#ifndef TYPEANAL_H
#define TYPEANAL_H
///////////////////////////////////////////////////////////////////////
// TypeAnal.h -   this class is responsible for type analysis and    //
//                creation of type table.                            //
// ver 3.3                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Type-based Dependency Analysis,                      //
// CSE687 - Object Oriented Design                                   //
// Author:      Nupur Kulkarni, Syracuse University,                 //
//              nvkulkar@syr.edu                                     //
// Source:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public Type Analysis class.
* TypeAnal supports functionalities like performing type analysis
* and storing aanalysis to Type Table.
*

Public Interface:
=================
void doTypeAnal(); //for performing type analysis
void createTypeTable(ASTNode* pRoot); //creation of type table.
void extractGlobalFunctions(ASTNode * globalNode); // detects global functions
void DFS(ASTNode* pNode); //traverse AST in DFS manner.

* Build Process:
* --------------
* Required Files: TypeTable.h,Display.h,ActionsAndRules.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 01 March 2017
* - first release of new design
*/


#include "../Parser/ActionsAndRules.h"
#include <iostream>
#include <functional>
#include "../TypeTable/TypeTable.h"
#include "../Display/Display.h"

#pragma warning (disable : 4101)  // disable warning re unused variable x, below

namespace CodeAnalysis
{
	class TypeAnal
	{
	public:
		using SPtr = std::shared_ptr<ASTNode*>;

		TypeAnal();
		void doTypeAnal();
		void createTypeTable(ASTNode* pRoot);
		void displayTypeTable(std::string ttout);
		void extractGlobalFunctions(ASTNode * globalNode);
	private:
		void DFS(ASTNode* pNode);
		AbstrSynTree& ASTref_;
		ScopeStack<ASTNode*> scopeStack_;
		Scanner::Toker& toker_;
		TypeTable& typeTable_;
		Display1 d;
	};

	inline bool doDisplay(ASTNode* pNode)
	{
		static std::string toDisplay[] = {
			"function", "lambda", "class", "struct", "enum", "alias", "typedef"
		};
		for (std::string type : toDisplay)
		{
			if (pNode->type_ == type)
				return true;
		}
		return false;
	}

	
}
#endif