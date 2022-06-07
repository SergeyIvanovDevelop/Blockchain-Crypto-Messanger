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

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;



// Функция вычисления размера файла
long int filesize( FILE *fp )
{
    long int save_pos, size_of_file;
    save_pos = ftell( fp );
    fseek( fp, 0L, SEEK_END );
    size_of_file = ftell( fp );
    fseek( fp, save_pos, SEEK_SET );
    return( size_of_file );
}



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

    void start() {
        socket_.async_handshake(boost::asio::ssl::stream_base::server,
                                boost::bind(&session::handle_handshake, this,
                                            boost::asio::placeholders::error));
    }

    void handle_handshake(const boost::system::error_code& error)
    {
        if (!error) {

            if (this->stop_send_flag != 2) {

            //    for (int i = 0; i < this->current_data_length; i++) {
            //        std::cout << "buf[" << i << "] = " << this->buf[i] << std::endl;
            //    }

                std::cout << "Отправка: ";
                std::cout << this->current_data_length;
                std::cout << " байт данных клиенту ..." << std::endl;

                boost::asio::async_write(socket_,
                                         boost::asio::buffer(this->buf, this->current_data_length), //this->current_data_length
                                         boost::bind(&session::handle_write, this,
                                                     boost::asio::placeholders::error));

                if (this->fsize - this->total == 4)
                    this->buf_size = 2;

            } else {

                    this->stop_send_flag = 0;

                    if (this->filename != "ZERO FILE") {
                        delete this->buf;
                        std::cout << "END of FILE" << std::endl;
                        std::cout << "File " << this->filename << " sent to client.\n" << std::endl;
                        delete this;
                        return;
                    }

                    this->total = 0;
                    info_about_file();

            }
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

        if (!error) {

            std::cout << "reply_bytes_transferred = " << bytes_transferred <<std::endl;
            std::cout << "Reply: ";
            // std::cout<<reply_<<std::endl;
            std::cout.write(reinterpret_cast<const char*>(data_), bytes_transferred);
            std::cout << "\n";

            // Дальнейшая отправка части файла
            if (this->stop_send_flag == 0) {
                //Считали из него "buf_size" (или менее) байт данных
                this->buf = new char[1024];
                int real_read_bytes = fread(this->buf, 1, this->buf_size, this->f);
                this->current_data_length = real_read_bytes;

                this->total = this->total + this->current_data_length;

                if (this->current_data_length == 0)
                {

                    // Закрытие файла
                    fclose(this->f);

                    std::cout << "END of FILE" << std::endl;

                    this->stop_send_flag = 0;

                    if (this->filename != "ZERO FILE")
                    {
                        delete this->buf;
                        std::cout << "File " << this->filename << " sent to client.\n" << std::endl;
                    }

                    this->filename = "ZERO FILE";
                    this->current_data_length = -1;
                    this->total = 0;

                    delete this;

                    return;
                }

                std::cout << "Real_read_bytes = " << this->current_data_length << std::endl;
                std::cout<< "Total = " << this->total << "/" << this->fsize << std::endl;

                //Проверка на окончание файла
                if ((real_read_bytes != this->buf_size) or (feof(this->f)))
                    if (feof(this->f)) {
                        std::cout << "END of FILE" << std::endl;

                        // Означает ПОСЛЕДНЮЮ отправку порции данных для ТЕКУЩЕГО файла
                        this->stop_send_flag = 1;

                        // Закрытие файла
                        fclose(this->f);
                    }

                // Создаем переменную ошибки для совместимости с handle_handshake
                const boost::system::error_code error_my;
                // Вот он повторный вызов-то !!!
                this->handle_handshake(error_my);

            }
            else
            {
                this->stop_send_flag = 2;

                // Создаем переменную ошибки для совместимости с handle_handshake
                const boost::system::error_code error_my;
                // Вот он повторный вызов-то !!!
                // Но тут "this->buf, real_read_bytes" ужны лишь для совместимости с "handle_handshake"
                this->handle_handshake(error_my);
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



    void filename2(const boost::system::error_code& error, size_t bytes_transferred)
    {
        const char* filename_get;
        filename_get = const_cast<const char*>(data_);
        //std::cout << "file_name = " << filename_get << "__";
        std::cout << "bytes_transferred = " << bytes_transferred <<std::endl;

        int size_fn = strlen(filename_get);

        //std::cout << "size_fn = " << size_fn << std::endl;
        char* fn = new char[size_fn-3];

        for (int i =0; i < size_fn-4; i ++)
            fn[i] = '\0';


        for (int i=0; i < size_fn - 3; i++)
        {
            fn[i] = filename_get[i+4];
        }

        const char* FileName = fn;
        std::string filename__without_get (FileName);
        delete fn;
        this->filename = "Store/" + filename__without_get;
        //std::cout << "this->filename = " << this->filename <<"___";

        //Открыли файл (с проверкой)

        if ((this->f=fopen(this->filename.c_str(), "rb"))==NULL)
        {
            std::cout << "Невозможно открыть файл: '" << this->filename << "' !" << std::endl;
            //exit(1);
            return;
        }

        //Узнали его размер
        this->fsize = filesize(this->f);
        std::cout << "File_size = " << this->fsize << std::endl;

        this->buf = reinterpret_cast<char*>(&this->fsize);

        this->current_data_length = sizeof(fsize);

        // Создаем переменную ошибки для совместимости с handle_handshake
        const boost::system::error_code error_my;
        // Вот он повторный вызов-то !!!
        this->handle_handshake(error_my);

    }

    void info_about_file (void)

    {

        boost::asio::async_read(socket_,
                                 boost::asio::buffer(data_, max_length),
                                 boost::bind(&session::filename2, this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));

    }


        // public variabels

    //-----------------------------

    // Указатель на файл
    FILE* f;

    //Размер файла
    int fsize = 0;

    //Определили счетчик считанных байт данных из файла
    int total = 0;

    // Определили, сколько за раз будет считываться даных из файла
    int buf_size = 512;

    // Этот флаг будет срабатывать когда будет достигнут конец файла
    int stop_send_flag = 2;

    //Буфер памяти под считываемые данные из файла
    char* buf;

    // Имя обрабатываемого в данный момент файла
    std::string filename = "ZERO FILE";

    // Текущее значения данных, которое было считано
    int current_data_length = -1;


    //------------------------------



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
