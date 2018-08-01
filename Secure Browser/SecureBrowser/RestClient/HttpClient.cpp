#include <vector>
#include <string>
#include <fstream>

#include "HttpClient.h"
#include "Base64.h" // for image decoding
#include "..\TraceUtility\LogInfo.h"
#include "..\TraceUtility\LogWarning.h"
#include "..\TraceUtility\LogError.h"


namespace ISXHttpClient 
{
	HttpClient::HttpClient() : token(L""), domen(L""), base_url(L"http://192.168.195.180"), client(base_url)
	{		
	}	

	HttpClient::HttpClient(HttpClient&& rhs) : base_url(rhs.base_url), client(rhs.client)
	{
		// set rhs in deleted state
	}

	HttpClient::~HttpClient()
	{
	}

	HttpClient& HttpClient::operator=(HttpClient&& rhs)
	{
		if (this != &rhs)
		{
			// free resources, nothing (no pointers)

			base_url = rhs.base_url;
			client = rhs.client;
		}

		// set rhs in deleted state, nothing (no pointers)

		return *this;
	}	

	pplx::task<void> HttpClient::request_files_upload(char* buffer, size_t lenght, utility::string_t filename)
	{	

		if (buffer == nullptr)
		{
			tlf_e << AT << "File or buffer is empty";
			return create_task([]
			{
				throw std::invalid_argument("Empty buffer");
			});
		}
		else
		{
			tlf_i << AT << "File is attached";
		}

		std::string encoded_str = base64_encode(reinterpret_cast<const unsigned char*>(buffer), lenght);
		std::wstring wstr_encoded(encoded_str.begin(), encoded_str.end());
				
		if (!domen.empty())
		{
			http_client new_client(domen);
			client = std::move(new_client);
		}

		std::wstring body = L"/webservice/rest/server.php?&wstoken=";
			
		body.append(token);		
		body.append(L"&wsfunction=core_files_upload&contextid=0&component=user&filearea=draft&itemid=0&filepath=/&filename=");
			
		body.append(filename);
		body.append(L"&filecontent=");
		
		body.append(wstr_encoded);
		body.append(L"&contextlevel=user&instanceid=7");				
		
		return client.request(methods::POST, body).then([](http_response response)
		{	
			int responsecode = response.status_code();
			std::string respS = "Server returned status code " + std::to_string(responsecode);
			const char *respCh = respS.c_str();
			tlf_i << AT << respCh;
		
			// retrieve a response
			size_t lenght = (size_t)response.headers().content_length();
			istream bodyStream = response.body();

			container_buffer<std::string> inStringBuffer;

			return bodyStream.read(inStringBuffer, lenght).then([inStringBuffer](size_t bytesRead)
			{
				std::string &text = inStringBuffer.collection();
				std::wstring wstr(text.begin(), text.end());

				std::size_t start_pos = text.find("<RESPONSE>");
				if (start_pos == std::string::npos)
					{
						start_pos = text.find("<MESSAGE>");
						if (start_pos != std::string::npos)
						{
							std::size_t found_pos = text.find("</MESSAGE>", start_pos + 9);
							const std::size_t length = found_pos - start_pos - 9;
							const std::string subexpr = text.substr(start_pos + 9, length) + ". File do not passed";
							const char *ch = subexpr.c_str();
							tlf_e << AT << ch;
						}
						else
						{
							tlf_e << AT << "Unknown bad response from server. File do not passed";
						}
					}
				else
				{
					tlf_i << AT << "File passed succesfully";
				}
				

			});
		});	
	}


	pplx::task<void> HttpClient::request_files_upload(utility::string_t filepath)
	{

		char* buffer = nullptr;

		//const char file_path[] = filepath;
		const char file_path[] = "Log//info_file.log";

		std::ifstream is(file_path);

		if (!is.is_open())
		{
			is.close();
			tlf_e << AT << "Can't open file Log file";
			return create_task([]
			{
				throw std::invalid_argument("Can't open Log file");
			});
		}

		long p = (long)is.tellg();
		is.seekg(0, std::ios::end);
		size_t size = (size_t)is.tellg();
		is.seekg(p);

		buffer = new char[size];

		is.read(buffer, size);
		is.close();
		

		std::string encoded_str = base64_encode(reinterpret_cast<const unsigned char*>(buffer), size);
		std::wstring wstr_encoded(encoded_str.begin(), encoded_str.end());

		if (!domen.empty())
		{
			http_client new_client(domen);
			client = std::move(new_client);
		}

		std::wstring body = L"/webservice/rest/server.php?&wstoken=";

		body.append(token);
		body.append(L"&wsfunction=core_files_upload&contextid=0&component=user&filearea=draft&itemid=0&filepath=/&filename=");

		body.append(L"info_file.log");
		body.append(L"&filecontent=");

		body.append(wstr_encoded);
		body.append(L"&contextlevel=user&instanceid=7");

		return client.request(methods::POST, body).then([](http_response response)
		{
			int responsecode = response.status_code();
			std::string respS = "Server returned status code " + std::to_string(responsecode);
			const char *respCh = respS.c_str();
			tlf_i << AT << respCh;

			// retrieve a response
			size_t lenght = (size_t)response.headers().content_length();
			istream bodyStream = response.body();

			container_buffer<std::string> inStringBuffer;

			return bodyStream.read(inStringBuffer, lenght).then([inStringBuffer](size_t bytesRead)
			{
				std::string &text = inStringBuffer.collection();
				std::wstring wstr(text.begin(), text.end());

				std::size_t start_pos = text.find("<RESPONSE>");
				if (start_pos == std::string::npos)
				{
					start_pos = text.find("<MESSAGE>");
					if (start_pos != std::string::npos)
					{
						std::size_t found_pos = text.find("</MESSAGE>", start_pos + 9);
						const std::size_t length = found_pos - start_pos - 9;
						const std::string subexpr = text.substr(start_pos + 9, length) + ". Log file do not passed";
						const char *ch = subexpr.c_str();
						tlf_e << AT << ch;
					}
					else
					{
						tlf_e << AT << "Unknown bad response from server. Log file do not passed";
					}
				}
				else
				{
					tlf_i << AT << "Log file passed succesfully";
				}


			});
		});
	}

	bool HttpClient::openConfigFile(std::string fileName)
	{
		std::ifstream in_file(fileName);

		if (!in_file.is_open())
		{
			std::wcout << "Config file is not opened" << std::endl;
			in_file.close();
			return false;
		}

		else
		{
			std::string line;

			while (std::getline(in_file, line))
			{
				if (line.find("#TOKEN") != std::string::npos)
				{
					line.erase(line.cbegin(), line.cbegin() + 7);
					std::wstring temp(line.cbegin(), line.cend());
					
					token.assign(temp);

					std::wcout << token << std::endl;
				}

				if (line.find("#DOMEN") != std::string::npos)
				{
					line.erase(line.cbegin(), line.cbegin() + 7);
					std::wstring temp(line.cbegin(), line.cend());

					domen.assign(temp);

					std::wcout << domen << std::endl;
				}
			}
		}

		return true;
	}
}