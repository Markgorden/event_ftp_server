#include "ftp/ftp_server.h"
#include "ftp/ftp_conn.h"
#include "server/conn.h"
#include "server/libevent_thread.h"
#include "network/sock_opr.h"
#include "network/net_addr.h"
#include "base/error.h"
#include "base/file_opr.h"

#include <iostream>
#include <stdio.h>
#include <event2/bufferevent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#define USER_FILE "./users"



Ftp_server::Ftp_server(int port) : Event_server(port)
{
    get_user_list();
	
    handler_map["USER"] = Ftp_server::cmd_user_handler;
    handler_map["AUTH"] = Ftp_server::cmd_auth_handler;
    handler_map["PASS"] = Ftp_server::cmd_pass_handler;
    handler_map["SYST"] = Ftp_server::cmd_syst_handler;
    handler_map["TYPE"] = Ftp_server::cmd_type_handler;
    handler_map["PASV"] = Ftp_server::cmd_pasv_handler;
    handler_map["LIST"] = Ftp_server::cmd_list_handler;
    handler_map["CWD"]  = Ftp_server::cmd_cwd_handler;
    handler_map["PWD"]  = Ftp_server::cmd_pwd_handler;
    handler_map["CDUP"] = Ftp_server::cmd_cdup_handler;
    handler_map["MKD"] = Ftp_server::cmd_mkd_handler;
    handler_map["STOR"] = Ftp_server::cmd_stor_handler;
    handler_map["RETR"] = Ftp_server::cmd_retr_handler;
    handler_map["DELE"] = Ftp_server::cmd_dele_handler;
    handler_map["RMD"]  = Ftp_server::cmd_rmd_handler;
    handler_map["RNFR"] = Ftp_server::cmd_rnfr_handler;
    handler_map["RNTO"] = Ftp_server::cmd_rnto_handler;
}



Ftp_server::~Ftp_server()
{
	
}



//parse the received client cmd string
//input : cli_str-->received client string
//output: cmd-->ftp cmd       arg-->ftp cmd argument
static int parse_command(std::string cli_str, std::string &cmd, std::string &arg)
{
    std::size_t pos_space;
    std::size_t pos_end;

    std::cout << cli_str << std::endl;

    pos_space = cli_str.find(' ');
    if(pos_space == std::string::npos)
    {
        pos_end = cli_str.find("\r\n");
        if(pos_end == std::string::npos)
        {
            fprintf(stderr, "command string is invalid1");
            return -1;
        }
        
        cmd = cli_str.substr(0, pos_end);
        return 0;
    }

    pos_end = cli_str.find("\r\n");
    if(pos_end == std::string::npos)
    {
        fprintf(stderr, "command string is invalid2");
        return -1;
    }
    
    cmd = cli_str.substr(0, pos_space);
    arg = cli_str.substr(pos_space + 1, pos_end - pos_space - 1);

    return 0;
}



//read a line from file
int read_line(int fd, char *buf, int size)
{
    char c;
    int i=0;
    int ret;

next_line: 
    while((ret = read(fd, &c, 1)) > 0)
    {
        if(c != '\n')
            buf[i++] = c;
        else
            break;

        if(i>=size)
            return -1;
    }

    if(ret < 0)
    {
        return -1;
    }
    else if(ret == 0)
    {
        if(i <= 1)     //invalid end line... drop it
            return 0;
        
        buf[i] = 0;   //valid end line..
        return 1;
    }
    else
    {
        if(i < 3) //invalid line, skip it
        {
            i = 0;
            goto next_line;
        }
        buf[i] = 0;
        return 1;
    }
}




static void get_user_and_password(std::string s, std::string &user, 
                                            std::string &password)
{
    int pos = s.find(' ');

    user = s.substr(0,pos);
    password = s.substr(pos+1, s.size() - pos - 1);
}



void Ftp_server::get_user_list()
{
    int fd;
    char buf[128];
    int ret;
    
    fd = open(USER_FILE, O_RDONLY);
    assert(fd > 0);

    while((ret = read_line(fd, buf, sizeof(buf))) > 0)
    {
        std::string user_name,password;
        std::pair<std::string, std::string> user;
        get_user_and_password(buf, user_name, password);
        
        std::cout << "user:" << user_name<<"  password:" << password << std::endl;

        user = std::make_pair(user_name, password);
        user_list.push_back(user);

        if(ret == 0)
            break;
    }    

    if(ret < 0)
    {
        fprintf(stderr, "read user file error");
        exit(1);
    }
}




