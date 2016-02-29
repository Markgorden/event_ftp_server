#include "ftp/ftp_conn.h"
#include "base/error.h"
#include "network/sock_opr.h"
#include <iostream>

const size_t Ftp_conn::SEND_BUFFER_SIZE = 4096;
const size_t Ftp_conn::RECV_BUFFER_SIZE = 4096;


Ftp_conn::Ftp_conn()
{

}


Ftp_conn::~Ftp_conn()
{
    if(pasv_listening_)
    {
        event_del(&pasv_listen_event_);
        socks::close(pasv_listener_.get_sockfd());
    }

    while(!req_queue_.empty())
    {
        Ftp_conn::data_conn_req_t *req = req_queue_.front();

        req_queue_.pop();
        delete req;
    }
}

int Ftp_conn::command_reply(std::string s)
{
    std::string reply = s + "\r\n";
    ssize_t n;
    
    std::cout << "reply: " << reply << std::endl;
    if((n = socks::write(this->get_socket_fd(), reply.c_str(), reply.length())) 
			!= reply.length())
    {
        std::cout << "write failed," << n << "," << reply.length() <<std::endl;
        return ERR_FS_WRITE;
    }
	
    return SUCCESS;
}


void Ftp_conn::set_user_name(std::string &s)
{
    user_name_ = s;
}



const std::string &Ftp_conn::get_user_name()
{
    return user_name_;
}




void Ftp_conn::add_data_conn_req(data_conn_req_t *req)
{
    req->c = this;
    req_queue_.push(req);
}



std::string Ftp_conn::real_path(std::string &file_name)
{
    if(file_name[0] == '/')
        return file_name;
	
    return cur_work_dir_ + '/' + file_name;
}
