#ifdef _WIN32
#include <io.h>
#else 
#include <unistd.h>
#endif
#include "transmission_server.h"
#include <algorithm>
#include <time.h>
#include <signal.h>


static bool brk_sig_received = false;
void sig_handler(int num)
{
	if (num == SIGBREAK) {
		brk_sig_received = true;
	}
}

int server_thread_count = 8;
std::mutex mtx;

transmission_server::transmission_server()
{
	signal(SIGBREAK, sig_handler);
	this->sock_obj = new xxsocket(AF_INET, SOCK_STREAM, 0);
}

static std::string get_short_name(const std::string& complete_filename)
{
	size_t pos = complete_filename.find_last_of("\\");

	if (pos == std::string::npos)
		pos = complete_filename.find_last_of("/");

	if (pos != std::string::npos) {
		try {
			return complete_filename.substr(pos + 1);
		}
		catch (...) {
		}
	}
	return complete_filename;
}
bool transmission_server::start( const char* ip, u_short port )
{
	if(this->sock_obj->bind(ip, port) != 0)
	{
		std::cerr << ERRS_SERVER_BIND_FAIELD << " port: " << port << "\n";
		return false;
	}

	if (this->sock_obj->listen() != 0)
	{
		std::cerr << ERRS_SERVER_LISTEN_FAILED << "\n";
		return false;
	};

	std::string option = "no";

	for (auto i = 0; i < server_thread_count; ++i) {
		threads.push_back(new std::thread([this]{
			this->run();
		}));
	}


	for (auto t : threads)
	{
		t->join();
	}

	for (auto t : threads)
		delete t;

	threads.clear();

	return true;
}

void transmission_server::run(void)
{
	char* buffer = new char[BUFFER_SIZE];

	auto tid = std::this_thread::get_id();
	std::cout << "Transmission Server thread id:" << tid << "\n";
	do {
		std::cout << "Transmission Server: waiting for transmission client...\n";
		mtx.lock();
		xxsocket csock = this->sock_obj->accept();
		mtx.unlock();
		std::cout << "Transmission Server: new client connection arrived, peer address:" << csock.local_endpoint().to_string() << "\n";

		sftp_header header;

		int count = csock.recv(buffer, header.size); // recv header
		if (count == -1) {
			std::cout << "Transmission Server: client is gone.\n";
			csock.release();
			continue;
		}
		// dump_hex_i("收到包头:", buffer, header.size);
		decode_sftp_header(buffer, header);

		char filenamebuf[256] = { 0 };
		std::cout << "Transmission Server: header received, size: " << header.size << "\n"
			<< "Transmission Server: namesize: " << header.namesize << " datasize: " << header.datasize << ", now receive name, please wait...\n";
		csock.recv(filenamebuf, header.namesize);

		std::cout << "Transmission Server: receive complete, detail: \n"
			"                     filename size --> " << header.namesize << "\n"
			"                     filename -------> " << filenamebuf << "\n"
			"                     filedata size --> " << header.datasize << "\n"
			"                     filedata      --> now saveing...\n";
		std::ofstream of;

		std::string name = get_short_name(filenamebuf);
		if (0 == access(name.c_str(), 0))
		{
			std::cerr << "Transmission Server: the file:" << name << " already exist in current directory.\n";
			std::cout << "Do you want overrite it?[yes/no]\n";
			char command[64] = { 0 };
			std::cin >> command;
			if (strcmp(command, "yes") != 0) {
				std::cout << "You select no, now close client.\n";
				csock.close();
				csock.release();
				return;
			}

			remove(name.c_str());
		}
		of.open(name.c_str(), std::ios_base::binary);
		if (!of.is_open())
		{
			std::cout << "Transmission Server: create file failed!\n";
			return;
		}

		long start = clock();

		// recv and write file
		std::streamsize bytes_left = header.datasize;
		while (bytes_left > 0) {
			auto byte_to_recv = (std::min)(bytes_left, (std::streamsize)sizeof(buffer));
			auto bytes_recv = csock.recv(buffer, byte_to_recv);
			if (recv > 0) {
				of.write(buffer, bytes_recv);
				bytes_left -= bytes_recv;
			}
			else {
				std::cout << "Transmission Server: connection missing!\n";
				break;
			}
		}

		of.close();

		auto time_elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
		auto speed = header.datasize / time_elapsed / SZ(1, M);
		std::cout << "Transmission Server: congratulations, save file complete.\n";
		std::cout << "Transmission Server: speed: " << speed << "MB/s.\n";
		this->sock_obj->send("ACK", sizeof("ACK"));

        csock.shutdown();

	} while (!brk_sig_received);


	delete[] buffer;
}

