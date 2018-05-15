#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <ctime>

enum Severity { Unspecified, Trace, Debug, Info, Warning, Error, Fatal };

static int i = 0;

class File
{
public:
	void OpenFile()
	{
		ofile.open("info_file.log", std::ios_base::app);
	}
	void CloseFile()
	{
		if (ofile.is_open())
			ofile.close();
	}
	template <typename T>
	void WriteToFile(const T& msg, Severity severity_level)
	{
		i++;

		if (!(i % 2))
		{
			ofile.write(msg, sizeof(msg));
			ofile.write("\n\n", sizeof("\n\n"));
		}
		else
		{
			if (severity_level == Unspecified)
				ofile.write("[Unspecified]\t", sizeof("[Unspecified]\t"));
			else if (severity_level == Trace)
				ofile.write("[Trace]\t", sizeof("[Trace]\t"));
			else if (severity_level == Debug)
				ofile.write("[Debug]\t", sizeof("[Debug]\t"));
			else if (severity_level == Info)
				ofile.write("[Info]\t\t", sizeof("[Info]\t\t"));
			else if (severity_level == Warning)
				ofile.write("[Warning]\t", sizeof("[Warning]\t"));
			else if (severity_level == Error)
				ofile.write("[Error]\t", sizeof("[Error]\t"));
			else if (severity_level == Fatal)
				ofile.write("[Fatal]\t", sizeof("[Fatal]\t"));

			auto current_time = GetCurrentTime();
			ofile.write(current_time, 24); // 25-th is a newline character
			ofile.write("\t", sizeof("\t"));

			ofile.write(msg, sizeof(msg));
			ofile.write("\t", sizeof("\t"));
		}

	}
private:
	std::ofstream ofile;

	bool FileExists(const std::string& name)
	{
		struct stat buffer;
		return (stat(name.c_str(), &buffer) == 0);
	}
	auto GetCurrentTime()
	{
		std::time_t result = std::time(nullptr);
		return std::asctime(std::localtime(&result));
	}
};

class Log
{
public:
	Log()
	{
		severity_level = Unspecified;
	}
	template <typename T>
	auto &operator << (const T& msg)
	{
		file.OpenFile();
		file.WriteToFile(msg, severity_level);
		file.CloseFile();
		return *this;
	}
protected:
	File file;
	Severity severity_level;
}tlf;

class LogTrace : public Log
{
public:
	LogTrace()
	{
		severity_level = Trace;
	}
}tlf_t;

class LogDebug : public Log
{
public:
	LogDebug()
	{
		severity_level = Debug;
	}
}tlf_d;

class LogInfo : public Log
{
public:
	LogInfo()
	{
		severity_level = Info;
	}
}tlf_i;

class LogWarning : public Log
{
public:
	LogWarning()
	{
		severity_level = Warning;
	}
}tlf_w;

class LogError : public Log
{
public:
	LogError()
	{
		severity_level = Error;
	}
}tlf_e;

class LogFatal : public Log
{
public:
	LogFatal()
	{
		severity_level = Fatal;
	}
}tlf_f;