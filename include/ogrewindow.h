#ifndef OGREWINDOW_H
#define OGREWINDOW_H

#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>

#include "ogrewidget.h"

class OgreWindow : public Gtk::Window
{

public:
	OgreWindow();
	virtual ~OgreWindow();
	virtual bool on_delete_event(GdkEventAny* event);
	bool hasExited() { return mExited; } 

	virtual bool on_key_press_event(GdkEventKey *event);
	virtual bool on_key_release_event(GdkEventKey *event); 

protected:
	OgreWidget mOgreWidget;
	bool mExited;
};

#endif
