//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

// Для подсчета файлов в директории
#include <dirent.h>

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

class session
{
public:
    session(boost::asio::io_context& io_context,
            boost::asio::ssl::context& context)
            : socket_(io_context, context)
    {
    }

    ssl_socket::lowest_layer_type& socket()
    {
        return socket_.lowest_layer();
    }

    int count_of_file (void)
    {
        int file_count = 0;
        int dir_count = 0;
        int total = 0;
        DIR * dirp;
        struct dirent * entry;
        dirp = opendir("Store/");
        while ((entry = readdir(dirp)) != NULL) {
            total++;
            if (entry->d_type == DT_REG) {
                file_count++;
            }
        }
        dir_count = total - file_count;
        closedir(dirp);
        return file_count;
    }


    void start() {

        std::cout << "session_new" << std::endl;
        this->current_file_name = count_of_file();
        socket_.async_handshake(boost::asio::ssl::stream_base::server,
                                boost::bind(&session::handle_handshake, this,
                                            boost::asio::placeholders::error));
    }

    void handle_handshake(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                    boost::bind(&session::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));


            std::string sClientIp = this->socket().remote_endpoint().address().to_string();
            unsigned short uiClientPort = this->socket().remote_endpoint().port();

            std::cout << "Клиент подключился по IP = " << sClientIp << " и порту = " << uiClientPort << "." << std::endl;


        }
        else
        {
            std::cout<<"Error in handle_handshake"<<std::endl;
            delete this;
        }
    }

    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred)
    {

        int flag = 0;
        if (!error) {

            std::cout << "bytes_transferred = " << bytes_transferred << std::endl;

            // Если принят размер файла
            if ((bytes_transferred == 4) and (this->size_flag == 0)) {
                std::string filename = "Store/" + std::to_string(this->current_file_name) + ".dat";
                if ((this->f = fopen(filename.c_str(), "wb")) == NULL) {
                    std::cout << "Невозможно создать/открыть файл !" << std::endl;
                    exit(1);
                }

                this->filename = filename;
                this->filesize = *(reinterpret_cast<int *>(&data_));

                std::cout << "Размер нового принимаемого файла равен = " << this->filesize << std::endl;

                for (int i = 0; i < 1024; i++)
                    this->responde[i] = '\0';

                this->responde[0] = 'I';
                this->responde[1] = ' ';
                this->responde[2] = 'R';
                this->responde[3] = 'F';
                this->responde[4] = 'S';


                boost::asio::async_write(socket_,
                                         boost::asio::buffer(this->responde, max_length),
                                         boost::bind(&session::handle_write, this,
                                                     boost::asio::placeholders::error));

                this->size_flag = 1;
                return;
            }


            if (this->size_flag == 1) {

                // Увеличиваем счетчик на количество принятых байт
                this->total = this->total + bytes_transferred;

                std::cout<< "Total = "<<this->total<<"/"<<this->filesize<<std::endl;

                // Записывае принятые байты в файл
                int real_write_bytes = fwrite(data_, 1, bytes_transferred, this->f);
                if (real_write_bytes != bytes_transferred) {
                    std::cout << "ERROR = real_write_bytes != bytes_transferred " << std::endl;
                    exit(1);
                }



                // Файл принят полностью
                if (this->total == this->filesize) {
                    fclose(this->f);
                    this->filesize = 0;
                    this->total = 0;
                    this->current_file_name = this->current_file_name + 1;

                    std::cout << "Файл " << this->filename << " принят полностью" << std::endl;

                    for (int i = 0; i < 1024; i++)
                        this->responde[i] = '\0';

                    std::string count_files = std::to_string(this->current_file_name -1);

                    const char* count_files_const = count_files.c_str();


                    for (int i=0; i < strlen(count_files_const); i++)
                    {
                        this->responde[i] = count_files_const[i];
                    }

                    this->stop_flag = 1;

                    boost::asio::async_write(socket_,
                                             boost::asio::buffer(this->responde, max_length),
                                             boost::bind(&session::handle_write, this,
                                                         boost::asio::placeholders::error));

                   flag = 1;
                   this->size_flag=0;

                }


            // Просмотр принимаемых данных
            //std::cout << "Recieved: ";

            //for (int i = 0; i < bytes_transferred; i++) {
            //   std::cout << data_[i];
            //}

            if (flag == 0) {
                std::cout << "\n" << std::endl;


                for (int i = 0; i < 1024; i++)
                    this->responde[i] = '\0';

                this->responde[0] = 'O';
                this->responde[1] = 'K';

                boost::asio::async_write(socket_,
                                         boost::asio::buffer(this->responde, max_length),
                                         boost::bind(&session::handle_write, this,
                                                     boost::asio::placeholders::error));
                //sleep(1);
		        //delete this;


            }
        }
            } else {
                std::cout<<"Error in handle_read"<<std::endl;
                delete this;
            }

    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {


            if (this->stop_flag == 1)
            {
                return;
            }

            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                    boost::bind(&session::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            std::cout<<"Error in handle_write"<<std::endl;
            delete this;
        }
    }



    // Publiv variabels

    // -------------------------------------

    int stop_flag = 0;
    int filesize;
    int total = 0;
    FILE* f;
    int size_flag = 0;
    int current_file_name = 0;
    std::string filename = "NO FILE";
    char responde[1024] = {'n','o',' ','r','e','s','p','o','n','d'};

    // -------------------------------------

private:
    ssl_socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_context& io_context, unsigned short port)
            : io_context_(io_context),
              acceptor_(io_context,
                        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), port)),
              context_(boost::asio::ssl::context::sslv23)
    {
        context_.set_options(
                boost::asio::ssl::context::default_workarounds
                | boost::asio::ssl::context::no_sslv2
                | boost::asio::ssl::context::single_dh_use);
        //context_.set_password_callback(boost::bind(&server::get_password, this));
        context_.use_certificate_chain_file("user.crt");
        context_.use_private_key_file("user.key", boost::asio::ssl::context::pem);
        context_.use_tmp_dh_file("dh2048.pem");

        start_accept();
    }

    std::string get_password() const
    {
        return "tset";
    }

    void start_accept()
    {

        std::cout << "start_accept" << std::endl;
        session* new_session = new session(io_context_, context_);
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&server::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }

    void handle_accept(session* new_session,
                       const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
        }
        else
        {
            delete new_session;
        }

        start_accept();
    }

private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ssl::context context_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        using namespace std; // For atoi.
        server s(io_context, atoi(argv[1]));

        io_context.run();


    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
