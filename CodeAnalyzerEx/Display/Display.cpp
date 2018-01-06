///////////////////////////////////////////////////////////////////////
// Display.cpp - this class shows type table on console.             //
// ver 3.3                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Type-based Dependency Analysis,                      //
//              CSE687 - Object Oriented Design                      //
// Author:      Nupur Kulkarni, Syracuse University,                 //
//              nvkulkar@syr.edu                                     //
///////////////////////////////////////////////////////////////////////
/*

* Build Process:
* --------------
* Required Files: Display.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 03 March 2017
* - first release of new design
*/

#include "Display.h"
#include <sstream>
#include <iomanip>


//function to show type table on console.
std::string CodeAnalysis::Display1::displayTypeTable(TypeTableMap typeTableMap)
{
	std::ostringstream out;
	out.setf(std::ios::adjustfield, std::ios::left);
	out << "\n\n    Type Table:  \n\n";
	out << "\n    " << std::setw(30) << "TypeName" <<std::setw(30) << "Type" << std::setw(27) << "FileName" << "\n";
	out << std::setw(8) << "------------------------------------------------------------------------------------";
	for (auto it = typeTableMap.begin(); it != typeTableMap.end(); ++it)
	{
		out << "\n   ";
		out << std::setw(30) << it->first << std::setw(30) << it->second.front().tname << std::setw(27) << it->second.front().filepackage;
	}
	out << "\n";
	return out.str();
}

#ifdef TEST_STRONGCOMPONENT

#include "StrongComponents.h"
int main()
{
	//testing how type table is getting displayed
	displayTypeTable(TypeTableMap typeTableMap);
}

#endif


