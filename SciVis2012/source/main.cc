// Usage: Drag with the mouse to add smoke to the fluid. This will also move a "rotor" that disturbs
//        the velocity field at the mouse location. Press the indicated keys to change options
//--------------------------------------------------------------------------------------------------

#include <iostream>
#include "Application.h"

using namespace std;

int main(int argc, char** argv)
{	
	try
	{		
		Application::outputUsage();
		Application::initialize(&argc, argv);
	}
	catch(exception &ex)
	{
		//cerr << "an exception occurred: " << ex.what() << '\n';		
	}
	catch(...)
	{
		cerr << "an exception occurred.\n";		
	}
}