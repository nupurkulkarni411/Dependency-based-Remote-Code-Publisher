///////////////////////////////////////////////////////////////////////
// Window.cpp - Provides a GUI client to interact with the remote	 //
//				code publisher server								 //
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
*  To run as a Windows Application:
*  - Set Project Properties > Linker > System > Subsystem to Windows
*  - Comment out int main(...) at bottom
*  - Uncomment int _stdcall WinMain() at bottom
*  To run as a Console Application:
*  - Set Project Properties > Linker > System > Subsytem to Console
*  - Uncomment int main(...) at bottom
*  - Comment out int _stdcall WinMain() at bottom
*/
#include "Window.h"
#include "../HTTPMessage/HTTPMessage.h"
using namespace CppCliWindows;

WPFClient2::WPFClient2(array<System::String^>^ args)
{
	//Initialization
	ClientPortNumber = args[0];
	ServerPortNumber = args[1];
	// set up channel
	ObjectFactory* pObjFact = new ObjectFactory;
	pChann_ = pObjFact->createMockChannel("localhost", std::stoi(toStdString(args[0])));
	pChann_->start();
	pChann_->connect("localhost", std::stoi(toStdString(args[1])));
	delete pObjFact;
	// client's receive thread
	recvThread = gcnew Thread(gcnew ThreadStart(this, &WPFClient2::getMessage));
	recvThread->Start();
	// set event handlers
	hDeleteButton->IsEnabled = false;
	hPublishWithDepButton->IsEnabled = false;
	hOpenWebPageButton->IsEnabled = false;
	this->Loaded +=
		gcnew System::Windows::RoutedEventHandler(this, &WPFClient2::OnLoaded);
	this->Closing +=
		gcnew CancelEventHandler(this, &WPFClient2::Unloading);
	hGetPkgListButton->Click += gcnew RoutedEventHandler(this, &WPFClient2::getFileList);
	hDeleteButton->Click += gcnew RoutedEventHandler(this, &WPFClient2::fileDeletion);
	hOpenWebPageButton->Click += gcnew RoutedEventHandler(this, &WPFClient2::OpenWebPage);
	hPublishWithDepButton->Click += gcnew RoutedEventHandler(this, &WPFClient2::publishWithDep);
	hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFClient2::browseForFolder);
	hUploadGetFileListButton->Click += gcnew RoutedEventHandler(this, &WPFClient2::getPkgList);
	hUploadButton->Click += gcnew RoutedEventHandler(this, &WPFClient2::Upload);

	// set Window properties

	this->Title = "Client";
	this->Width = 800;
	this->Height = 600;

	// attach dock panel to Window

	this->Content = hDockPanel;
	hDockPanel->Children->Add(hStatusBar);
	hDockPanel->SetDock(hStatusBar, Dock::Bottom);
	hDockPanel->Children->Add(hGrid);

	// setup Window controls and views

	setUpTabControl();
	setUpStatusBar();
	setUpPublishView();
	setUpUploadView();
}

// -- destructor deletes memory allocated to the MockChannel pointer -- //

WPFClient2::~WPFClient2()
{
	delete pChann_;
}

// -- sets up the status bar and docks it to the bottom of the window -- //

void WPFClient2::setUpStatusBar()
{
	hStatusBar->Items->Add(hStatusBarItem);
	hStatus->Text = "very important messages will appear here";
	hStatusBarItem->Content = hStatus;
	hStatusBar->Padding = Thickness(10, 2, 10, 2);
}

// -- sets up the tab control by adding the upload and publish tabs -- //

void WPFClient2::setUpTabControl()
{
	hGrid->Children->Add(hTabControl);
	hUploadTab->Header = "Upload";
	hPublishTab->Header = "Publish/Delete";
	hTabControl->Items->Add(hUploadTab);
	hTabControl->Items->Add(hPublishTab);
}

//  -- set up the listbox to display packages in the publish view -- //

