cd x64\Debug\
TestExecutive.exe
start MsgServer.exe 8080 ..\..\Repository
cd ..\..\Debug\
start Client2.exe 8060 8080
start CppCli-WPF-App.exe 8040 8080 ..\TestSuit



