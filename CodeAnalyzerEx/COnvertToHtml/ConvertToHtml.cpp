///////////////////////////////////////////////////////////////////////
// ConvertToHtml.cpp - this is responsible for generating HTML code  //
//                     for C++ source code                           //
// ver 3.3                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Dependency-Based CodePublisher,                      //
//              CSE687 - Object Oriented Design                      //
// Author:      Nupur Kulkarni, Syracuse University,                 //
//              nvkulkar@syr.edu                                     //
///////////////////////////////////////////////////////////////////////
/*

* Build Process:
* --------------
* Required Files: ConvertToHtml.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 04 April 2017
* - first release of new design
*/



#include "ConvertToHtml.h"
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <fstream>


using namespace std;
using namespace CodeAnalysis;

//sets the file name of the file which is currently being processed 
void ConvertToHtml::SetFileName(std::string FileName)
{
	SourceFileName = FileName;
}

//Generates HTML head part containing all references to CSS and JavaScript files 
//and dependancies which are included in html file.
void ConvertToHtml::FileInitialization()
{
	std::string fileName = SourceFileName.substr(SourceFileName.find_last_of('\\') + 1);
	if (!FileExists(DestDirName + "\\" + "CSSFile.css")) //create css file if one does not exist 
		CreateCSS();
	if(!FileExists( DestDirName + "\\" + "JSFile.js")) //create javascript file if one does not exist
		CreateJS();
	FileString = FileString + "<html>\n";
	FileString = FileString + "<head>\n";
	FileString = FileString + "<link rel = " + "stylesheet " + "href = " + "CSSFile.css" + ">\n";
	FileString = FileString + "<script src= " + "JSFile.js" + ">" + "</script>";
	FileString = FileString + "\n</head>\n" + "<body>\n";
	FileString = FileString + "<h3>" + fileName + "</h3>" + "\n<hr />\n";
	FileString = FileString + "<div class=" + "indent" + ">";
	FileString = FileString + "<h4>Dependencies:</h4>";
	for (auto it = depgraph.begin(); it != depgraph.end(); ++it) //inserting links to dependant files
	{
		Vertex<std::string, std::string> v = *it;
		if (fileName == (v.value()).c_str())
		{
			for (size_t i = 0; i < v.size(); ++i)
			{
				Vertex<std::string, std::string>::Edge edge = v[i];
				FileString = FileString + "<p>"+ "<a href = " + depgraph[edge.first].value().c_str() + ".htm" + "> " + depgraph[edge.first].value().c_str() + "</a>" + "</p>";
			}
		}
	}
	FileString = FileString + "</div>\n" + "<hr />\n";
	FileString = FileString + "<pre>\n";
}

//process source code and generates corresponding HTML code
void ConvertToHtml::EnterCode(std::string file)
{
	int counter = 1;
	std::ifstream in(file);
	std::string line;
	do {
			std::getline(in, line);
			std::string tok = " " + line + " \n";
			tok = ReplaceAll(tok, "<", "&lt;");
			tok = ReplaceAll(tok, ">", "&gt;");
			tok = ReplaceTab(tok, "\t", "&nbsp;&nbsp;&nbsp;&nbsp;");
			if (tok.find("{") >= 0 && tok.find("{") < tok.length() && 
			   (tok.find("}") >= 0 && tok.find("}") < tok.length())) {
				FileString += tok; // case if opening and closing bracket iss on same line
			}
			else {
				if (tok.find("{") >= 0 && tok.find("{") < tok.length() && 
					!(tok.find("}") >= 0 && tok.find("}") < tok.length())) {
					if(CheckForClassFunGlfn(tok, FileString)) { //brackets are not on same line then check 
						size_t position = tok.find_last_of("{"); // if line contains only bracket
						std::string smallTok = tok.substr(0, position+1);
						FileString += smallTok;
						FileString += "<button onClick = \"OnClickDiv('div" + std::to_string(counter);
						FileString += "\', 'button" + std::to_string(counter);
						FileString += "\')"; FileString += "\"";
						FileString += " id = \"button" + std::to_string(counter);
						FileString += "\">"; FileString += "-</button>";
						FileString += "<div id = \"div" + std::to_string(counter); FileString += "\">";
						FileString += tok.substr(position+1);
						counter++;
					}
					else {
						FileString += "<div id = \"div" + std::to_string(counter); FileString += "\">";
						FileString += tok;  // current line of tokens does not contain any class name
						counter++;          //or function name and just  contains opening bracket
					}
				}
				else if (tok.find("}") >= 0 && tok.find("}") < tok.length()) {	
						FileString += tok + "</div><br>";
				}
				else {
					FileString += tok; //no brackets
				}
			}
	} while (in.good());
}

//Ends HTML file.
void ConvertToHtml::FileClose()
{
	FileString = FileString + "\n</pre>";
	//FileString = FileString + "<a href = Home.htm " + "class = button " + ">" + "Home" + "</a>";
	FileString = FileString + "\n</body>\n" + "</html>";
	std::string File_name = SourceFileName.substr(SourceFileName.rfind('\\') + 1);
	std::ofstream out( DestDirName + "\\" + File_name + ".htm");
	if (out.good()) {
		out << FileString;
		out.close();
	}
}


