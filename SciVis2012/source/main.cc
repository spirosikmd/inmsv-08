// Usage: Drag with the mouse to add smoke to the fluid. This will also move a "rotor" that disturbs
//        the velocity field at the mouse location. Press the indicated keys to change options
//--------------------------------------------------------------------------------------------------

#include <iostream>
#include "Application.h"
#include "Colormap.h"

using namespace std;

int main(int argc, char** argv)
{	
    
    Colormap *rainbow = new Colormap();
    rainbow->putColor(WHITE,255);
    //rainbow->putColor(WHITE,2);
    //rainbow->putColor(WHITE,4);
    //rainbow->putColor(WHITE,6);
    //rainbow->putColor(WHITE,8);
    //rainbow->putColor(BLACK,0);
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