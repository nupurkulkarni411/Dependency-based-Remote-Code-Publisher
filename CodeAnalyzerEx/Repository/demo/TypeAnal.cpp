///////////////////////////////////////////////////////////////////////
// TypeAnal.cpp - this is responsible for type analysis and          //
//                storing type analysis to type table.               //
// ver 3.3                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Type-based Dependency Analysis,                      //
//              CSE687 - Object Oriented Design                      //
// Author:      Nupur Kulkarni, Syracuse University,                 //
//              nvkulkar@syr.edu                                     //
// Source:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
/*

* Build Process:
* --------------
* Required Files: TypeAnal.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 01 March 2017
* - first release of new design
*/


#include "TypeAnal.h"

using namespace CodeAnalysis;

//constructor
TypeAnal::TypeAnal() :
	ASTref_(Repository::getInstance()->AST()),
	scopeStack_(Repository::getInstance()->scopeStack()),
	toker_(*(Repository::getInstance()->Toker())),
	typeTable_(Repository::getInstance()->typeTable())
{
	std::function<void()> test = [] { int x; };
}


//Traverse the AST in depth first search manner.
void TypeAnal::DFS(ASTNode* pNode)
{
	static std::string path = "";
	if (pNode->path_ != path) {
		std::cout << "\n    -- " << pNode->path_ << "\\" << pNode->package_;
		path = pNode->path_;
	}
	if (doDisplay(pNode)) {
		std::cout << "\n  " << pNode->name_;
		std::cout << ", " << pNode->type_;
	}
	for (auto pChild : pNode->children_)
		DFS(pChild);
}

//perform type analysis 
void TypeAnal::doTypeAnal()
{
	std::cout << "\n  starting type analysis:\n";
	std::cout << "\n  scanning AST and displaying important things:";
	std::cout << "\n -----------------------------------------------";
	ASTNode* pRoot = ASTref_.root();
	DFS(pRoot);
	createTypeTable(pRoot);
	std::string ttoutput = d.displayTypeTable(typeTable_.getTypeTable());
	displayTypeTable(ttoutput);
}

//storing result of analysis into type table.
void TypeAnal::createTypeTable(ASTNode * pRoot)
{
	for (ASTNode* node : pRoot->children_) {
		if (node->children_.size() > 0) {
			createTypeTable(node);
		}
		TypeTableEntry entry;
		entry.filepath = node->path_;//path of file in which token is present
		entry.filepackage = node->package_; //package of file in which token is present
		entry.tname = node->type_; //token type
		if (entry.tname == "struct" || entry.tname == "class" || entry.tname == "enum" || entry.tname == "alias"
			|| entry.tname == "typedef" || entry.tname == "using")
		{
			typeTable_.makeTypeTableEntry(node->name_, entry);
		}
		if (entry.tname == "function" && (node->parentType_ == "class"
			|| node->parentType_ == "struct" || node->parentType_ == "enum" || node->parentType_ == "typedef"))
		{
			typeTable_.makeTypeTableEntry(node->name_, entry);
		}
	}
	extractGlobalFunctions(Repository::getInstance()->getGlobalScope());
}

//display type table on console.
void TypeAnal::displayTypeTable(std::string ttout)
{
	std::cout << ttout;
}

//to extract global scope
void TypeAnal::extractGlobalFunctions(ASTNode * globalNode)
{

	for (auto node : globalNode->children_) {
		if (node->children_.size() > 0) {
			extractGlobalFunctions(node);
		}
		TypeTableEntry entry;
		entry.filepath = node->path_;
		entry.filepackage = node->package_;
		entry.tname = "global "; entry.tname.append(node->type_);
		if (node->type_ == "function" && node->name_ != "main" && node->parentType_ != "class"
			&& node->parentType_ != "struct" && node->parentType_ != "enum" && node->parentType_ != "typedef") {

			typeTable_.makeTypeTableEntry(node->name_, entry);

		}
	}
}

#ifdef TEST_TYPEANAL

int main()
{
	//testing how type table is getting displayed
	std::string ttoutput = d.displayTypeTable(typeTable_.getTypeTable());
	displayTypeTable(ttoutput);
	return 0;
}
#endif
