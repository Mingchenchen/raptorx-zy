// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// $Id: server.C,v 1.18.16.1 2007-03-25 22:02:27 oliver Exp $

#include <BALL/VIEW/KERNEL/server.h>
#include <BALL/CONCEPT/client.h>
#include <BALL/VIEW/KERNEL/mainControl.h>
#include <BALL/VIEW/DIALOGS/preferences.h>
#include <BALL/VIEW/DIALOGS/serverPreferences.h>
#include <BALL/SYSTEM/socket.h>
#include <BALL/FORMAT/INIFile.h>

#include <QtGui/qstatusbar.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qtooltip.h>

using namespace std;

namespace BALL
{
	namespace VIEW
	{
		const char* Server::mini_ray_xpm_[] =
		{
        "16 14 4 1",
        "   c None",
        ".  c black",
        "X  c yellow",
        "o  c gray50",
        "     .........  ",
        "     .XXXXXX.o  ",
        "    .XXXXXX.o   ",
        "    .XXXXX.o    ",
        "   .XXXXX.o     ",
        "   .XXXX.....   ",
        "  .XXXXXXXX.o   ",
        "  .....XXX.o    ",
        "   oo.XXX.o     ",
        "     .XX.o      ",
        "    .XX.o       ",
        "    .X.o        ",
        "   .X.o         ",
        "   ..o          "
		};        
  
		Server::NotCompositeObject::NotCompositeObject(const char* file, int line)
			throw()
			:	Exception::GeneralException(file, line, string("NotCompositeObject"), string("received an non composite object!"))
		{
		}

		Server::Server(QWidget* parent, const char* name)
				throw()
			:	QTimer(parent),
				ModularWidget(name),
				object_creator_(0),
				composite_hashmap_(),
				iostream_socket_(0),
				sock_inet_buf_(0),
				port_(VIEW_DEFAULT_PORT),
				server_preferences_(0),
				server_icon_(0)
		{
			#ifdef BALL_VIEW_DEBUG
				Log.error() << "new Server " << this << std::endl;
			#endif

			// register ModularWidget
			registerWidget(this);
			
			unregisterObjectCreator();
		}

		Server::Server(const Server&)
			: QTimer(),
				ModularWidget()
		{
		}

		Server::~Server()
			throw()
		{
			#ifdef BALL_VIEW_DEBUG
				Log.error() << "Destructing object " << this << " Server" << std::endl;
			#endif 

			ConnectionObject::destroy();

			if (object_creator_ != 0) delete object_creator_;
		}

		void Server::clear()
			throw()
		{
			QTimer::stop();
			ConnectionObject::clear();
		}


		// initializes a new socket and starts the timer
		void Server::activate()
			throw()
		{
			static SockInetBuf sock_inet_buf(SocketBuf::sock_stream);
			
			// if the timer is already running, clear it and close the
			// socket first
			if (isActive())
			{
				stop();
				sock_inet_buf.close();
			}
			
			if (sock_inet_buf.bind(0, port_) != 0)
			{
				Log.error() << "VIEW::Server: cannot bind to port " << port_ << endl;
				return;
			}

			sock_inet_buf_ = &sock_inet_buf;

			Log.info() << "VIEW::Server: listening at port " 
								 << sock_inet_buf_->localhost() << ":"
								 << sock_inet_buf_->localport() << endl;

			sock_inet_buf_->listen();

			// check once per second
			setInterval(1000);
			start();
		}

		void Server::deactivate()
				throw()
		{
			if (isActive())
			{
				Log.info() << "VIEW::Server: stopped." << endl;
				QTimer::stop();
				sock_inet_buf_->close();
			}
		}
			
		int Server::getPort() const
			throw()
		{
			return port_;
		}
		
		void Server::setPort(const int port)
			throw()
		{
			port_ = port;
		}
		
