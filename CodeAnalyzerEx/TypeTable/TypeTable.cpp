///////////////////////////////////////////////////////////////////////
// TypeTable.cpp - this file contains test stub used to test         //
//				   functionality of type table.                      //
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
* Required Files: TypeTable.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 01 March 2017
* - first release of new design
*/

#include "TypeTable.h"
#ifdef TEST_TYPETABLE

int main()
{
	//testing how type table is getting displayed
	std::string ttoutput = d.displayTypeTable(typeTable_.getTypeTable());
	displayTypeTable(ttoutput);
	return 0;
}
#endif