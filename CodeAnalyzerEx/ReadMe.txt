ReadMe.txt

The directories where the .exe is getting generated is different when running the 
code from visual studio and running using developer propt. 

So need to set relative path accordingly as follows before running the perticular
functionality.

if current directory is x64/Debug/
set path for msg.content() as ../../ClientDump

if current directory is Debug/
set path for msg.content() as ../ClientDump 

Need to set relative path according to current directories.

 