void WPFClient2::setPublishListProperties()
{
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hPublishGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hRepFileList;
	hRepFileList->Items->Add("");
	hScrollViewer1->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewer1->Content = hBorder1;
	hPublishGrid->SetRow(hScrollViewer1, 0);
	hPublishGrid->Children->Add(hScrollViewer1);
}

// -- setup the buttons in the publish view -- //

void WPFClient2::setButtonsProperties()
{
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(75);
	hPublishGrid->RowDefinitions->Add(hRow2Def);
	hGetPkgListButton->Content = "Get Repository\n   Contents";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 50;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hDeleteButton->Content = "Delete File";
	hBorder2->Child = hGetPkgListButton;
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 50;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hDeleteButton;
	hStackPanel1->Children->Add(hBorder2);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 20;
	hStackPanel1->Children->Add(hSpacer);
	hStackPanel1->Children->Add(hBorder3);
	TextBlock^ hSpacer2 = gcnew TextBlock();
	hSpacer2->Width = 20;
	hStackPanel1->Children->Add(hSpacer2);
	Border^ hBorder4 = gcnew Border();
	hBorder4->Width = 120;
	hBorder4->Height = 50;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hPublishWithDepButton->Content = "Publish";
	hBorder4->Child = hPublishWithDepButton;
	hStackPanel1->Children->Add(hBorder4);
	TextBlock^ hSpacer3 = gcnew TextBlock();
	hSpacer3->Width = 20;
	hStackPanel1->Children->Add(hSpacer3);
	Border^ hBorder5 = gcnew Border();
	hBorder5->Width = 120;
	hBorder5->Height = 50;
	hBorder5->BorderThickness = Thickness(1);
	hBorder5->BorderBrush = Brushes::Black;
	hOpenWebPageButton->Content = "Open Web Page";
	hBorder5->Child = hOpenWebPageButton;
	hStackPanel1->Children->Add(hBorder5);
	hStackPanel1->Orientation = Orientation::Horizontal;
	hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hPublishGrid->SetRow(hStackPanel1, 1);
	hPublishGrid->Children->Add(hStackPanel1);
}

// -- setup the publish tab view and its controls -- //

void WPFClient2::setUpPublishView()
{
	Console::Write("\n  setting up publish view");
	hPublishGrid->Margin = Thickness(20);
	hPublishTab->Content = hPublishGrid;

	setPublishListProperties();
	setButtonsProperties();
}

// -- accepts a System::String^ and returns it as a std::string -- //

std::string WPFClient2::toStdString(String^ pStr)
{
	std::string dst;
	for (int i = 0; i < pStr->Length; ++i)
		dst += (char)pStr[i];
	return dst;
}

// -- function that frames an HTTPMessage for requesting the package list and posts it to the MockChannel for sending -- //

void WPFClient2::getPkgList(Object^ obj, RoutedEventArgs^ args)
{
	Console::Write("\n  sending package list request");
	HTTPMessage msg;
	msg.command() = "get_package_list";
	msg.fromAddress() = "[localhost]:" + toStdString(ClientPortNumber);
	msg.toAddress() = ("[localhost]:" + toStdString(ServerPortNumber));
	msg.name() = "empty";
	msg.content() = "no_content";
	std::vector<std::string> vec = {};
	msg.setBody(vec);
	pChann_->postMessage(msg.buildMessage());
	hStatus->Text = "Sent Package List Request";
}

// -- function that frames an HTTPMessage for requesting the file list and posts it to the MockChannel for sending -- //

void CppCliWindows::WPFClient2::getFileList(Object ^ obj, RoutedEventArgs ^ args)
{
	Console::Write("\n  Requirement 5: Provided view Repository contents \n");
	Console::Write("  as per described in purpose section.\n");
	Console::Write("\n  sending file list request");
	HTTPMessage msg;
	msg.command() = "get_file_list";
	msg.fromAddress() = "[localhost]:" + toStdString(ClientPortNumber);
	msg.toAddress() = ("[localhost]:" + toStdString(ServerPortNumber));
	msg.name() = "empty";
	msg.content() = "no_content";
	std::vector<std::string> vec = {};
	msg.setBody(vec);
	pChann_->postMessage(msg.buildMessage());
	hStatus->Text = "Sent File List Request";
	hDeleteButton->IsEnabled = true;
	hPublishWithDepButton->IsEnabled = true;
	hOpenWebPageButton->IsEnabled = true;
}

