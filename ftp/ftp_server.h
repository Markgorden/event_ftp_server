#ifndef _FTP_SERVER_H_
#define _FTP_SERVER_H_

#include "server/event_server.h"
#include "network/socket.h"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <event2/event_struct.h>


class Ftp_conn;
class Libevent_thread;


class Ftp_server : public Event_server
{
    typedef int (*cmd_handler)(Ftp_server *ser, Ftp_conn *conn, std::string &cmd_arg);

/* each following function is responsed for an according FTP command */
    //cmd "AUTH"
    static int cmd_auth_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "USER", specify the username when logining on
    static int cmd_user_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "PASS" , specify the password when logining on
    static int cmd_pass_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "SYST" , tell the system type
    static int cmd_syst_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "TYPE", specify the data type
    static int cmd_type_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "PASV", to listen for data connection
    static int cmd_pasv_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "LIST", list directory 
    static int cmd_list_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "PWD", show the current work directory
    static int cmd_pwd_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "CWD", change the current work directory
    static int cmd_cwd_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "CDUP", change to the upper directory
    static int cmd_cdup_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "MKD", create a new directory
    static int cmd_mkd_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "STOR", get file from client 
    static int cmd_stor_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "RETR", send file to client
    static int cmd_retr_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "DELE", delete file
    static int cmd_dele_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "RMD", remove a directory
    static int cmd_rmd_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "RNFR"  specify the file for rename
    static int cmd_rnfr_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //cmd "RNTO" specify the name rename to
    static int cmd_rnto_handler(Ftp_server *ser, Ftp_conn *c, std::string &arg);

    //for unknown cmd
    static int cmd_unknown(Ftp_conn *c);

    static void pasv_accept(evutil_socket_t listener, short event, void *arg);
	
public:
    Ftp_server(int port);
    virtual ~Ftp_server();
    virtual Conn *handle_accept_event(int fd);
    virtual void handle_read_event(Conn *connection);
    virtual void handle_write_event(Conn *connection);
    virtual void handle_error_event(Conn *connection);

    //judge whether the given user is allowed to access
    bool is_user_valid(const std::string &user);

    //search the password of specified user
    std::string get_user_password(const std::string &user);
    
private:
    std::map<std::string, cmd_handler> handler_map; // a cmd-->handler map
    std::vector<std::pair<std::string, std::string>> user_list;//(username, password) vector
    std::string local_ip_;//local ip client connected to
    void get_user_list();//get all valid user
};



#endif