// Any markup character which is to be escaped will be replaced by appropriate exit code.
std::string ConvertToHtml::ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos ) {
		if ((!(str[start_pos + 1] == from[0] || str[start_pos - 1] == from[0])))
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
		}
		else
			start_pos = start_pos + 1; 
	}
		
	return str;
}

// creates JavaScript file
void ConvertToHtml::CreateJS()
{
	std::ofstream out(DestDirName + "\\" + "JSFile.js");
	if (out.good()) {
		out << "function OnClickDiv(divId, buttonId){\n"; 
		out << "\tvar value = document.getElementById(divId).style.display;\n";
		out << "\tif(value != 'none'){\n";
		out << "\t\tdocument.getElementById(divId).style.display = 'none';\n";
		out << "\t\tdocument.getElementById(buttonId).innerHTML = '+';\n";
		out << "\t}else{\n";
		out << "\t\tdocument.getElementById(divId).style.display = 'block';\n";
		out << "\t\tdocument.getElementById(buttonId).innerHTML = '-';\n";
		out << "\t}\n";
		out << "}\n";
		out.close();
	}
}

// creates CSS file.
void CodeAnalysis::ConvertToHtml::CreateCSS()
{
	std::ofstream out(DestDirName + "\\" + "CSSFile.css");
	if (out.good()) {
		out << "body {\n\tmargin:20px;\n\tcolor:black;\n\tbackground-color:#eee;\n\tfont-family:Consolas;\n\t";
		out << "font-weight:600;\n\tfont-size:110%;\n}\n";
		out << ".indent {\n\tmargin-left:20px;\n\tmargin-right:20px;\n}\n";
		out << "h4 {\n\tmargin-bottom: 3px;\n\tmargin-top:3px;\n}\n";
		out << "h3 {\n\ttext-align: center;\n}\n";
		out << "table {\n\tfont-family: arial, sans-serif;\n\twidth: 100%;\n}\n";
		out << "td, th {\n\tborder: 1px solid #dddddd;\n\ttext-align: center;\n\tpadding: 8px;\n}\n";
		out << "tr:nth-child(even) {\n\tbackground-color: #dddddd;\n\ttext-align: center;\n}\n";
		out << ".button {\n\tbackground-color: #555555;\n\tborder: none;\n\tborder-radius: 8px;\n\tcolor: white;\n\t";
		out << "padding: 15px 32px;\n\ttext-align: center;\n\ttext-decoration: none;\n\tdisplay: inline-block;\n\t";
		out << "font-size: 16px;\n\tfloat: center;\n}\n";
		out.close();
	}

}

