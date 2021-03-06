// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// $Id: main.C,v 1.20.8.3 2007-03-26 07:11:33 amoll Exp $
//

// order of includes is important: first qapplication, than BALL includes
#include <QtGui/qapplication.h>
#include <BALL/CONFIG/config.h>

#ifdef BALL_USE_GLEW
#	include <GL/glew.h>
#endif

#include <QtGui/qmessagebox.h>
#include <QtGui/QSplashScreen>
#include <QtOpenGL/qgl.h>

#include "mainframe.h"
#include "splash.h"
#include <BALL/SYSTEM/path.h>
#include <BALL/SYSTEM/directory.h>
#include <BALL/COMMON/logStream.h>

#include <iostream>

void logMessages(QtMsgType type, const char *msg)
{
	BALL::String s(msg);
	if (s.hasPrefix("QTextBrowser")) return;

	switch ( type ) {
		case QtDebugMsg:
				BALL::Log.info() << msg << std::endl;
				break;
		case QtWarningMsg:
				BALL::Log.warn() << msg << std::endl;
				break;
		case QtFatalMsg:
				fprintf( stderr, "Fatal: %s\n", msg );
				abort();                    // deliberately core dump
		case QtCriticalMsg:
				fprintf( stderr, "Critical: %s\n", msg );
				abort();                    // deliberately core dump
	}
}


// uncomment this to use debugging to std::cout!
//#undef BALL_PLATFORM_WINDOWS

#ifndef BALL_PLATFORM_WINDOWS
int main(int argc, char **argv)
{
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR cmd_line, int)
{
	int argc = __argc;
	char** argv = __argv;
#endif

	qInstallMsgHandler(logMessages);

	putenv("BALL_RETURN_VALUE=");
	QApplication application(argc, argv);

  QPixmap splash_pm(splash_xpm);
  QSplashScreen* splash = new QSplashScreen(splash_pm);
  splash->show();

	// =============== testing for opengl support ======================================
	if (!QGLFormat::hasOpenGL())
	{
		QMessageBox::critical(0, "Error while starting BALLView", 
				"Your computer has no OpenGL support, please install the correct drivers. Aborting for now...",
				QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
		return -1;
	}

	// =============== testing if we can write in current directoy =====================
	BALL::String home_dir = BALL::Directory::getUserHomeDir();

	if (home_dir == "")
	{
		try
		{
			BALL::String temp_file_name;
			BALL::File::createTemporaryFilename(temp_file_name);
			BALL::File out(temp_file_name, std::ios::out);
			out << "test" << std::endl;
			out.remove();
		}
		catch(...)
		{
			QMessageBox::warning(0, "Error while starting BALLView",
					QString("You dont have write access to the current working directory\n") + 
					"and BALLView can not find your home directory. This can cause\n" + 
					"unexpected behaviour. Please start BALLView from your homedir with\n" + 
					"absolute path (e.g. C:\\Programs\\BALLView\\BALLView).\n");
		}
	}

	// =============== initialize Mainframe ============================================
	// Create the mainframe.
	BALL::Mainframe mainframe(0, "Mainframe");

	// can we use the users homedir as working dir?
	if (home_dir != "")
	{
		mainframe.setWorkingDir(home_dir);
	}

	// Register the mainfram (required for Python support).
	mainframe.setIdentifier("Mainframe");
	mainframe.registerThis();

	// Show the main window.
	mainframe.show();

	// =============== parsing command line arguments ==================================
	// If there are additional command line arguments, interpret them as files to open or logging flag.
	for (BALL::Index i = 1; i < argc; ++i)
	{
		BALL::String argument(argv[i]);
		if (argument == "-l") 
		{
			mainframe.enableLoggingToFile();
			continue;
		}

		mainframe.openFile(argument);
	}

	// enable ending of program from python script
	if (mainframe.isAboutToQuit()) 
	{
		mainframe.aboutToExit();
		return 0;
	}
	
	// Remove the splashscreen
	splash->finish(&mainframe);
	delete splash;

  // Hand over control to the application.
  int value = application.exec();
	char*	return_value = getenv("BALL_RETURN_VALUE");
	if (return_value != 0)
	{
		try
		{
			value = BALL::String(return_value).toInt();
		}
		catch(...)
		{
		}
	}

	return value;
}
