///////////////////////////////////////////////////////////////////////
// DepAnal.cpp - this class uses type table to analyze dependency    //
//				 analysis.                                           //
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
* Required Files: DepAnal.h, FileSystem.h, Parser.h
* 
* Maintenance History:
* --------------------
* ver 1.0 : 01 March 2017
* - first release of new design
*/


#include "DepAnal.h"
#include "../FileSystem/FileSystem.h"
#include "../Parser/Parser.h"
#include <vector>
#include <sstream>


using namespace CodeAnalysis;
#pragma warning(disable : 4996)


//dispays dependancy analysis graph
void CodeAnalysis::DepAnal::displayGraphWithDependencies()
{

	std::cout << "\n\n  Displaying Dependency Ananlysis\n";
	std::cout << "  -----------------------------------------------------------------------------------------";
	displayDependancy.show(dependancyGraph);
}

//function adds vertices to graph
void DepAnal::addVerticesToGraph()
{
	static int index = 0;
	for (auto i : fileMap_)
	{
		for (auto file : i.second)
		{
			Vertex<std::string, std::string> vert(file.substr(file.find_last_of("\\") + 1), index);
			if (dependancyGraph.addVertex(vert) == true)
			{
				index++;
			}
		}
	}
}


//adds edge to graph
void DepAnal::addDependencyToGraph(const std::string &fileName1, const std::string &fileName2)
{
	static int i = 0;
	int in1 = -1;
	int in2 = -1;
	for (size_t j = 0; j < dependancyGraph.size(); j++)
	{
		Vertex<std::string, std::string>& v1 = dependancyGraph[j];
		if (v1.value() == fileName1)
		{
			in1 = (int)j;
		}
		if (v1.value() == fileName2)
		{
			in2 = (int)j;
		}
	}

	std::stringstream stream;
	stream.str("");
	stream << "edge" << i;
	if ((in1 != -1) && (in2 != -1))
	{
		if (dependancyGraph.addEdge(stream.str(), dependancyGraph[in1], dependancyGraph[in2]) == true)
		{
			i++;
		}
	}
}

//extract contains of file 
bool DepAnal::Attach(const std::string& name)
{
	if (pToker == 0)
		return false;
	if (pIn != nullptr)
	{
		pIn->close();
		delete pIn;
	}
	pIn = new std::ifstream(name);
	if (!pIn->good())
		return false;

	// check for Byte Order Mark (BOM)
	char a, b, c;
	a = pIn->get();
	b = pIn->get();
	c = pIn->get();
	if (a != (char)0xEF || b != (char)0xBB || c != (char)0xBF)
		pIn->seekg(0);

	if (!pIn->good())
	{
		return true;
	}
	return pToker->attach(pIn);
}

//to process command line
bool DepAnal::ProcessCommandLine(int argc, char* argv[])
{
	if (argc < 2)
	{
		return false;
	}
	try {
		path_ = FileSystem::Path::getFullFileSpec(argv[1]);
		if (!FileSystem::Directory::exists(path_))
		{
			std::cout << "\n\n  path \"" << path_ << "\" does not exist\n\n";
			return false;
		}
		for (int i = 2; i < argc; ++i)
		{
			if (argv[i][0] == '/')
				options_.push_back(argv[i][1]);
			else
				patterns_.push_back(argv[i]);
		}
		if (patterns_.size() == 0)
		{
			return false;
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  command line argument parsing error:";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}
	return true;
}

//get all source files from specified file location on command line
void DepAnal::getSourceFiles()
{
	AnalFileMgr fm(path_, fileMap_);
	for (auto patt : patterns_)
		fm.addPattern(patt);
	fm.search();
}

//main functionality checking for dependancy 
void DepAnal::processSourceCode()
{
	addVerticesToGraph();
	for (auto item : fileMap_)
	{
		for (auto file : item.second)
		{
			Attach(file);
			std::string fileName1;
			if (file.find_last_of("\\") < file.length())
			{
				fileName1 = file.substr(file.find_last_of("\\") + 1);
			}
			while (pParser->next())
			{
				std::vector<std::string>::iterator it = pSemi->begin();
				for (it; it != pSemi->end(); it++)
				{
					if (typeTable.getTypeTable().find(*it) != typeTable.getTypeTable().end())
					{
						//if entry is present in the type table but filenames are different then make entry in dependancy map
						for each (auto typedetails in typeTable.getTypeTable().find(*it)->second)
						{
							if (fileName1 != typedetails.filepackage)
							{
								if (dependancyMap[fileName1].insert(typedetails.filepackage).second != false)
								{
									//add edge between dependant nodes
									std::string fileName2 = typedetails.filepackage;
									addDependencyToGraph(fileName1, fileName2);
								}
							}
						}
					}
				}
			}
		}
	}
}

//--------------------------------------------Test Stub-------------------------------------//

#ifdef TEST_DEPANAL
int main()
{
	saveDepAnalToNoSqlDB(getDependGraph());
	displayGraphWithDependencies();
	return 0;
}
#endif