// -- function that frames an HTTPMessage for requesting file/files upload and posts it to the MockChannel for sending -- //

void WPFClient2::Upload(Object^ obj, RoutedEventArgs^ args)
{
	if (hUploadList->SelectedItem == nullptr)
	{
		hStatus->Text = "Please select a file";
		return;
	}
	Console::Write("\n  Requirement 5: Provided a Client program that can upload files\n");
	Console::Write("  User can selcect one or multiple files to upload in the repository.\n");
	Console::Write("  Uploaded files can be seen in repository folder under selected category.\n");
	Console::Write("\n  sending upload request");
	hStatus->Text = "Sending upload Request";
	HTTPMessage msg;
	msg.command() = "upload";
	msg.fromAddress() = "[localhost]:" + toStdString(ClientPortNumber);
	msg.toAddress() = ("[localhost]:" + toStdString(ServerPortNumber));
	if (hCategoriesList->SelectedItem == nullptr)
	{
		hStatus->Text = "Please select a category";
		return;
	}
	msg.name() = toStdString(hCategoriesList->SelectedItem->ToString());
	msg.content() = std::to_string(1);
	int index = 0;
	int count = hUploadList->SelectedItems->Count;
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hUploadList->SelectedItems)
		{
			items[index++] = item;
		}
	}
	std::vector<std::string> dep;
	if (count > 0) {
		for each (String^ item in items)
		{
			dep.push_back(toStdString(item));
		}
	}
	msg.setBody(dep);
	msg.setContentLength(count);
	pChann_->postMessage(msg.buildMessage());
}

// -- function that frames an HTTPMessage for deleting a file in repository -- //

void WPFClient2::fileDeletion(Object^ sender, RoutedEventArgs^ args)
{
	if (hRepFileList->SelectedItem == nullptr)
	{
		hStatus->Text = "Please select a file";
		return;
	}
	Console::Write("\n  sending deletion request");
	hStatus->Text = "Sending deletion Request";
	HTTPMessage msg;
	msg.command() = "delete_file";
	msg.fromAddress() = "[localhost]:" + toStdString(ClientPortNumber);
	msg.toAddress() = ("[localhost]:" + toStdString(ServerPortNumber));
	std::string selection = toStdString(hRepFileList->SelectedItem->ToString());
	if (selection.find("*") != std::string::npos)
		selection = selection.substr(1);
	msg.name() = selection;
	msg.content() = "no_content";
	std::vector<std::string> vec = {};
	msg.setBody(vec);
	pChann_->postMessage(msg.buildMessage());
	hDeleteButton->IsEnabled = false;
	hPublishWithDepButton->IsEnabled = false;
	hOpenWebPageButton->IsEnabled = false;
}

// -- function that frames an HTTPMessage for deleting a file in repository -- //

void CppCliWindows::WPFClient2::OpenWebPage(Object ^ sender, RoutedEventArgs ^ args)
{
	if (hRepFileList->SelectedItem == nullptr)
	{
		hStatus->Text = "Please select a file";
		return;
	}
	if (toStdString(hRepFileList->SelectedItem->ToString()).find(".htm") == std::string::npos)
	{
		hStatus->Text = "Please click on Publish button";
		return;
	}
	Console::Write("\n  sending request for opening requested web page");
	hStatus->Text = "Sending open web page Request";
	HTTPMessage msg;
	msg.command() = "open_web_page";
	msg.fromAddress() = "[localhost]:" + toStdString(ClientPortNumber);
	msg.toAddress() = ("[localhost]:" + toStdString(ServerPortNumber));
	std::string selection = toStdString(hRepFileList->SelectedItem->ToString());
	if (selection.find("*") != std::string::npos)
		selection = selection.substr(1);
	msg.name() = selection;
	msg.content() = "../ClientDump";
	std::vector<std::string> vec = {};
	msg.setBody(vec);
	pChann_->postMessage(msg.buildMessage());
}

