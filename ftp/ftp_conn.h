#ifndef _FTP_CONN_H_
#define _FTP_CONN_H_

#include "server/conn.h"
#include <string>
#include <queue>
#include <event2/event.h>
#include <event2/event_struct.h>
#include "network/socket.h"

class Ftp_conn : public Conn
{
public:
	static const size_t SEND_BUFFER_SIZE; 
	static const size_t RECV_BUFFER_SIZE;
	
	enum data_conn_type_t
	{
		CMD_LIST,
		CMD_RETR,
		CMD_STOR,
	};
	
	typedef struct data_conn_req
	{
		data_conn_req()
		{
			c = NULL;
			ev = NULL;
			buf = NULL;
			fd = 0;
			size = 0;
			offset = 0;
		}
		~data_conn_req()
		{
			if(ev != NULL)
			{
				event_del(ev);
				event_free(ev);
			}

			if(buf != NULL)
			{
				delete [] buf;
			}
		}
		Ftp_conn *c;
		data_conn_type_t for_which;
		std::string arg;
		int fd;
		struct event *ev;
		char *buf;
		size_t size;
		size_t offset;
	}data_conn_req_t;

	friend class Ftp_server;

	
	Ftp_conn();
	~Ftp_conn();
	int command_reply(std::string s);
	void set_user_name(std::string &s);
	const std::string &get_user_name();
	void add_data_conn_req(data_conn_req_t *req);
	std::string real_path(std::string &file_name);
	
private:
	std::string user_name_;
	std::string cur_work_dir_ = "/home/vanbreaker";
	std::string rename_path_;
	std::queue<data_conn_req_t *> req_queue_;
	Socket pasv_listener_;
	bool pasv_listening_ = false;
	struct event pasv_listen_event_;
	//Libevent_thread *pasv_listen_worker_;
};


#endif
