//  Created by Steven Zilberberg on 11/25/14.
//  Copyright (c) 2014 Steven Zilberberg. All rights reserved.
//  File: Log.cpp
//
//  The implementation of Log.h
//      
//  *******************************************************************************

#include "Log.h"

Log * Log::instance = NULL;

Log * Log::Instance()
{
	if(!instance)
		instance = new Log(time(NULL));
	return instance;
}

Log::Log(time_t time)
{
	this->startTime = localtime(&time);
	this->indentation = 0;
	this->isEnabled = true;

	//Initiate Mutexes
	pthread_mutex_init(&this->lockWrite, NULL);
	pthread_mutex_init(&this->lockEnable, NULL);
	pthread_mutex_init(&this->lockIncrementChange, NULL);	

	//Create Log file path
	const char * fileName = SRZ_LOG_FILE_NAME;
	if(SRZ_LOG_USING_RELEASE)
		strcpy(logPath, SRZ_LOG_RELEASE_FILE_PATH);
	else
		strcpy(logPath, SRZ_LOG_DEBUG_FILE_PATH);

	int pathSize = strlen(logPath);
	for(unsigned int c = pathSize; c < pathSize + strlen(fileName); c++)
		logPath[c] = fileName[c - pathSize];

	outFile.open(logPath);
  write("Log has started...");
}

Log::~Log()
{
  write("Log has been closed");
  outFile.close();
}

Log::Log(const Log& other)
{
	this->indentation = other.indentation;
	this->startTime = other.startTime;
	this->isEnabled = other.isEnabled;
	this->lockWrite = other.lockWrite;
	this->lockEnable = other.lockEnable;
	this->lockIncrementChange = other.lockIncrementChange;

	for(unsigned int c = 0; c < strlen(other.logPath); c++)
			this->logPath[c] = other.logPath[c];
}

Log& Log::operator=(const Log& other)
{
	this->indentation = other.indentation;
	this->startTime = other.startTime;
	this->isEnabled = other.isEnabled;
	this->lockWrite = other.lockWrite;
	this->lockEnable = other.lockEnable;
	this->lockIncrementChange = other.lockIncrementChange;

	for(unsigned int c = 0; c < strlen(other.logPath); c++)
		this->logPath[c] = other.logPath[c];

  return *this;
}

struct tm * Log::getTime()
{
	time_t t = time(NULL);
	return localtime(&t);
}

char * Log::formatCurrentTime(struct tm * t)
{
	char * buffer =  new char[32];
	strftime(buffer, 32, "%m/%d/%Y-%H:%M:%S", t);

	return buffer;					//This should produce a pointer but valgrind says the delete is mismatched...
}

char * Log::getCurrentTime()
{
	return formatCurrentTime(getTime());
}

void Log::setEnabled(bool enabled)
{
	pthread_mutex_lock(&lockEnable);
	if(!enabled)
		write("Log file has been disabled.");
	else
		write("Log file has been enabled.");
	isEnabled = enabled;
	pthread_mutex_unlock(&lockEnable);
}

int Log::increaseIndentation()
{
	pthread_mutex_lock(&lockIncrementChange);
	++indentation;
	pthread_mutex_unlock(&lockIncrementChange);
	return indentation;
}

int Log::decreaseIndentation()
{
	if(indentation > 0)
	{
		pthread_mutex_lock(&lockIncrementChange);
		--indentation;
		pthread_mutex_unlock(&lockIncrementChange);
	}
	return indentation;
}

void Log::write(const char * msg)
{
	if(!isEnabled){
		return;
	}

	//Three Spaces = 1 indent
	char * indents = new char[indentation * 3 + 1];
	memset(indents, '\0', indentation * 3 + 1);
	for(unsigned int c = 0; c < indentation * 3; c += 3)
	{
		indents[c + 0] = ' ';
		indents[c + 1] = ' ';
		indents[c + 2] = ' ';
	}

	char * newMsg = new char[strlen(msg) + strlen(indents) + 1];
	memset(newMsg, '\0', strlen(msg) + strlen(indents) + 1);
	strcpy(newMsg, indents);
	for(unsigned int c = 0; c < strlen(msg); c++)
		newMsg[c + strlen(indents)] = msg[c];

	//msg = indents + msg;
	char * time = getCurrentTime();

	pthread_mutex_lock(&lockWrite);

	writeToFile(time, newMsg);
#if SRZ_LOG_TO_CONSOLE
	writeToConsole(time, newMsg);
#endif
	pthread_mutex_unlock(&lockWrite);

	delete [] indents;
	indents = NULL;

	delete [] newMsg;
	newMsg = NULL;

	delete [] time;
	time = NULL;
}

void Log::writeToConsole(char * time, const char * msg)
{
		printf("%s:%s\n", time, msg);	
}

void Log::writeToFile(char * time, const char * msg)
{
#if SRZ_LOG_USING_CCP
		unsigned int thing = outFile.tellp();
		float currentFileSize = (float)thing / 1024;
		if(currentFileSize >= SRZ_LOG_MAX_SIZE)					//If Log file size is greater than SRZ_LOG_MAX_SIZE
		{																						//close and reopen file to delete it and start over
			outFile.close();
			outFile.open(logPath);
		}
#endif

	//Check if Log Exists, if it doesn't then recreate it!
	std::ifstream dummy(logPath);
	if(!dummy)
	{
		outFile.close();
		outFile.open(logPath);
		outFile << time << ": " << "Log was re-created!" << std::endl;
	}
	else
	{
		dummy.close();
	}

  outFile << time << ": " << msg << std::endl;
}