bool Ftp_server::is_user_valid(const std::string &user)
{
    for(auto it = user_list.cbegin(); it < user_list.cend(); it++)
    {
        if(user.compare(it->first) == 0) //match the user in user list
        {
            return true;
        }
    }

    return false; 
}


std::string Ftp_server::get_user_password(const std::string &user)
{
    for(auto it = user_list.cbegin(); it < user_list.cend(); it++)
    {
        if(user.compare(it->first) == 0) //match the user in user list
        {
            return it->second;
        }
    }

    return "";
}


 
int Ftp_server::cmd_user_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg)
{
    if(ser->is_user_valid(arg))
    {
        c->set_user_name(arg);
        return c->command_reply("331 Please specify the password");
    }

    return c->command_reply("530 Login incorrect");
}



int Ftp_server::cmd_auth_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg)
{
    return c->command_reply("530 Please login with USER and PASS.");
}



int Ftp_server::cmd_pass_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg)
{
    if(arg.compare(ser->get_user_password(c->get_user_name())) == 0)
    {
        return c->command_reply("230 Login successful");
    }

    return c->command_reply("530 Password error");
}



int Ftp_server::cmd_syst_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg)
{
    return c->command_reply("215 UNIX Type: L8");
}



int Ftp_server::cmd_type_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg)
{
    if(arg == "I")
    {
        return c->command_reply("200 Switching to Binary mode");
    }
    else if(arg == "A")
    {
        return c->command_reply("200 Switching to ASCII mode");
    }
    else
    {
        return c->command_reply("504 Mode not support");
    }
}



int Ftp_server::cmd_pasv_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg)
{
    unsigned short port;
    char str_port[16];
    std::string ip;
    char send_buf[128];

    if(ser->local_ip_ == "")
    {
        char buf[32];
        
        socks::get_ip_addr(c->get_socket_fd(), buf, sizeof(buf));
        ser->local_ip_.assign(buf);
        std::cout << "local_ip:" << ser->local_ip_ << std::endl;
    }

    ip = ser->local_ip_;

    if(!c->pasv_listening_)//if not listening, go to listen
    {
        Net_addr addr(0); 

        c->pasv_listener_.bind_addr(addr);
        c->pasv_listener_.listen();
        socks::set_nonblock(c->pasv_listener_.get_sockfd());

        //add listen socket to event monitor
        event_assign(&c->pasv_listen_event_, c->get_attached_worker()->get_event_base(),
                    c->pasv_listener_.get_sockfd(), EV_READ | EV_PERSIST,
                    Ftp_server::pasv_accept, static_cast<void *>(c));
        event_add(&c->pasv_listen_event_, NULL);
        c->pasv_listening_ = true;
    }

    //reply to client the ip:port we listen on
    c->pasv_listener_.get_socket_port(&port);

    std::cout<< "get ip:" << ip << std::endl;
    std::cout<< "get port:" << port << std::endl;

    for(size_t i=0; i<ip.size(); i++)
    {
        if(ip[i] == '.')
            ip[i] = ',';
    }

    snprintf(str_port, sizeof(str_port), "%u,%u", port>>8, port&0xFF);
    snprintf(send_buf, sizeof(send_buf), "227 Enter Passive mode (%s,%s)", ip.c_str(), str_port);

    return c->command_reply(send_buf);
}




struct list_info
{
    std::string file_list;
    std::string dir_list;
};



//get file permission information
static std::string get_permission_str(mode_t mode, bool *is_dir)
{
    std::string permission;

    if(S_IWUSR & mode) permission += 'w';
    if(S_IXUSR & mode) permission += 'x';   
    if(S_IRUSR & mode) permission += 'r';

    permission += '-';

    if(S_IWGRP & mode)  permission += 'w';
    if(S_IXGRP & mode)  permission += 'x';
    if(S_IRGRP & mode)  permission += 'r';

    permission += '-';

    if(S_IWOTH & mode)  permission += 'w';
    if(S_IXOTH & mode)  permission += 'x';
    if(S_IROTH & mode)  permission += 'r';

    if(S_ISDIR(mode)) 
    {
        *is_dir = true;
        return "dr" + permission;
    }
    else
    {
        *is_dir = false;
        return "-r" + permission;
    }
}



