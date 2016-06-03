#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "transmission_client.h"

template<typename _Elem> inline
void strtolower(_Elem* source)
{
	while (*source != '\0')
	{
		*source = tolower(*source);
		++source;
	}
}

transmission_client::transmission_client()
{
}

bool transmission_client::start( const char* ip, u_short port)
{
	static char buffer[BUFFER_SIZE];
	static char filenamebuf[256];
	sftp_header header;

	std::string option = "no";

	do {
		std::cout << "Transmission Client: connecting to server: " << ip << ":" << port << "...\n";
		this->sock_obj = new xxsocket(AF_INET, SOCK_STREAM, 0);
		if (this->sock_obj->connect(ip, port) != 0)
		{
			std::cerr << "Transmission Client: connect failed!\n";
			return false;
		}
		std::cout << "Transmission Client: connect successfully. please input filename(full path) to transmitt:\n";
		//std::cin >> filenamebuf;
		char* nptr = filenamebuf;
		// std::cin.getline(filenamebuf, '\n');
		int ch = 0;
		do {
			ch = std::cin.get();
			if (ch != '\n' && ch != '\"')
				*(nptr++) = ch;
		} while (ch != '\n');

		header.namesize = strlen(filenamebuf); // filename size initialized
		std::cout << "Transmission Client: open the file: " << filenamebuf << "\n";
		std::ifstream fin;
		fin.open(filenamebuf, std::ios_base::binary);
		if (!fin.is_open())
		{
			std::cout << "Transmission Client: open failed!\n";
			return false;
		}
		fin.seekg(0, std::ios_base::end);
		header.datasize = fin.tellg(); // filesize initialized
		fin.seekg(0, std::ios_base::beg);

		std::cout << "Transmission Client: open file successfully, now reading...\n";

		// bodysize = filenamesize + filesize + sizeof(filenamesize) + sizeof(filesize);

		/*char* ptr = buffer;
		set_dval_step(ptr, bodysize);
		set_dval_step(ptr, filenamesize);
		set_stream_step(ptr, filenamebuf, filenamesize);
		set_qval_step(ptr, filesize);
		auto header_size = ptr - buffer;*/


		std::cout << "Transmission Server: read file complete, detail: \n"
			"           filename size --> " << header.namesize << "\n"
			"           filename -------> " << filenamebuf << "\n"
			"           filedata size --> " << header.datasize << "\n"
			"           total size -----> " << header.get_total_size() << "\n";
		"           filedata      --> now sending...\n";

		char* ptr = buffer;
		encode_sftp_header(header, ptr);
		/*this->sock_obj->send(buffer, sftp_header::size);
		dump_hex_i("·¢ËÍ°üÍ·:", buffer, sftp_header::size);*/
		ptr += sftp_header::size;

		memcpy(ptr, filenamebuf, header.namesize);
		ptr += header.namesize;

		auto header_with_name_size = header.size + header.namesize;

		auto bytes_to_read = (std::min)((long long)sizeof(buffer) - header_with_name_size, header.datasize);

		fin.read(ptr, bytes_to_read);
		int bytes_transferred = this->sock_obj->send(buffer, header_with_name_size + bytes_to_read);

		if (!fin.eof()) {
			auto bytes_left = header.get_total_size() - bytes_transferred;
			while (bytes_left > 0) {
				ptr = buffer;

				bytes_to_read = (std::min)(bytes_left, (long long)sizeof(buffer));
				fin.read(ptr, bytes_to_read);
				bytes_transferred = this->sock_obj->send(buffer, bytes_to_read);
				bytes_left -= bytes_transferred;
				if (fin.eof())
					break;
			}
		}

		fin.close();

		std::cout << "Transmission Server: send complete. waiting receive complete ACK...\n";
		this->sock_obj->recv(filenamebuf, sizeof("ACK"));
		std::cout << "received ACK, congratulations, file transmit successfully.\n";

		std::cout << "continue?[yes/no]";
		std::cin >> option;

		this->sock_obj->release();

		if (!option.empty())
			strtolower(&option.front());
		
	} while (option == "yes" || option == "y");

	return true;
}