// -- function that frames an HTTPMessage for requesting a file publish and posts it to the MockChannel for sending -- //

void WPFClient2::publishWithDep(Object^ sender, RoutedEventArgs^ args)
{
	if (hRepFileList->SelectedItem == nullptr)
	{
		hStatus->Text = "Please select a file";
		return;
	}
	std::string type = toStdString(hRepFileList->SelectedItem->ToString());
	if (type.find(".htm") != std::string::npos || type.find(".js") != std::string::npos || type.find(".css") != std::string::npos)
	{
		hStatus->Text = "Please select another file";
		return;
	}
	Console::Write("\n  sending publish request");
	hStatus->Text = "Sending Publish With Dependencies Request";
	HTTPMessage msg;
	msg.command() = "publish";
	msg.fromAddress() = "[localhost]:" + toStdString(ClientPortNumber);
	msg.toAddress() = ("[localhost]:" + toStdString(ServerPortNumber));
	std::string selection = toStdString(hRepFileList->SelectedItem->ToString());
	if (selection.find("*") != std::string::npos)
		selection = selection.substr(1);
	msg.name() = selection;
	msg.content() = "../ClientDump";
	std::vector<std::string> vec = {};
	msg.setBody(vec);
	pChann_->postMessage(msg.buildMessage());
}

// -- accepts a std::string and returns an equivalent System::String^ -- //

String^ WPFClient2::toSystemString(std::string& str)
{
	StringBuilder^ pStr = gcnew StringBuilder();
	for (size_t i = 0; i < str.size(); ++i)
		pStr->Append((Char)str[i]);
	return pStr->ToString();
}

// -- function to update the listboxes that display the repository files -- //

void WPFClient2::addPackage(String^ msg)
{
	HTTPMessage httpMsg;
	std::string stdstringMsg = toStdString(msg);
	httpMsg.parseMessage(stdstringMsg);
	std::vector<std::string> packages_ = httpMsg.getBody();
	array<String^>^ sysPackages_ = gcnew array<String^>(packages_.size());
	hCategoriesList->Items->Clear();
	for (size_t i = 0; i < packages_.size(); i++)
	{
		sysPackages_[i] = toSystemString(packages_[i]);
		hCategoriesList->Items->Add(sysPackages_[i]);
	}
	hStatus->Text = "Updated Package List";
}

// -- function to update the listboxes that display the repository files -- //

void WPFClient2::addFiles(String^ msg)
{
	HTTPMessage httpMsg;
	std::string stdstringMsg = toStdString(msg);
	httpMsg.parseMessage(stdstringMsg);
	std::vector<std::string> packages_ = httpMsg.getBody();
	array<String^>^ sysPackages_ = gcnew array<String^>(packages_.size());
	hRepFileList->Items->Clear();
	for (size_t i = 0; i < packages_.size(); i++)
	{
		sysPackages_[i] = toSystemString(packages_[i]);
		hRepFileList->Items->Add(sysPackages_[i]);
	}
	hStatus->Text = "Updated File List";
}

// -- updates the text of the status bar with the specified String^ -- //

void WPFClient2::updateStatusBar(String^ msg)
{
	hStatus->Text = msg;
}

// -- gets a message from the MockChannel and takes appropriate actions depending on the message received -- //