//get file date information
static std::string get_date_str(time_t crt_time)
{
    struct tm tm;
    char buf[64];
    std::string time_str;
    const char *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov", "Dec"};
    
    localtime_r(&crt_time, &tm);

    snprintf(buf, sizeof(buf), "%3.3s %2d %04d", month[tm.tm_mon], 
                tm.tm_mday, tm.tm_year + 1900);

    time_str.assign(buf);

    return time_str;
}



//get directory list information
static int get_dir_info(const char *name, struct stat *statbuf, void *arg)
{
    struct list_info *info = static_cast<struct list_info *>(arg);
    std::string date;
    std::string permission;
    bool is_dir;
    char line[256];

    permission = get_permission_str(statbuf->st_mode, &is_dir);
    date = get_date_str(statbuf->st_ctime);

    sprintf(line, "%s    1 %-10u %-10u %10lu %s %s\r\n",
            permission.c_str(), statbuf->st_uid, statbuf->st_gid, statbuf->st_size,
            date.c_str(), name);

    
   // std::cout << "get file info line:" << std::endl << line << std::endl;

    if(is_dir)
    {
        info->dir_list += line;
    }
    else
    {
        info->file_list += line;
    }
    
    return 0;
}




int Ftp_server::cmd_list_handler(Ftp_server *ser, Ftp_conn *c, std::string &path)
{
    Ftp_conn::data_conn_req_t *req = new Ftp_conn::data_conn_req_t;
    struct list_info info;
    int ret;
    size_t size;

    if(req == NULL)
    {
    	fprintf(stderr, "memory used out");
    	return ERR_NOMEM;
    }
    req->offset = 0;
    req->for_which = Ftp_conn::CMD_LIST;
    
    if(path == "")
    {
        req->arg = c->cur_work_dir_;
    }
    else
    {
        req->arg = path;
    }

    if(!file::is_dir_exist(req->arg.c_str()))
    {
        c->command_reply("550 Directory not found");
        return SUCCESS;
    }

    //traverse items under the directory and get information
    if((ret = file::for_each_in_dir(req->arg.c_str(), get_dir_info,
	   static_cast<void *>(&info))) != SUCCESS)
    {
    	c->command_reply("451 Local error in processing");
    	return ret;
    }


    //allocate buffer to story the list information
    size = info.dir_list.length() + info.file_list.length() + 1;
    req->buf = new char[size];
    if(req->buf == NULL)
    {
    	fprintf(stderr, "memory used out:cannot allocate %u bytes for list directory", size);
    	c->command_reply("451 Local error in processing");
    	return ERR_NOMEM;
    }
    req->size = size;

    sprintf(req->buf, "%s%s", info.dir_list.c_str(), info.file_list.c_str());
	
    c->add_data_conn_req(req);
	//printf("socket-->%d, c-->%p, queue-->%p\n",c->get_socket_fd(), c, &c->req_queue_);
	
    return SUCCESS;
}




int Ftp_server::cmd_stor_handler(Ftp_server *ser, Ftp_conn *c, std::string &path)
{
    int fd;
    Ftp_conn::data_conn_req_t *req = new Ftp_conn::data_conn_req_t;
    std::string fpath = c->real_path(path);

    if(req == NULL)
    {
    	fprintf(stderr, "memory used out");
    	return ERR_NOMEM;
    }
	
    if((fd = file::create_file(fpath.c_str())) <= 0)
    {	
    	perror("create file error:");
        c->command_reply("550 cannot create file");
        return SUCCESS;
    }
	
    req->for_which = Ftp_conn::CMD_STOR;
    req->fd = fd;
    req->size = req->offset = 0;
    req->buf = new char[Ftp_conn::RECV_BUFFER_SIZE];
    if(req->buf == NULL)
    {
    	c->command_reply("451 Inernal error");
    	return ERR_NOMEM;
    }

    c->add_data_conn_req(req);
	
    return SUCCESS;
}




