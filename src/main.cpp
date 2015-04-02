#include <gtkmm/main.h>
#include "ogrewindow.h"

int main (int argc, char *argv[])
{
	Ogre::Root* root = new Ogre::Root();

	if (!root->showConfigDialog())
		return -1;

	root->initialise(false);

	Gtk::Main kit(argc, argv);

	OgreWindow oWindow;
	oWindow.show();

	while (!oWindow.hasExited())
	{ 
		kit.iteration();
		// you could also call renderOneFrame() here instead.
	}       

	delete root;   
	return 0;
}
