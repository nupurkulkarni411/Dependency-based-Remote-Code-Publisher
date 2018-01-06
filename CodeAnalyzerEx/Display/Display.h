#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H
///////////////////////////////////////////////////////////////////////
// Display.h -   this class uses displays the type table             //
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
* This package provides a display class.
* Display class supports functionalities like showing type table on console.
* 
*

Public Interface:
=================
std::string displayTypeTable(TypeTableMap typeTableMap); // Displays Type Table


* Build Process:
* --------------
* Required Files: TypeTable.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 03 March 2017
* - first release of new design
*/


#include <iostream>
#include <unordered_map>
#include "../TypeTable/TypeTable.h"


namespace CodeAnalysis
{
	class Display1
	{
	public:
		using Name = std::string;
		using TypeTableMap = std::unordered_map<Name, std::vector<TypeTableEntry>>;
		std::string displayTypeTable(TypeTableMap typeTableMap);
	};
}
#endif