int Ftp_server::cmd_retr_handler(Ftp_server * ser, Ftp_conn *c, std::string &path)
{
    int fd;
    Ftp_conn::data_conn_req_t *req = new Ftp_conn::data_conn_req_t;
    std::string fpath = c->real_path(path);

    if(req == NULL)
    {
        fprintf(stderr, "memory used out");
        return ERR_NOMEM;
    }

    if((fd = file::open(fpath.c_str(), file::RDONLY)) < 0)
    {
        c->command_reply("550 Cannot access file");
        return SUCCESS;
    }

    req->for_which = Ftp_conn::CMD_RETR;
    req->fd = fd;
    req->size = req->offset = 0;
    req->buf = new char[Ftp_conn::SEND_BUFFER_SIZE];
    if(req->buf == NULL)
    {
        c->command_reply("451 Inernal error");
        return ERR_NOMEM;
    }
	
    c->add_data_conn_req(req);
	
    return SUCCESS;
}



int Ftp_server::cmd_pwd_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg)
{
    return c->command_reply("257 " + c->cur_work_dir_);
}



int Ftp_server::cmd_cwd_handler(Ftp_server *ser, Ftp_conn *c, std::string &path)
{
    std::string dir_path;
    
    if(path[0] != '/')
    {
        dir_path = c->cur_work_dir_ + '/' + path;
    }
    else
    {
        dir_path = path;
    }

    if(!file::is_dir_exist(dir_path.c_str()))
    {
        return c->command_reply("550 Directory not found");
    }

    c->cur_work_dir_ = dir_path;
    std::cout << "change workdir to " << c ->cur_work_dir_ << std::endl;

    return c->command_reply("250 CWD command successful");
}



int Ftp_server::cmd_cdup_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg)
{
    std::string path;
    std::size_t pos;

    if(c->cur_work_dir_ != "/")//not root dir 
    {
        pos = c->cur_work_dir_.find_last_of('/');
        path = c->cur_work_dir_.substr(0, pos);
        c->cur_work_dir_ = path;
    }

    return c->command_reply("250 CDUP successful");
}




int Ftp_server::cmd_mkd_handler(Ftp_server *ser, Ftp_conn *c, std::string &dir_name)
{
    std::string path;

    if(dir_name[0] == '/')
    {
        path = dir_name;
    }
    else
    {
        path = c->cur_work_dir_ + '/' + dir_name;
    }

    if(file::is_dir_exist(path.c_str()))
    {
        return c->command_reply("550 Already existed");
    }

    if(file::make_dir(path.c_str()) != 0)
    {
        return c->command_reply("550 Create directory failed");
    }

    return c->command_reply("257 Create directory successful");
}





int Ftp_server::cmd_dele_handler(Ftp_server *ser, Ftp_conn *c, std::string &file_name)
{
    std::string fpath = c->real_path(file_name);
    int ret;
    if(!file::is_file_exist(fpath.c_str()))
    {
        return c->command_reply("550 File not exist");
    }

    if((ret = file::rm(fpath.c_str())) != SUCCESS)
    {
        c->command_reply("550 Delete file failed");
        return ret;
    }

    return c->command_reply("250 Delete file OK");
}




int Ftp_server::cmd_rmd_handler(Ftp_server *ser, Ftp_conn *c, std::string &file_name)
{
    std::string fpath = c->real_path(file_name);
    int ret;
    
    if(!file::is_dir_exist(fpath.c_str()))
    {
        return c->command_reply("550 Directory not exist");
    }
    
    if((ret = file::rm(fpath.c_str())) != SUCCESS)
    {
        c->command_reply("550 Delete directory failed");
        return ret;
    }
    
    return c->command_reply("250 Delete directory OK");
}



int Ftp_server::cmd_rnfr_handler(Ftp_server * ser, Ftp_conn *c, std::string &path)
{
    std::string fpath = c->real_path(path);
	
    if((!file::is_dir_exist(fpath.c_str())) && !file::is_file_exist(fpath.c_str()))
    {
        c->command_reply("550 File or directory not exist");
    }
    c->rename_path_ = fpath;
	
    return c->command_reply("350 Please specify destination name");
}