		void Server::initializeWidget(MainControl& main_control)
			throw()
		{
			server_icon_ = new QLabel(main_control.statusBar());
			main_control.statusBar()->addPermanentWidget(server_icon_);
//   			QToolTip::add(server_icon_, "VIEW server status");
			QPixmap icon(mini_ray_xpm_);

  		server_icon_->setFrameShape(QLabel::NoFrame);
			server_icon_->setPixmap(icon);
			server_icon_->setMaximumSize(14,20);
			server_icon_->setMinimumSize(14,20);
			server_icon_->show();
 		}

		void Server::finalizeWidget(MainControl& main_control)
			throw()
		{
			main_control.statusBar()->removeWidget(server_icon_);	
			delete server_icon_;
		}
			
		void Server::initializePreferencesTab(Preferences &preferences)
			throw()
		{
			server_preferences_ = new ServerPreferences();

			preferences.insertEntry(server_preferences_);
		}

		void Server::finalizePreferencesTab(Preferences &preferences)
			throw()
		{
			if (server_preferences_ != 0)
			{
				preferences.removeEntry(server_preferences_);
				server_preferences_ = 0;
			}
		}
		
		void Server::applyPreferences()
			throw()
		{
			if (server_preferences_ == 0) return;

			// get server mode
			if (server_preferences_->getServerStatus())
			{
				// retrieve the port number
				int port = server_preferences_->getPort();

				// set the port and active the server
				setPort(port);
				activate();

				// adjust the tool tip and update the server icon
				QString tip;
				tip.sprintf("VIEW Server listening on port %d", port); 
//   				QToolTip::add(server_icon_, tip);
				server_icon_->show();
			}
			else
			{
				// stop the server
				deactivate();

				// hide the icon
				server_icon_->hide();
			}
		}
		
		bool Server::isValid() const
			throw()
		{
			return (ConnectionObject::isValid());
		}

		void Server::dump(ostream& s, Size depth) const
			throw()
		{
			BALL_DUMP_STREAM_PREFIX(s);
			
			BALL_DUMP_DEPTH(s, depth);
			BALL_DUMP_HEADER(s, this, this);

			ConnectionObject::dump(s, depth + 1);

			BALL_DUMP_STREAM_SUFFIX(s);
		}

		// main event loop
	  void Server::timer()
		{
			unsigned int command;

			// wait until socket is read ready
			if (!sock_inet_buf_->is_readready(0,20))
			{
				return;
			}

			// open stream socket
			IOStreamSocket iostream_socket(sock_inet_buf_->accept());

			// reading command
			iostream_socket >> command;
			
			// process commands
			switch (command)
			{
				case Client::COMMAND__SEND_OBJECT:
					sendObject(iostream_socket);
				break;

   			default:
					Log.info() << "Server: unkown command." << endl;
					break;
			}
		}

	  void Server::sendObject(IOStreamSocket &iostream_socket)
				throw(Server::NotCompositeObject)
    {
			Log.info() << "Server: receiving object ... " << endl;

			unsigned long object_handle;
			
			// get object handle
			iostream_socket >> object_handle;
			
			Log.info() << "creating object: object_creator_ = " << object_creator_ << endl;

			// use specified object creator for inserting the object in the scene
			Composite* new_composite_ptr = object_creator_->operator()(iostream_socket);

			if (new_composite_ptr == 0)
			{
				throw NotCompositeObject(__FILE__, __LINE__);
			}

			received_composite_ = new_composite_ptr;

			// get composite with handle
 			CompositeHashMap::Iterator iterator = composite_hashmap_.find(object_handle);

		 	// already in hashmap ?
			if (iterator != composite_hashmap_.end())
			{
				getMainControl()->remove(*iterator->second);
					
				// remove old composite from hashmap
				composite_hashmap_.erase(object_handle);
			}

			// insert new composite 
			composite_hashmap_.insert(CompositeHashMap::ValueType(object_handle, new_composite_ptr));
			getMainControl()->insert(*new_composite_ptr);
    }


#		ifdef BALL_NO_INLINE_FUNCTIONS
#			include <BALL/VIEW/KERNEL/server.iC>
#		endif 

} } // namespaces
