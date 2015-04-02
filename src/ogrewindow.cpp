#include "ogrewindow.h"
    #include <iostream>
 
    OgreWindow::OgreWindow() :
        mOgreWidget(),
        mExited(false)
    {
      set_border_width(10);
 
      Gtk::VBox *vb = new Gtk::VBox(false,10);
 
      Gtk::Button *mb = new Gtk::Button("Some Button");
 
      vb->pack_start(*mb,true,true,10);
      vb->pack_start(mOgreWidget,true,true,10);
 
      add(*vb); 
      show_all();
    }
 
    OgreWindow::~OgreWindow()
    {
    }
 
    bool OgreWindow::on_delete_event(GdkEventAny* event)
    {
        mExited = true;
        return false;
    }
 
    bool OgreWindow::on_key_press_event(GdkEventKey *event) {
       std::cout << "keydown\n";
       return true;
    }
 
    bool OgreWindow::on_key_release_event(GdkEventKey *event) {
       std::cout << "keyup\n";
       return true;
    }