int Ftp_server::cmd_rnto_handler(Ftp_server *ser, Ftp_conn *c, std::string &path)
{
    std::string fpath = c->real_path(path);
    std::string cmd;
	
    if(file::is_dir_exist(fpath.c_str()) || file::is_file_exist(fpath.c_str()))
    {
        return c->command_reply("550 File or directory with the same name existed");
    }
    cmd = "mv " + c->rename_path_ + " " + fpath;
    system(cmd.c_str());
    
    return c->command_reply("250 Rename file successful");
}




static void data_list_cb(evutil_socket_t fd, short event, void *arg)
{
    Ftp_conn::data_conn_req_t *req = static_cast<Ftp_conn::data_conn_req_t *>(arg);
    Ftp_conn *c = req->c;

    c->command_reply("150 Here comes the directory list");

    while(1)
    {
        size_t bytes_to_send = req->size - req->offset;
        ssize_t n;
		
        if(bytes_to_send == 0)
        {	
             break;
        }

        n = file::writen(fd, req->buf + req->offset, bytes_to_send);
        if(n <= 0) //write error
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {//write buffer full, wait for the next writable time
                return;
            }
	    perror("write error:");
            break;
        }
        else if(n > 0)
        {
            file::writen(STDOUT_FILENO, req->buf + req->offset, bytes_to_send);
			req->offset += n;
        	
        }
    }

    c->command_reply("226 Directory send OK");
	
    delete req;
    socks::close(fd);
}



static int get_download_data_ready(Ftp_conn::data_conn_req_t *req)
{
    size_t n;
	
    if(req->size != 0 && req->offset < req->size)//data in buffer not handled
    {
        return 1;
    }

    req->size = 0;
    req->offset = 0;

    n = file::readn(req->fd, req->buf, Ftp_conn::SEND_BUFFER_SIZE);
    if(n < 0) //read error
    {
        return -1;
    }
    else  if(n == 0)//no more data
    {
        return 0;
    }
    else//read OK, 
    {
        req->size = n;
        return 1;
    }
}




static void download_file_cb(evutil_socket_t fd, short event, void *arg)
{
    Ftp_conn::data_conn_req_t *req = static_cast<Ftp_conn::data_conn_req_t *>(arg);
    Ftp_conn *c = req->c;
    int ret;

    c->command_reply("150 OK to send file data");        

    while(1)
    {
    	ret = get_download_data_ready(req);
    	if(ret == -1)//read local file error
        {
            std::cout << "get file data error" << std::endl;
            c->command_reply("426 Transfer aborted");
            goto STOP_TRANSFER;
        }
        else if(ret == 0)//transfer  all data OK
        {
            std::cout << "file download over" << std::endl;
            c->command_reply("226 Transfer complete");
            goto STOP_TRANSFER;
        }
        else
        {
            ssize_t n;
            size_t bytes_to_send = req->size - req->offset;
            if(bytes_to_send == 0)
            {
                req->size = 0;
                req->offset = 0;
                continue; // goto read next block of file
	}
	
	n = file::writen(fd, req->buf + req->offset, bytes_to_send);
	if(n <= 0) //write error
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {//write buffer full, wait for the next writable time
                return;
            }
	    c->command_reply("426 Transfer aborted");
            goto STOP_TRANSFER;
        }
        else if(n > 0)
        {
            file::writen(STDOUT_FILENO, req->buf + req->offset, bytes_to_send);
            req->offset += n;
	}
    }
	
STOP_TRANSFER:
    delete req;
    socks::close(fd);
}




static void upload_file_cb(evutil_socket_t fd, short event, void *arg)
{
    Ftp_conn::data_conn_req_t *req = static_cast<Ftp_conn::data_conn_req_t *>(arg);
    Ftp_conn *c = req->c;

	//c->command_reply("150 OK to receive file data");
	
    while(1)
    {
        ssize_t n;
        n = file::readn(fd, req->buf, Ftp_conn::RECV_BUFFER_SIZE);
        if(n < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return;
            }
            goto STOP_TRANSFER;
        }
        else if(n == 0)
        {
            std::cout << "peer closed data connection" << std::endl;
            c->command_reply("226 Transfer complete");
            goto STOP_TRANSFER;
        }
	else
        {
            ssize_t write_bytes;
            write_bytes = file::writen(req->fd, req->buf, n);
            if(write_bytes <= 0)
            {
                c->command_reply("426 Transfer aborted");
                goto STOP_TRANSFER;
            }
        }
    }
    
