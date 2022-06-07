//
// client.cpp
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

enum { max_length = 1024 };

const char* ARGV_FN;
int gen_return = -1;
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


class client
{
public:
    client(boost::asio::io_context& io_context,
           boost::asio::ssl::context& context,
           boost::asio::ip::tcp::resolver::results_type endpoints)
            : socket_(io_context, context)
    {
        socket_.set_verify_mode(boost::asio::ssl::verify_peer);
        socket_.set_verify_callback(
                boost::bind(&client::verify_certificate, this, _1, _2));

        boost::asio::async_connect(socket_.lowest_layer(), endpoints,
                                   boost::bind(&client::handle_connect, this,
                                               boost::asio::placeholders::error));
    }

    bool verify_certificate(bool preverified,
                            boost::asio::ssl::verify_context& ctx)
    {
        // The verify callback can be used to check whether the certificate that is
        // being presented is valid for the peer. For example, RFC 2818 describes
        // the steps involved in doing this for HTTPS. Consult the OpenSSL
        // documentation for more details. Note that the callback is called once
        // for each certificate in the certificate chain, starting from the root
        // certificate authority.

        // In this example we will simply print the certificate's subject name.
        char subject_name[256];
        X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
        std::cout << "Verifying " << subject_name << "\n";

        return preverified;
    }

    void handle_connect(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_handshake(boost::asio::ssl::stream_base::client,
                                    boost::bind(&client::handle_handshake, this,
                                                boost::asio::placeholders::error));
        }
        else
        {
            std::cout << "Connect failed: " << error.message() << "\n";
        }
    }

    void handle_handshake(const boost::system::error_code& error)
    {

        if (this->stop_send_flag !=2)
        {
            if (!error) {

                 // Вывод передающихся данных
                 //   for (int i = 0; i < this->current_data_length; i++) {
                 //       std::cout << "buf[" << i << "] = " << this->buf[i] << std::endl;
                 //   }

                    std::cout << "Отправка: ";
                    std::cout << this->current_data_length;
                    std::cout << " байт данных на сервер ..." << std::endl;


                    boost::asio::async_write(socket_,
                                             boost::asio::buffer(this->buf, this->current_data_length),
                                             boost::bind(&client::handle_write, this,
                                                         boost::asio::placeholders::error,
                                                         boost::asio::placeholders::bytes_transferred));

                    if (this->fsize - this->total == 4)
                        this->buf_size=2;

            } else {
                std::cout << "Handshake failed: " << error.message() << "\n";
            }
        }
        else
        {
            if (!error) {

                this->stop_send_flag = 0;

                if (this->filename != "ZERO FILE")
                {
                    delete this->buf;
                    std::cout << "END of FILE" << std::endl;
                    std::cout << "File " << this->filename << " sent to server.\n" << std::endl;

                    this->stop_flag = 1;

                }

                this->total = 0;

                // Обновляем информацию о файле
                info_about_file();

            }
            else {
                std::cout << "Handshake failed: " << error.message() << "\n";
            }
        }


    }

    void handle_write(const boost::system::error_code& error,
                      size_t bytes_transferred)
    {
        if (!error)
        {
            boost::asio::async_read(socket_,
                                    boost::asio::buffer(reply_, max_length),
                                    boost::bind(&client::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            std::cout << "Write failed: " << error.message() << "\n";
        }
    }

    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred)
    {


            if (!error) {

                std::cout << "reply_bytes_transferred = " << bytes_transferred <<std::endl;
                std::cout << "Reply: ";
               // std::cout<<reply_<<std::endl;
                std::cout.write(reinterpret_cast<const char*>(reply_), bytes_transferred);
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
                            std::cout << "File " << this->filename << " sent to server.\n" << std::endl;
                            //return;
                            this->stop_flag = 1;
                        }

                        this->filename = "ZERO FILE";
                        this->current_data_length = -1;
                        this->info_about_file();
                        this->total = 0;

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
                std::cout << "Read failed: " << error.message() << "\n";
            }

    }


    void info_about_file (void)

    {

        if (this->stop_flag == 1)
        {

            std::string gen_return_local(reply_);

            gen_return = std::atoi(gen_return_local.c_str());

            return ;
        }

        // Создаем массив char для хранения имени файла
        char filename_char[150];
        for (int i = 0; i < 150; i++)
            filename_char[i] = '\0';

        // Просим пользователя ввести имя файла для отправки
      //  std::cout << "Please, enter filename for sending: ";
      //  std::cin >> filename_char;

        // Преобразуем массив char в string
        std::string filename1(ARGV_FN);

        this->filename = "Files_for_sending/" + filename1;

        //Пока что сделаем так (чтобы не вводить длинное название много раз)
        //filename = "Files_for_sending/from_client_to_server.txt";

        //Открыли файл (с проверкой)
        if ((this->f=fopen(this->filename.c_str(), "rb"))==NULL)
        {
            std::cout << "Невозможно открыть файл !" << std::endl;
            exit(1);
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

    // Указатель на файл
    FILE* f;

    int stop_flag = 0;

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


private:
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
    //char request_[max_length];
    char reply_[max_length];
};


int main(int argc, char* argv[])
{
    try
    {

        if (argc != 4)
        {
            std::cerr << "Usage: client <host> <port> <filename_for_sending_to_server>\n";
            return 1;
        }


        boost::asio::io_context io_context;


       // const char* a1 = "localhost";
       // const char* a2 = "2041";


        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::ip::tcp::resolver::results_type endpoints =
               // resolver.resolve(a1, a2);
                resolver.resolve(argv[1], argv[2]);

        boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
        ctx.load_verify_file("rootca.crt");

        client c(io_context, ctx, endpoints);


        ARGV_FN = argv[3];

        io_context.run();




        //Закрыли файл
        fclose(c.f);



        std::cout << "RETURNING = " << gen_return <<std::endl;


    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }


    //  Возвращаем номер файла на сервере (то как он будет называться: "Store/<return>.dat")
    return gen_return;
}
