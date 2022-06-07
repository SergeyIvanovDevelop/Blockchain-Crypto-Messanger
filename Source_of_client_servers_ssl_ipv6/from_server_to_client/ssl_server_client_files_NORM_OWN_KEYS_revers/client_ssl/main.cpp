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

const char* ARV_FN;

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
                                    boost::bind(&client::info_about_file, this,
                                                boost::asio::placeholders::error));
        }
        else
        {
            std::cout << "Connect failed: " << error.message() << "\n";
        }
    }

    void handle_handshake(const boost::system::error_code& error)
    {

            if (!error) {

                std::string request = "GET_" + this->filename;

                    std::cout << "Отправка запроса: ";
                    std::cout << request;
                    std::cout << " на сервер ..." << std::endl;

                   // std::cout << "request.length() = " << request.length() << std::endl;

                    int length_name = request.length();

                    boost::asio::async_write(socket_,
                                             boost::asio::buffer(request.c_str(), max_length),
                                             boost::bind(&client::handle_write, this,
                                                         boost::asio::placeholders::error,
                                                         boost::asio::placeholders::bytes_transferred));

            } else {
                std::cout << "Handshake failed: " << error.message() << "\n";
            }

    }

    void handle_write(const boost::system::error_code& error,
                      size_t bytes_transferred)
    {
        if (!error)
        {



            if (this->flag_end == 1)
            {return;}

            socket_.async_read_some(boost::asio::buffer(reply_, max_length),
                                    boost::bind(&client::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));


            // Он handle не возвращает, пока не придет именно то количество байт, которое указано в "length"

            /*
            boost::asio::async_read(socket_,
                                    boost::asio::buffer(reply_, length), // max_length
                                    boost::bind(&client::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
*/
            //std::cout << "Hi_0" <<std::endl;

        }
        else
        {
            std::cout << "Write failed: " << error.message() << "\n";
        }
    }

    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred)
    {

       // std::cout << "Hi_1" <<std::endl;

        int flag = 0;

            if (!error) {

                std::cout << "reply_bytes_transferred = " << bytes_transferred <<std::endl;

                // Обработка длины полученных данных
                // Если принят размер файла
                if ((bytes_transferred == 4) and (this->size_flag == 0)) {
                    std::string filename = "Receive_Tmp/Recieved_" + this->filename;
                    if ((this->f = fopen(filename.c_str(), "wb")) == NULL) {
                        std::cout << "Невозможно создать/открыть файл !" << std::endl;
                        exit(1);
                    }


                    //  ....
                    this->filesize = *(reinterpret_cast<int *>(reply_));

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
                                             boost::bind(&client::handle_write, this,
                                                         boost::asio::placeholders::error,
                                                         boost::asio::placeholders::bytes_transferred));

                    this->size_flag = 1;
                    return;

                }




                if (this->size_flag == 1) {

                    // Увеличиваем счетчик на количество принятых байт
                    this->total = this->total + bytes_transferred;

                    std::cout<< "Total = "<<this->total<<"/"<<this->filesize<<std::endl;

                    // Записывае принятые байты в файл
                    int real_write_bytes = fwrite(reply_, 1, bytes_transferred, this->f);
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


                        this->responde[0] = 'I';
                        this->responde[1] = ' ';
                        this->responde[2] = 'F';
                        this->responde[3] = 'U';
                        this->responde[4] = 'L';
                        this->responde[5] = 'L';
                        this->responde[6] = 'R';
                        this->responde[7] = 'F';

                        this->flag_size = 0;

                        delete(this->reply_);


                        this->flag_end = 1;

                        boost::asio::async_write(socket_,
                                                 boost::asio::buffer(this->responde, max_length),
                                                 boost::bind(&client::handle_write, this,
                                                             boost::asio::placeholders::error,
                                                             boost::asio::placeholders::bytes_transferred));

                        //   return;

                        flag = 1;
                        this->size_flag = 0;


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
                                                 boost::bind(&client::handle_write, this,
                                                             boost::asio::placeholders::error,
                                                             boost::asio::placeholders::bytes_transferred));

                    }
                }


            } else {
                std::cout << "Read failed: " << error.message() << "\n";
            }

    }


    void info_about_file (const boost::system::error_code& error)

    {

        this->reply_ = new char[max_length];
        // Создаем массив char для хранения имени файла
        char filename_char[150];
        for (int i = 0; i < 150; i++)
            filename_char[i] = '\0';

        // Просим пользователя ввести имя файла для запроса
       // std::cout << "Please, enter filename for requesting: ";
       // std::cin >> filename_char;

        // Преобразуем массив char в string
        std::string filename(ARV_FN);

        filename = filename + ".dat";

        //Пока что сделаем так (чтобы не вводить длинное название много раз)
       // filename = "43434.mp4";

        this->filename = filename;



        // Создаем переменную ошибки для совместимости с handle_handshake
        const boost::system::error_code error_my;
        // Вот он повторный вызов-то !!!
        this->handle_handshake(error_my);


    }

    // public variabels

  // ----------------------------

  int flag_size = 0;

    int flag_end = 0;

    int filesize;
    int total = 0;

    FILE* f;

    int size_flag = 0;
    int current_file_name = 0;

    std::string filename = "NO FILE";

    char responde[1024] = {'n','o',' ','r','e','s','p','o','n','d'};

 // ------------------------------

private:
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
    char* reply_;
};


int main(int argc, char* argv[])
{
    try
    {

        if (argc != 4)
        {
            std::cerr << "Usage: client <host> <port> <filename_for_request_from_server>\n";
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

        ARV_FN = argv[3];

        io_context.run();

        //Закрыли файл (на всякий случай тут тоже дублируем)
        fclose(c.f);

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