STOP_TRANSFER:
    delete req;
    socks::close(fd);
}




void Ftp_server::pasv_accept(evutil_socket_t listener, short event, void *arg)
{
	Ftp_conn *c = static_cast<Ftp_conn *>(arg);
	int conn_fd;
	Net_addr addr;
	Ftp_conn::data_conn_req_t *req; 

	//accept data connection
	conn_fd = c->pasv_listener_.accept(addr);	
	socks::set_nonblock(conn_fd);
	std::cout<< "data connection from" << addr.get_ip_port() << std::endl;

	printf("c-->%p, queue-->%p\n", c, &c->req_queue_);
	std::cout << "queue size: " << c->req_queue_.size() << std::endl;

	//get data request
	req = c->req_queue_.front();
	c->req_queue_.pop();

	if(req == NULL)
	{
		fprintf(stderr, "no valid request in queue");
		socks::close(conn_fd);
		return;
	}
	
	if(req->for_which == Ftp_conn::CMD_LIST)//for listing directory
	{
		req->ev = event_new(c->get_attached_worker()->get_event_base(), conn_fd,
							 EV_WRITE | EV_PERSIST,
							 data_list_cb, 
							 static_cast<void *>(req));
		if(req->ev == NULL)
		{
			fprintf(stderr, "event_new failed");
			return;
		}
		event_add(req->ev, NULL);
	}	
	else if(req->for_which == Ftp_conn::CMD_RETR)//for  downloading file
	{
		req->ev = event_new(c->get_attached_worker()->get_event_base(), conn_fd,
							 EV_WRITE | EV_PERSIST,
							 download_file_cb, 
							 static_cast<void *>(req));
		if(req->ev == NULL)
		{
			fprintf(stderr, "event_new failed");
			return;
		}
		event_add(req->ev, NULL);
	}
	else //for uploading file
	{
		req->ev = event_new(c->get_attached_worker()->get_event_base(), conn_fd,
					 EV_READ | EV_PERSIST,
					 upload_file_cb, 
					 static_cast<void *>(req));
		if(req->ev == NULL)
		{
			fprintf(stderr, "event_new failed");
			return;
		}
		event_add(req->ev, NULL);

		c->command_reply("150 OK to receive file data");
	}
}




int Ftp_server::cmd_unknown(Ftp_conn *c)
{
    return c->command_reply("500 Unknow command");
}




Conn *Ftp_server::handle_accept_event(int fd)
{
	Conn *c = new Ftp_conn;
	const char *str = "220 (welcome to vanbreaker's ftp)\r\n";

	std::cout << str << std::endl;

	if(file::writen(fd, str, strlen(str)) < 0)
	{
		fprintf(stderr, "write welcome message error");
	}

	return c;
}




void Ftp_server::handle_read_event(Conn *connection)
{
	char buf[64];
	std::string cmd,cmd_arg;
	size_t n;
	Event_server *event_server = connection->get_attached_worker()->get_event_server();
	Ftp_server *ser = static_cast<Ftp_server *>(event_server);
	Ftp_conn *c = static_cast<Ftp_conn *>(connection);
	
	n = bufferevent_read(c->get_bev(), buf, sizeof(buf));
	if(n <= 0)
	{
		fprintf(stderr, "error on bufferevent_read");
		return ;
	}
	buf[n] = 0;

	if(parse_command(buf, cmd, cmd_arg) != 0)
	{
		cmd_unknown(c);
		return;
	}

	auto t = ser->handler_map.find(cmd);
	if(t == ser->handler_map.end())
	{
		std::cout << "unrecognized command:" << cmd << std::endl;
		cmd_unknown(c);
	}
	else
	{
		cmd_handler handler = t->second;
		
		handler(ser, c, cmd_arg);
	}

}




void Ftp_server::handle_write_event(Conn *connection)
{
	
}




void Ftp_server::handle_error_event(Conn * connection)
{
	std::cout << "peer closed connection" << std::endl;
	Ftp_conn *c = static_cast<Ftp_conn *>(connection);

	delete c;
}

#if 1
int main()
{
	Ftp_server ftp_server(8888);

	ftp_server.run();

	return 0;
}
#endif
