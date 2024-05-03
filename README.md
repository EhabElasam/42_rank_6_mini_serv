## Modified Mini Serv Project
# Introduction

 with modifications aimed at addressing specific requirements for the Rank 6 examination at the 42 School. It enhances the original simple server implementation to support continuous client connection handling and message broadcasting among multiple clients.  
Modifications Include:

  Implementing a persistent while loop in the main function for ongoing client and message management.
  Replacing printf statements with a custom print function for unified error handling, using the error messages specified in the project guidelines.
  Adding two critical functions: send_msg for message broadcasting to clients and extract_message for parsing incoming messages.

Getting Started
# Prerequisites

Ensure you have a C compiler installed on your system, such as gcc or clang.
# Compilation

Compile the project using the following command:


    gcc -Wall -Wextra -Werror -o mini_serv mini_serv.c

This creates an executable named mini_serv.
# Running the Server

Start the server on a specified port with:


    ./mini_serv <port_number>

Replace <port_number> with your desired port.
# Usage

Clients can connect to the server using tools like netcat by specifying the server's IP address and port. For example:


    netcat 127.0.0.1 <port_number>

Once connected, clients can send messages to the server, which then broadcasts these messages to all other connected clients.
# Acknowledgments

Modifications made for educational purposes under the guidance of 42 School's curriculum.
