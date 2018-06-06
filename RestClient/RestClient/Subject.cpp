#include "Subject.h"
#include "Observer.h"

	void Subject::attach(Observer *obs)
	{
		collection.push_back(obs);
	}

	void Subject::setBufferData(char* val)
	{
		buffer_data = std::make_unique<char*>(val);
		notify();
	}

	char* Subject::getBufferData()
	{
		if (buffer_data)
			return *buffer_data;
		else
			return nullptr;
	}

	void Subject::notify()
	{
		for (int i = 0; i < collection.size(); i++)
					collection[i]->request();
	}