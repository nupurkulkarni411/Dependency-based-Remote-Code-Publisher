#ifndef WINDOW_H
#define WINDOW_H
///////////////////////////////////////////////////////////////////////
// Window.h - Provides a GUI client to interact with the Remote		 //
//				Code Publisher server								 //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Platform:    Lenovo Ideapad 500, Windows 10, AMD A10 x64			 //
// Application: Remote Code Publisher, Project 4					 //
//              CSE687 - Object Oriented Design						 //
// Author:		Nupur Kulkarni, Syracuse University					 //
//				nvkulkar@syr.edu									 //
// Source:      Jim Fawcett, CST 2-187, Syracuse University		     //
//              (315) 443-3948, jfawcett@twcny.rr.com				 //
///////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package provides one class, WPFClient,
*  derived from System::Windows::Window that is compiled with the /clr
*  option to run in the Common Language Runtime. It uses the MockChannel
*  package to communicate with the remote server.
*
*  The window class hosts, in its window, a tab control with two views,
*  one for uploading files into the repository, and another for publishing or deleting
*  files from the repository.
*  It loads the DLL holding MockChannel. It posts messages to be sent,
*  and dequeues messages to be processed from the MockChannel.
*
* Build Process:
* --------------
* Required Files:
*   Window.h, Window.cpp, MockChannel.h, MockChannel.cpp
*   HTTPMessage.h, HTTPMessage.cpp
*
* Build Command: devenv RemoteRepository.sln /rebuild debug
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 1 May 2016
*/
/*
* Create C++/CLI Console Application
* Right-click project > Properties > Common Language Runtime Support > /clr
* Right-click project > Add > References
*   add references to :
*     System
*     System.Windows.Presentation
*     WindowsBase
*     PresentatioCore
*     PresentationFramework
*/
using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;

#include "../MockChannel/MockChannel.h"
#include <iostream>
#include <vector>

namespace CppCliWindows
{
	ref class WPFClient2 : Window
	{
		// CommChannel reference

		IMockChannel* pChann_;

		// Controls for Window

		DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
		Grid^ hGrid = gcnew Grid();
		TabControl^ hTabControl = gcnew TabControl();
		TabItem^ hPublishTab = gcnew TabItem();
		TabItem^ hUploadTab = gcnew TabItem();
		StatusBar^ hStatusBar = gcnew StatusBar();
		StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
		TextBlock^ hStatus = gcnew TextBlock();

		// Controls for Publish View

		Grid^ hPublishGrid = gcnew Grid();
		Button^ hGetPkgListButton = gcnew Button();
		Button^ hDeleteButton = gcnew Button();
		Button^ hPublishWithDepButton = gcnew Button();
		ListBox^ hRepFileList = gcnew ListBox();
		ScrollViewer^ hScrollViewer1 = gcnew ScrollViewer();
		StackPanel^ hStackPanel1 = gcnew StackPanel();
		Button^ hOpenWebPageButton = gcnew Button();

		// Controls for Upload View
		Grid^ hUploadGrid = gcnew Grid();
		Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
		ListBox^ hUploadList = gcnew ListBox();
		ListBox^ hCategoriesList = gcnew ListBox();
		Button^ hFolderBrowseButton = gcnew Button();
		Button^ hUploadGetFileListButton = gcnew Button();
		Button^ hUploadButton = gcnew Button();
		TextBlock^ hUploadTextBlock = gcnew TextBlock();
		TextBlock^ hCategoriesTextBlock = gcnew TextBlock();
		Grid^ hGrid2 = gcnew Grid();

		// receive thread

		Thread^ recvThread;

	public:
		WPFClient2(array<System::String^>^ args);
		~WPFClient2();

		void setUpStatusBar();
		void setUpTabControl();
		void setUpPublishView();
		void setUpUploadView();
		void setUpUploadButtons();
		void setUpUploadLabels();
		void setUpUploadLists();

		void getPkgList(Object^ obj, RoutedEventArgs^ args);
		void getFileList(Object^ obj, RoutedEventArgs^ args);
		void Upload(Object^ obj, RoutedEventArgs^ args);
		void addPackage(String^ msg);
		void addFiles(String^ msg);
		void updateStatusBar(String^ msg);
		void getMessage();
		void fileDeletion(Object^ sender, RoutedEventArgs^ args);
		void OpenWebPage(Object^ sender, RoutedEventArgs^ args);
		void publishWithDep(Object^ sender, RoutedEventArgs^ args);
		void browseForFolder(Object^ sender, RoutedEventArgs^ args);
		void OnLoaded(Object^ sender, RoutedEventArgs^ args);
		void Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args);
	private:
		std::string toStdString(String^ pStr);
		String^ toSystemString(std::string& str);
		void setPublishListProperties();
		void setButtonsProperties();
		String^ ServerPortNumber;
		String^ ClientPortNumber;
	};
}


#endif
