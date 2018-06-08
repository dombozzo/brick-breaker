

project: project.cpp
	g++ project.cpp gfxnew.o -std=c++11 -lX11 -oproject

clean: 
	rm project
