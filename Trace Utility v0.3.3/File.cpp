#include "File.h"

void File::OpenFile()
{
	ofile.open("info_file.txt", std::ios_base::app);
}

void File::CloseFile()
{
	if (ofile.is_open())
		ofile.close();
}