void WPFClient2::getMessage()
{
	// recvThread runs this function
	while (true)
	{
		std::cout << "\n  receive thread calling getMessage()";
		std::string msg = pChann_->getMessage();
		HTTPMessage httpMsg;
		httpMsg.parseMessage(msg);
		if (httpMsg.command() == "package_list")
		{
			String^ sMsg = toSystemString(msg);
			array<String^>^ args = gcnew array<String^>(1);
			args[0] = sMsg;
			Action<String^>^ act = gcnew Action<String^>(this, &WPFClient2::addPackage);
			Dispatcher->Invoke(act, args);  // must call addText on main UI thread
		}
		else if (httpMsg.command() == "file_list")
		{
			String^ sMsg = toSystemString(msg);
			array<String^>^ args = gcnew array<String^>(1);
			args[0] = sMsg;
			Action<String^>^ act = gcnew Action<String^>(this, &WPFClient2::addFiles);
			Dispatcher->Invoke(act, args);  // must call addText on main UI thread
		}
		else if (httpMsg.command() == "published")
		{
			String^ sMsg = "Publish Successful";
			array<String^>^ args = gcnew array<String^>(1);
			args[0] = sMsg;
			Action<String^>^ act = gcnew Action<String^>(this, &WPFClient2::updateStatusBar);
			Dispatcher->Invoke(act, args);
		}
		else if (httpMsg.command() == "file_deleted")
		{
			String^ sMsg = "Delete Successful";
			array<String^>^ args = gcnew array<String^>(1);
			args[0] = sMsg;
			Action<String^>^ act = gcnew Action<String^>(this, &WPFClient2::updateStatusBar);
			Dispatcher->Invoke(act, args);
		}
		else if (httpMsg.command() == "upload")
		{
			String^ sMsg = "Uploading Successful";
			array<String^>^ args = gcnew array<String^>(1);
			args[0] = sMsg;
			Action<String^>^ act = gcnew Action<String^>(this, &WPFClient2::updateStatusBar);
			Dispatcher->Invoke(act, args);
		}
	}
}

// -- sets up the labels for the upload view -- //

void WPFClient2::setUpUploadLabels()
{
	hUploadTextBlock->Text = "Select Folder\n to Upload : ";
	hUploadTextBlock->Height = 30;
	hUploadTextBlock->Width = 120;
	hUploadGrid->SetRow(hUploadTextBlock, 0);
	hUploadGrid->SetColumn(hUploadTextBlock, 0);
	hUploadGrid->Children->Add(hUploadTextBlock);

	hCategoriesTextBlock->Text = "Select Category\n to upload : ";
	hCategoriesTextBlock->Height = 30;
	hCategoriesTextBlock->Width = 120;
	hUploadGrid->SetRow(hCategoriesTextBlock, 2);
	hUploadGrid->SetColumn(hCategoriesTextBlock, 0);
	hUploadGrid->Children->Add(hCategoriesTextBlock);
}

// -- sets up the listboxes for the upload tab -- //

void WPFClient2::setUpUploadLists()
{
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hUploadList->SelectionMode = SelectionMode::Multiple;
	hBorder1->Child = hUploadList;
	hUploadGrid->SetRow(hBorder1, 0);
	hUploadGrid->SetColumn(hBorder1, 1);
	hUploadGrid->Children->Add(hBorder1);

	Border^ hBorder2 = gcnew Border();
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hCategoriesList->SelectionMode = SelectionMode::Single;
	hBorder2->Child = hCategoriesList;
	hUploadGrid->SetRow(hBorder2, 2);
	hUploadGrid->SetColumn(hBorder2, 1);
	hUploadGrid->Children->Add(hBorder2);
}

// -- sets up the buttons for the upload tab -- //

