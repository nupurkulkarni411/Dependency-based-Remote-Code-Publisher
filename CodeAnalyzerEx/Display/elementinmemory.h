#pragma once
#ifndef ELEMENTINMEMEORY_H
#define ELEMENTINMEMORY_H

#include <vector>
#include "../CppProperties/CppProperties.h"
#include "../CppProperties/CppProperties.h"
#include "../Convert/Convert.h"
#include "../StrHelper.h"
#include <ctime>

/////////////////////////////////////////////////////////////////////
// Element class                                                   //
// - This Class provides basic structure of value                  //
//   of Key/Value Database                                         //
/////////////////////////////////////////////////////////////////////

using namespace std;

template<typename Data>
class Element
{
private:
	std::vector<std::string> child;

public:

	//veriables
	using Name = std::string;
	using Category = std::string;
	using TimeDate = std::string;
	using Description = std::string;


	Property<Name> name;            // metadata
	Property<Category> category;    // metadata
	Property<Description> description; //metadata
	Property<TimeDate> timeDate;    // metadata
	Property<Data> data;            // data

									//methods
	std::string show();
	std::vector<std::string> Element<Data>::getchild();
	void setChild(std::string key);
	void deletechild(int pos);
	Element();
	std::string setdatetime();

};


template<typename Data>
std::vector<std::string> Element<Data>::getchild()
{
	return child;
}

template<typename Data>
void Element<Data>::setChild(std::string key)
{
	child.push_back(key);
}

template<typename Data>
void Element<Data>::deletechild(int pos)
{

	child.erase(child.begin() + pos);
}

template<typename Data>
inline Element<Data>::Element()
{
	name = "NULL";
	category = "NULL";
	timeDate = "NULL";
	description = "NULL";
}


template<typename Data>
std::string Element<Data>::show()
{
	// show single element 

	std::ostringstream out;

	out.setf(std::ios::adjustfield, std::ios::left);
	out << "\n    " << std::setw(15) << "Name" << " : " << name;
	out << "\n    " << std::setw(15) << "Nategory" << " : " << category;
	out << "\n    " << std::setw(15) << "Description" << " : " << category;
	out << "\n    " << std::setw(15) << "TimeDate" << " : " << timeDate;
	std::vector<std::string> temp = getchild();
	if (temp.empty())
	{
		out << "\n    No children.";
	}
	else
	{
		for (size_t i = 0; i < temp.size(); i++)
		{
			out << "\n    " << "Child " << i << std::setw(10) << " : " << temp[i] << " ";
		}
	}
	out << "\n    " << std::setw(15) << "Data" << " : " << data;
	out << "\n";
	return out.str();

}


#endif

template<typename Data>
inline std::string Element<Data>::setdatetime()
{
	Convert<time_t> conv;
	time_t t = time(0);
	struct tm * now = localtime(&t);
	return conv.toString((now->tm_year + 1900)) + "-" + conv.toString((now->tm_mon + 1)) + "-" + conv.toString((now->tm_mday)) + " " + conv.toString((now->tm_hour)) + ":" + conv.toString((now->tm_min)) + ":" + conv.toString((now->tm_sec));
}
