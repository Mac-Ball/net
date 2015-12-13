#ifndef NET_H
#define NET_H

/*Net.h - C++ Header file

 * This file contains functions for working with the net class.
 * Net class is used for remote access to one or more computers and 
 * exchange data between them using the UDP protocol.

 * Contributors:
 * Created by Vladislav Ivanov  <vladislavivanov2745@gmail.com>
 
 * This source code is offered for use in the public domain. You may
 * use, modify or distribute it freely.
 
 * This code is distributed in the hope that it will be useful but
 * WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 * DISCLAIMED. This includes but is not limited to warranties of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 
 * Content:
 
 * Info (struct) - contains ip address and hostname of computer.
 * .Start (function) - initialize Info struct.
 * .My_ip (function) - return ip.
 * .My_name (function) - return hostname.
 
 * Net (class) - main class.
 * ->Net (constructor) - create new Net connection.
 * ->Throw (function) - send the package.
 * ->Catch (function) - receive the package.
 * ->Close (function) - close Net connection.
 
*/

#ifdef __unix__

    #error "It does not work in *nix systems!"

#elif defined(_WIN32) || defined(WIN32)

    #include <windows.h>
    #include <winsock2.h>
    #include <cstdlib>
    #include <cstring>
    #include <iostream>

    #define THIS_MACHINE "127.0.0.1"
    #define NET_STD_PORT 1234
    #define SERVER 1
    #define CLIENT 0
    #define ANY ""

    struct
    {
        std::string My_name = "";
        std::string My_ip = "";

        void Start()
        {
            My_ip = "";
            My_name = "";

            char s[32];
            long unsigned int size_s = 32;
            char buffer[32];

            GetComputerName(s,&size_s);
            for(int i = 0;i < strlen(s);i++)
            {
                My_name = My_name + s[i];
            }

            hostent *h= gethostbyname(s);

            for(int i = 0;i < 4;i++)
            {
                itoa((int)(unsigned char)h->h_addr_list[0][i],buffer,10);
                for(int j = 0;j < strlen(buffer);j++)
                {
                    My_ip = My_ip + buffer[j];
                }
                if(i != 3)My_ip = My_ip + ".";
            }
        }

    }Info;

    typedef std::string package;

    class Net
    {
        public:

            Net(int _type,std::string _ip = THIS_MACHINE,int _port = NET_STD_PORT,bool log = 1)
            {
                type = _type;
                ip = _ip;
                port = _port;
                if(WSAStartup(MAKEWORD(2,1), &data) != 0)
                    std::cout << "Start Up error!" << std::endl;
                else
                {
                    if(log)std::cout << "Start Up complete!" << std::endl;
                }

                s = socket(AF_INET,SOCK_DGRAM,0);
                if(s == SOCKET_ERROR)
                    std::cout << "Socket creating error!" << std::endl;
                else
                {
                    if(log)std::cout << "Socket creating complete!" << std::endl;
                }


                if(type == CLIENT)
                {
                    addr.sin_family = AF_INET;
                    addr.sin_port = htons(port);
                    addr.sin_addr.s_addr = inet_addr(ip.c_str());
                }

                if(type == SERVER)
                {
                    addr.sin_family = AF_INET;
                    addr.sin_port = htons(port);
                    if(ip == ANY)addr.sin_addr.s_addr = htonl(INADDR_ANY);
                    else addr.sin_addr.s_addr = inet_addr(ip.c_str());

                    if(bind(s,(sockaddr*)&addr,sizeof(addr)) == SOCKET_ERROR)
                        std::cout << "Server binding error!" << std::endl;
                    else
                    {
                        if(log)std::cout << "Server binding complete!" << std::endl;
                    }

                }

                addr_size = sizeof(addr);

                Info.Start();
            }

            void Throw(package msg)
            {
                if(sendto(s,msg.c_str(),msg.length() + 1,0,(sockaddr*)&addr,sizeof(addr)) == SOCKET_ERROR)
                {
                    std::cout << "Message sending error!" << std::endl;
                }

            }

            package Catch()
            {
                buffer_s = "";
                ZeroMemory(buffer_c,sizeof(buffer_c));
                if(recvfrom(s,buffer_c,sizeof(buffer_c) + 1,0,(sockaddr*)&addr,&addr_size) == SOCKET_ERROR)
                {
                    std::cout << "Message receiving error!" << std::endl;
                }

                for(int i = 0;i < strlen(buffer_c);i++)
                {
                    buffer_s = buffer_s + buffer_c[i];
                }

                return buffer_s;
            }

            void Close()
            {
                closesocket(s);
                WSACleanup();
            }

        private:

            int type;
            std::string buffer_s;
            char buffer_c[1024];
            int addr_size;
            sockaddr_in addr;
            SOCKET s;
            WSAData data;
            std::string ip = THIS_MACHINE;
            int port = NET_STD_PORT;

    };

    void operator << (Net* a,std::string b)
    {
        a->Throw(b);
    }

    void operator >> (Net* a,std::string& b)
    {
        b = a->Catch();
    }

    #endif

#endif // NET_H
