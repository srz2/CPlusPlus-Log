//  Created by Steven Zilberberg on 11/25/14.
//  Copyright (c) 2014 Steven Zilberberg. All rights reserved.
//  File: Log.h
//
//  This Class will construct and allow interaction to make a log file which can
//  be used to assist in the debugging of issues which may arise or used for the
//  confirmation of actions
//      
//  This class has a SINGLETON implementation so it doesn't need to be passed
//  around by everything.
//
//  *******************************************************************************

#ifndef __SRZ_LOG__
#define __SRZ_LOG__

#include <stdio.h>
#include <fstream>
#include <string.h>
#include <ctime>
#include <pthread.h>

//Determine if log will use a release or debug path
#ifndef SRZ_LOG_USING_RELEASE
#define SRZ_LOG_USING_RELEASE       false
#endif

//Max size of the log file before it starts anew
#ifndef SRZ_LOG_MAX_SIZE
#define SRZ_LOG_MAX_SIZE	          1024			//Kilobytes
#endif

//Max size of an individual log message
#ifndef SRZ_LOG_MAX_MESSAGE_SIZE
#define SRZ_LOG_MAX_MESSAGE_SIZE    100		    //Characters/Bytes
#endif

//The name of the log file
#ifndef SRZ_LOG_FILE_NAME
#define SRZ_LOG_FILE_NAME           "log.txt"
#endif

//The path for the log file if in debug mode
#ifndef SRZ_LOG_DEBUG_FILE_PATH
#define SRZ_LOG_DEBUG_FILE_PATH     "./"
#endif

//The path for the log file if in release mode
#ifndef SRZ_LOG_RELEASE_FILE_PATH
#define SRZ_LOG_RELEASE_FILE_PATH   "./"
#endif

//If log messages will be written to the console
#ifndef SRZ_LOG_TO_CONSOLE
#define SRZ_LOG_TO_CONSOLE          true
#endif

class Log
{
private:
	static Log * instance;					//Singleton Variable containing instance
	Log(time_t time);						    //Private constructor

	Log(const Log& other);					//Private copy constructor
	Log& operator=(const Log& other);

	pthread_mutex_t lockWrite;
	pthread_mutex_t lockEnable;
	pthread_mutex_t lockIncrementChange;

	char logPath[256];
	bool isEnabled;
	unsigned int indentation;
  std::ofstream outFile;
  
  struct tm * startTime;
  struct tm * getTime();
  char * formatCurrentTime(struct tm *);
  char * getCurrentTime();

  //Write to the Console
  void writeToConsole(char * time, const char * msg);

  //Write message to the file
  void writeToFile(char * time, const char * msg);

public:
  static Log * Instance();
  ~Log();
  
  //Enable or disable writing to the log
  void setEnabled(bool enabled);

  //Increase the indentation by 1
  int increaseIndentation();

  //Decrease the indentation by 1
  int decreaseIndentation();

  //Write a message to the log  
  void write(const char * msg);

  //Write to the log with one variable
  template <class a>
  void write(const char * msg, a var1)
  {
  	if(strlen(msg) > SRZ_LOG_MAX_MESSAGE_SIZE)
		{
			char buf[SRZ_LOG_MAX_MESSAGE_SIZE];
			memset(buf, '\0', SRZ_LOG_MAX_MESSAGE_SIZE);
			sprintf(buf, "LOG MSG IGNORED(Too Long):\'%*.*s...\'", 50, 50, msg);
			write(buf);
			return;
		}

  	char buf[SRZ_LOG_MAX_MESSAGE_SIZE];
  	memset(buf, '\0', SRZ_LOG_MAX_MESSAGE_SIZE);
  	sprintf(buf, msg, var1);
  	write(buf);
  }

  //Write to the log with two variables
  template <class a, class b>
  void write(const char * msg, a var1, b var2)
  {
  	if(strlen(msg) > SRZ_LOG_MAX_MESSAGE_SIZE)
		{
			char buf[SRZ_LOG_MAX_MESSAGE_SIZE];
			memset(buf, '\0', SRZ_LOG_MAX_MESSAGE_SIZE);
			sprintf(buf, "LOG MSG IGNORED(Too Long):\'%*.*s...\'", 50, 50, msg);
			write(buf);
			return;
		}

  	char buf[SRZ_LOG_MAX_MESSAGE_SIZE];
  	memset(buf, '\0', SRZ_LOG_MAX_MESSAGE_SIZE);
  	sprintf(buf, msg, var1, var2);
  	write(buf);
  }

  //Write to the log with three variables
  template <class a, class b, class c>
  void write(const char * msg, a var1, b var2, c var3)
  {
  	if(strlen(msg) > SRZ_LOG_MAX_MESSAGE_SIZE)
		{
			char buf[SRZ_LOG_MAX_MESSAGE_SIZE];
			memset(buf, '\0', SRZ_LOG_MAX_MESSAGE_SIZE);
			sprintf(buf, "LOG MSG IGNORED(Too Long):\'%*.*s...\'", 50, 50, msg);
			write(buf);
			return;
		}

  	char buf[SRZ_LOG_MAX_MESSAGE_SIZE];
  	memset(buf, '\0', SRZ_LOG_MAX_MESSAGE_SIZE);
  	sprintf(buf, msg, var1, var2, var3);
  	write(buf);
  }

  //Write to the log with four variables
  template <class a, class b, class c, class d>
	void write(const char * msg, a var1, b var2, c var3, d var4)
	{
  	if(strlen(msg) > SRZ_LOG_MAX_MESSAGE_SIZE)
		{
			char buf[SRZ_LOG_MAX_MESSAGE_SIZE];
			memset(buf, '\0', SRZ_LOG_MAX_MESSAGE_SIZE);
			sprintf(buf, "LOG MSG IGNORED(Too Long):\'%*.*s...\'", 50, 50, msg);
			write(buf);
			return;
		}

		char buf[SRZ_LOG_MAX_MESSAGE_SIZE];
		memset(buf, '\0', SRZ_LOG_MAX_MESSAGE_SIZE);
		sprintf(buf, msg, var1, var2, var3, var4);
		write(buf);
	}
};

#endif /* defined(__SRZ_LOG__) */
