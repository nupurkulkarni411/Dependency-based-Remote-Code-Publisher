#pragma once
#ifndef TYPETABLE_H
#define TYPETABLE_H
///////////////////////////////////////////////////////////////////////
// TypeTable.h - Provides class typetable and typetableentry         //
//               to store type analysis information.                 //
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
* This package provides public TypeTable and TypeTableEntry classes.
* TypeTable is used to store type analysis.

Public Interface:
=================
void makeTypeTableEntry(Name name, TypeTableEntry entry) //makes an entry in a typetable.
TypeTableMap& getTypeTable() //returns type table.

* Build Process:
* --------------
* Required Files: None.
*
* Maintenance History:
* --------------------
* ver 1.0 : 01 March 2017
* - first release of new design
*/


#include <vector>
#include <unordered_map>
#include <iostream>

namespace CodeAnalysis
{
	class TypeTableEntry
	{
		using FilePath = std::string;
		using FilePackage = std::string;
		using TypeName = std::string;
	public:
		FilePath filepath;
		FilePackage filepackage;
		TypeName tname;
	};

	class TypeTable
	{
		using Name = std::string;
		using TypeTableMap = std::unordered_map<Name, std::vector<TypeTableEntry>>;
	public:
		void makeTypeTableEntry(Name name, TypeTableEntry entry)
		{
			if(typetablemap[name].size() != 0)
			{
				for each (auto typeMapEntry in typetablemap[name])
				{
					if (entry.filepath == typeMapEntry.filepath)
						return;
				}
			}
			typetablemap[name].push_back(entry);
		}
		TypeTableMap& getTypeTable()
		{
			return typetablemap;
		}
	private:
		TypeTableMap typetablemap;
	};
}
#endif