void WPFClient2::setUpUploadButtons()
{
	// Select Files button
	hFolderBrowseButton->Content = "Select Files";
	hFolderBrowseButton->Height = 30;
	hFolderBrowseButton->Width = 120;
	hFolderBrowseButton->BorderThickness = Thickness(2);
	hFolderBrowseButton->BorderBrush = Brushes::Black;
	hUploadGrid->SetRow(hFolderBrowseButton, 0);
	hUploadGrid->SetColumn(hFolderBrowseButton, 2);
	hUploadGrid->Children->Add(hFolderBrowseButton);

	// Get Repository Categories List button.
	hUploadGetFileListButton->Content = "Get Categories List";
	hUploadGetFileListButton->Height = 30;
	hUploadGetFileListButton->Width = 120;
	hUploadGetFileListButton->BorderThickness = Thickness(2);
	hUploadGetFileListButton->BorderBrush = Brushes::Black;
	hUploadGrid->SetRow(hUploadGetFileListButton, 2);
	hUploadGrid->SetColumn(hUploadGetFileListButton, 2);
	hUploadGrid->Children->Add(hUploadGetFileListButton);

	// Upload button
	hUploadButton->Content = "Upload";
	hUploadButton->Height = 30;
	hUploadButton->Width = 120;
	hUploadButton->BorderThickness = Thickness(2);
	hUploadButton->BorderBrush = Brushes::Black;
	hUploadGrid->SetRow(hUploadButton, 3);
	hUploadGrid->SetColumn(hUploadButton, 1);
	hUploadGrid->Children->Add(hUploadButton);

}

// -- sets up the upload tab and its controls -- //

void WPFClient2::setUpUploadView()
{
	Console::Write("\n  setting up Upload view");
	hUploadGrid->Margin = Thickness(20);
	hUploadTab->Content = hUploadGrid;
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(200);
	hUploadGrid->RowDefinitions->Add(hRow1Def);
	RowDefinition^ spacer = gcnew RowDefinition();
	spacer->Height = GridLength(20);
	hUploadGrid->RowDefinitions->Add(spacer);
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(200);
	RowDefinition^ hRow2Def2 = gcnew RowDefinition();
	//hRow2Def2->Height = GridLength(75);
	hUploadGrid->RowDefinitions->Add(hRow2Def);
	hUploadGrid->RowDefinitions->Add(hRow2Def2);
	ColumnDefinition^ hColumn1Def = gcnew ColumnDefinition();
	hColumn1Def->Width = GridLength(100);
	hUploadGrid->ColumnDefinitions->Add(hColumn1Def);
	ColumnDefinition^ hColumn2Def = gcnew ColumnDefinition();
	hColumn2Def->Width = GridLength(450);
	hUploadGrid->ColumnDefinitions->Add(hColumn2Def);
	ColumnDefinition^ hColumn3Def = gcnew ColumnDefinition();
	//hColumn3Def->Width = GridLength(500);
	hUploadGrid->ColumnDefinitions->Add(hColumn3Def);

	setUpUploadLabels();
	setUpUploadLists();
	setUpUploadButtons();

	hFolderBrowserDialog->ShowNewFolderButton = false;
	hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
}

// -- browses the local file system and displays source files in selected directory -- //

void WPFClient2::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Browsing for folder";
	hUploadList->Items->Clear();
	System::Windows::Forms::DialogResult result;
	result = hFolderBrowserDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hFolderBrowserDialog->SelectedPath;
		array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.h");
		for (int i = 0; i < files->Length; ++i)
			hUploadList->Items->Add(files[i]);
		array<String^>^ files1 = System::IO::Directory::GetFiles(path, L"*.cpp");
		for (int i = 0; i < files1->Length; ++i)
			hUploadList->Items->Add(files1[i]);
	}
}

// -- logs a window loaded message on the console -- //

void WPFClient2::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  Window loaded");
}

// -- logs a window closed message on the console and shuts down the MockChannel -- //

void WPFClient2::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
	Console::Write("\n  Window closing");
	pChann_->stop();
}

[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
	Console::WriteLine(L"\n Starting WPFClient");
	std::cout << "\n\n-----------------------------Requirement 1-------------------------------------\n\n";
	std::cout << "    Used Windows Presentation Foundation (WPF) to provide a required client1 Graphical User Interface (GUI).\n";
	Application^ app = gcnew Application();
	app->Run(gcnew WPFClient2(args));
	Console::WriteLine(L"\n\n");
}