// this function checks if css and javascript files 
//are already created and prevents creation of files every time.
bool ConvertToHtml::FileExists(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

//this function checks if current token 
//is class function or global function used for expand and collapse.
bool ConvertToHtml::CheckForClassFunGlfn(std::string line,std::string FileStr)
{
	std::size_t last_occ = FileStr.find_last_of("\n");
	FileStr = FileStr.substr(0, last_occ);
	std::size_t SecLast_occ = FileStr.find_last_of("\n");
	FileStr = FileStr.substr(SecLast_occ);
	if (line.find("if") != std::string::npos || line.find("while") != std::string::npos || 
		line.find("for") != std::string::npos || line.find("else") != std::string::npos)
		return false;
	if (FileStr.find("if") != std::string::npos || FileStr.find("while") != std::string::npos || 
		FileStr.find("for") != std::string::npos || FileStr.find("else") != std::string::npos)
		return false;
	if (ChkBrOnlyLine(line))
	{
		for (auto i = typeTable.getTypeTable().begin(); i != typeTable.getTypeTable().end(); ++i)
		{
			std::size_t f1 = FileStr.find(i->first);
			if (f1 != std::string::npos)
			{
				if (i->second.front().tname == "class" || i->second.front().tname == "global function" || 
					i->second.front().tname == "function")
				{
					return true;
				}
			}
		}
	}
	else
	{ 
		for (auto i = typeTable.getTypeTable().begin(); i != typeTable.getTypeTable().end(); ++i)
		{
			std::size_t f = line.find(i->first); 
			if (f != std::string::npos)
			{
				if (i->second.front().tname == "class" || i->second.front().tname == "global function" || 
					i->second.front().tname == "function")
				{
					return true;
				}
			}
		}
	}
	return false;
}

//creates home page listing all files and dependancies
void ConvertToHtml::CreateHomePage(std::vector<std::string> files)
{
	std::ofstream out(DestDirName + "\\" + "Home.htm");
	if (out.good()) {
		out << "<html>\n";
		out << "<head>\n";
		out << "<link rel = "; out << "stylesheet ";  out << "href = "; out << "CSSFile.css"; out << ">";
		out << "\n</head>\n"; out << "<body>\n"; //referance to CSS file.
		out << "<h3>"; out << "Home"; out << "</h3>";
		if (depgraph.size() != 0)
		{
			out << "<table>";
			out << "<tr>"; out << "<th>"; out << "Files"; out << "</th>";
			out << "<th>"; out << "Dependant Files"; out << "</th>"; out << "</tr>";
			for (auto it = depgraph.begin(); it != depgraph.end(); ++it)
			{
				out << "<tr>\n";
				Vertex<std::string, std::string> v = *it;
				out << "<td>\n";
				out << "<a href = "; out << (v.value()).c_str(); out << ".htm"; out << "> ";  
				out << (v.value()).c_str(); out << "</a>";
				out << "</td>\n";
				out << "<td>\n";
				for (size_t i = 0; i < v.size(); ++i)
				{
					Vertex<std::string, std::string>::Edge edge = v[i];
					out << "<a href = "; out << depgraph[edge.first].value().c_str(); 
					out << ".htm"; out << "> "; out << depgraph[edge.first].value().c_str(); 
					out << "</a>"; out << "\n";
				}
				out << "</td>\n";
				out << "</tr>\n";
			}
			out << "</table>";
		}
		else
		{
			for (size_t i = 0;i < files.size();i++)
			{
				out << "<a href = "; out << files[i] ; out << "> ";  out << files[i] << "</a>\n";
			}
		}
		out << "</body>";
		out << "</html>";
		out.close();
	}
}

//sets destination folder path in which HTM files are stored
void ConvertToHtml::SetDestDirName(std::string destDirName)
{
	DestDirName = destDirName;
}

//opens htm file stated in command line
void ConvertToHtml::OpenCommandLineFile(int argc, char * argv[])
{
	size_t find;
	std::string path = "Home.htm"; //by default Home.htm will be opened if no other file 
	std::string str;               //stated in the command line
	for (int i = 2; i < argc; i++)
	{
		str = argv[i];
		find = str.find(".htm");
		if (find != std::string::npos) {
			path = str;
			break;
		}
	}
	std::string homeLink = DestDirName + "\\" + path;
	std::wstring wHomeLink = std::wstring(homeLink.begin(), homeLink.end());
	LPCWSTR sw = wHomeLink.c_str();
	ShellExecute(0, 0, sw, 0, 0, SW_SHOW);
}

//inserts prologue in each HTM file
void ConvertToHtml::InsertPrologue()
{
	FileString = "<!----------------------------------------------------------------------------\n";
	std::string File_name = SourceFileName.substr(SourceFileName.rfind('\\') + 1);
	FileString += File_name + ".htm " + "- Generated HTML file of Project #3\n" ;
	FileString += "Published 04 April 2017\n";
	FileString += "Nupur Kulkarni, CSE687 - Object Oriented Design, Spring 2017\n\n";
	FileString += "Note:\n";
	FileString += "- Markup characters in the text part, enclosed in <pre>...</pre> have been \n";
	FileString += "  replaced with escape sequences, e.g., < becomes &lt; and > becomes &gt;\n";
	FileString += "----------------------------------------------------------------------------->\n";

}

// function used to check if line contains only opening bracket
bool ConvertToHtml::ChkBrOnlyLine(std::string line)
{
	line = line.substr(0, line.find("{") - 1);
	for (size_t i = 0 ;i< line.size();)
	{
		if (line[i] == ' ' || line[i] == '&' || line[i] == 'n' || 
			line[i] == 'b' || line[i] == 's' || line[i] == 'p' || line[i] == ';' || isblank(line[i]))
			i++;
		else
			return false;
	}
	return true;
}

void CodeAnalysis::ConvertToHtml::CreateIndexPage(HTTPMessage hMsg, std::vector<std::string> dep)
{
	std::ofstream out(DestDirName + "\\" + "Home.htm");
	if (out.good()) {
		out << "<html>\n";
		out << "<head>\n";
		out << "<link rel = "; out << "stylesheet ";  out << "href = "; out << "CSSFile.css"; out << ">";
		out << "\n</head>\n"; out << "<body>\n"; //referance to CSS file.
		out << "<h3>"; out << "Home"; out << "</h3>";
		out << "<h2>"; out << "Requested File: \n" << "</h2>";
		out << "<a href = "; out << hMsg.name(); out << ".htm"; out << "> ";  out << hMsg.name() << "</a>\n";
		out << "<h2>"; out << "Dependant Files: \n" << "</h2>";
		for (size_t i = 0; i < dep.size(); i++)
		{
			out << "<a href = "; out << dep[i]; out << "> ";  out << dep[i].substr(0,dep[i].find_last_of(".")) << "</a>\n";
		}
		out << "</body>";
		out << "</html>";
		out.close();
	}
}

//replaces tab with exit code.
std::string CodeAnalysis::ConvertToHtml::ReplaceTab(std::string str, const std::string & from, const std::string & to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}


//------------------------------------Test Stub----------------------------------------------//

#ifdef TEST_CONVERTTOHTML

int main()
{
	ConvertToHtml cth1;
	cth1.SetFileName(file);
	cth1.SetDestDirName(DestFolderPath);
	cth1.InsertPrologue();
	cth1.FileInitialization();
	cth1.EnterCode(file);
	cth1.FileClose();
}

#endif



