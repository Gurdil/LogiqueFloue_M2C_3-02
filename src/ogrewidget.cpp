#include "ogrewidget.h"
 
    //#include <gdkmm/drawable.h>
	#include <gdk/gdk.h>
	#include <gdkmm/window.h>
    #include <gdkmm/general.h>
    #include <iostream>
    #include <cstring>
#include <gdk/gdkx.h>
#include <OGRE/OgreConfigFile.h>
 
    #ifdef WIN32
       #include <gdk/gdkwin32.h>
    #endif
 
    OgreWidget::OgreWidget() :
      Glib::ObjectBase("ogrewidget"),
      Gtk::Widget(),
      mRenderWindow(0), mCamera(0), mSceneMgr(0), mViewport(0)
    {
    	set_has_window(false);
      set_redraw_on_allocate(false);
      std::cout << "GType name: " << G_OBJECT_TYPE_NAME(gobj()) << std::endl;
      this->set_size_request(800, 600);
    }
 
    OgreWidget::~OgreWidget()
    {
    }
 
    void OgreWidget::on_size_request(Gtk::Requisition* requisition)
    {

      *requisition = Gtk::Requisition();
 
      requisition->width = 800;
      requisition->height = 600;

    }
 
    void OgreWidget::on_size_allocate(Gtk::Allocation& allocation)
    {
      set_allocation(allocation);
 
      if(mRefGdkWindow)
      {
        mRefGdkWindow->move_resize( allocation.get_x(), allocation.get_y(),
                allocation.get_width(), allocation.get_height() );
      }
 
      if (mRenderWindow)
      {
          mRenderWindow->windowMovedOrResized();
          mCamera->setAspectRatio(Ogre::Real(allocation.get_width()) / Ogre::Real(allocation.get_height()));     
          on_expose_event(NULL);
      }
    }
 
    void OgreWidget::on_map()
    {
      //Call base class:
      Gtk::Widget::on_map();
    }
 
    void OgreWidget::on_unmap()
    {
      //Call base class:
      Gtk::Widget::on_unmap();
    }
 
    void setupResources(void)
        {
            // Load resource paths from config file
    	Ogre::ConfigFile cf;
    #if OGRE_DEBUG_MODE
            cf.load("resources_d.cfg");
    #else
    		cf.load("resources.cfg");
    #endif

            // Go through all sections & settings in the file
    		Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    		Ogre::String secName, typeName, archName;
            while (seci.hasMoreElements())
            {
                secName = seci.peekNextKey();
                Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
                Ogre::ConfigFile::SettingsMultiMap::iterator i;
                for (i = settings->begin(); i != settings->end(); ++i)
                {
                    typeName = i->first;
                    archName = i->second;
    #if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
                    // OS X does not set the working directory relative to the app,
                    // In order to make things portable on OS X we need to provide
                    // the loading with it's own bundle path location
    				if (!StringUtil::startsWith(archName, "/", false)) // only adjust relative dirs
    					archName = String(macBundlePath() + "/" + archName);
    #endif
    				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                        archName, typeName, secName);

                }
            }
        }

    void OgreWidget::on_realize()
    {
       //Call base class:
       Gtk::Widget::on_realize();
 
       Gtk::Allocation allocation = get_allocation();
 
       if(!mRefGdkWindow)
       {
          //Create the GdkWindow:
          GdkWindowAttr attributes;
          memset(&attributes, 0, sizeof(attributes));
 
          //Set initial position and size of the Gdk::Window:
          attributes.x = allocation.get_x();
          attributes.y = allocation.get_y();
          attributes.width = allocation.get_width();
          attributes.height = allocation.get_height();
 
          attributes.event_mask = get_events () | Gdk::EXPOSURE_MASK | Gdk::ALL_EVENTS_MASK ;
          attributes.window_type = GDK_WINDOW_CHILD;
          attributes.wclass = GDK_INPUT_OUTPUT;
 
          mRefGdkWindow = Gdk::Window::create(get_window(), &attributes, GDK_WA_X | GDK_WA_Y);
 
       }
 
       if (!mRenderWindow)
       {
          Ogre::NameValuePairList params;
    #ifdef WIN32
       params["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)GDK_WINDOW_HWND(mRefGdkWindow->gobj()));
    #else
    GdkWindow* parent = mRefGdkWindow->gobj();
       GdkDisplay* display = gdk_window_get_display(parent);
       GdkScreen* screen = gdk_window_get_screen(parent);
 
    Display* xdisplay = GDK_DISPLAY_XDISPLAY(display);
       Screen* xscreen = GDK_SCREEN_XSCREEN(screen);
       int screen_number = XScreenNumberOfScreen(xscreen);
       XID xid_parent = GDK_WINDOW_XID(parent);
 
       params["externalWindowHandle"] =
          Ogre::StringConverter::toString(reinterpret_cast<unsigned long>(xdisplay)) + ":" +
          Ogre::StringConverter::toString(static_cast<unsigned int>(screen_number)) + ":" +
          Ogre::StringConverter::toString(static_cast<unsigned long>(xid_parent));
    #endif
          mRenderWindow = Ogre::Root::getSingleton().createRenderWindow("Gtk+Ogre Widget",
             allocation.get_width(), allocation.get_height(), false, &params);
 
         mRenderWindow->setAutoUpdated(false);
 
         set_has_window(true);
 
         set_window(mRefGdkWindow);
 
          set_double_buffered(true);
          //make the widget receive expose events
         mRefGdkWindow->set_user_data(gobj());
         //mRefGdkWindow->set_back_pixmap(Glib::RefPtr<Gdk::Pixmap>(),false);
 
         setupResources();
         Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

          createScene();     
 
          // Start idle function for frame update/rendering
          Glib::signal_idle().connect( sigc::mem_fun(*this, &OgreWidget::on_idle) );
 
       }
    }
 
    void OgreWidget::on_unrealize()
    {
      mRefGdkWindow.clear();
      //Call base class:
      Gtk::Widget::on_unrealize();
    }
 
    bool OgreWidget::on_expose_event(GdkEventExpose* event)
    {
 
      if (mRenderWindow)
      {
          Ogre::Root::getSingletonPtr()->_fireFrameStarted();
          mRenderWindow->update();
          Ogre::Root::getSingletonPtr()->_fireFrameEnded(); 
 
      } 
      return true;
    }
 
    bool OgreWidget::on_idle()
    {
        on_expose_event(0);
        return true;
    }
 
    void OgreWidget::createScene()
    {
        // Set default mipmap level & texture filtering
        Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);
 
        // Create scene manager
        mSceneMgr = Ogre::Root::getSingletonPtr()->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
 
        // Create the camera
        mCamera = mSceneMgr->createCamera("Camera");
 
        // Create one viewport, entire window
        mViewport = mRenderWindow->addViewport(mCamera);
 
        // Alter the camera aspect ratio to match the viewport
        mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));

        mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
        Ogre::Entity* ogreEntity = mSceneMgr->createEntity("robot.mesh");
        Ogre::SceneNode* ogreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        ogreNode->attachObject(ogreEntity);
        Ogre::Light* light = mSceneMgr->createLight("MainLight");
        light->setPosition(20, 80, 50);
        mViewport->setBackgroundColour (Ogre::ColourValue(0, 1, 0));
    }
 
    bool OgreWidget::on_motion_notify_event(GdkEventMotion *event) {
       std::cout << ".";
       return true;
    }
 
    bool OgreWidget::on_button_press_event(GdkEventButton *event) {
       std::cout << "button press\n";
       return true;
    }
 
    bool OgreWidget::on_button_release_event(GdkEventButton *event) {
       std::cout << "button release\n";
       return true;
    }
