//you have to change the main you got a bit, add while loop and add 2 functions to it.
//instead of printf , else printf , call the function print(2, "Fatal error") only once.
//The messages that you have to add with print are given in subject.

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>  // Includes the standard library for free.
#include <stdio.h>  // Includes the standard input/output like sprintf

int max_fd;        // Global variable for the highest file descriptor number.
int arr_id[5000];  // Array to store client IDs.
char *arr_str[5000];    // Array of pointers to store message strings from clients.
char *message = NULL;   // Pointer for storing the current message.

fd_set actual_set, read_set, write_set;  // fd_set structures for select(), to monitor sockets for reading and writing.

char buff_send[100];   // Buffer for sending messages.
char buff_read[1001];  // Buffer for receiving messages.

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

//------add the print function-----//
// This function sends a message to a specified file descriptor (fd) and then exits the program.
void print(int fd, char *s)
{
	write(fd, s, strlen(s));
	write(fd, "\n", 1);
	exit(1);
}


//------add the sen_msg function-----//
// This function broadcasts a message to all connected clients except the one that triggered the message.
// It iterates through all file descriptors from 3 up to max_fd (maximum file descriptor number),
// checking if each is ready for writing and not the source of the current message (fd).
void send_msg(int fd)
{
	for (int c_fd = 3; c_fd <= max_fd; c_fd++)
	{
		if (FD_ISSET(c_fd, &write_set) && c_fd != fd)
		{
			send(c_fd, buff_send, strlen(buff_send), 0);
			if (message)
				send(c_fd, message, strlen(message), 0);
		}
	}
}


int main(int argc, char **argv)
{

	if (argc != 2)   // you have to add the 2 line to the main
		print(2, "Wrong number of arguments"); // Checks if the correct number of arguments is given.

	int sockfd, connfd; //you have to change len Type
	socklen_t len;  // Typedef for the length of address structures.
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		print(2, "Fatal error"); //change printf to print function

	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1]));  // change the Port, Sets the port based on the argument.

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		print(2, "Fatal error");  //change printf to print function

	if (listen(sockfd, 10) != 0)
		print(2, "Fatal error");  //change printf to print function

	len = sizeof(cli);

	max_fd = sockfd;      		// Initializes max_fd with the server socket descriptor.
	FD_ZERO(&actual_set);		// Initializes the actual_set to monitor no file descriptors.
	FD_SET(sockfd, &actual_set);// Adds the server socket to the actual_set.
	int clientid = 0;			// Initializes client ID.


//----------add the entire while loop except the 2 line with accept, if (they are given with just change printf to print again)-----//
	while(1)   // Enters an infinite loop to handle incoming connections and messages
	{
		read_set = write_set = actual_set;  // Prepares the read and write sets from the actual set of monitored file descriptors
		if (select(max_fd + 1, &read_set, &write_set, NULL, NULL) <= 0)  // Checks for activity on any of the sockets. If there's no activity, continues to the next iteration
			continue;

		if (FD_ISSET(sockfd, &read_set)) // Checks if there is a new connection on the listening socket
		{
			connfd = accept(sockfd, (struct sockaddr *) &cli, &len); //it is given
			if (connfd < 0)    //it is given
				print(2, "Fatal error");  //change printf to print function

			arr_id[connfd] = clientid++;   // Assigns an ID to the new connection, adds it to the set of monitored descriptors, and updates max_fd if necessary
			FD_SET(connfd, &actual_set);
			max_fd = connfd > max_fd ? connfd : max_fd;
			sprintf(buff_send, "server: client %d just arrived\n", arr_id[connfd]);
			send_msg(connfd);
			arr_str[connfd] = NULL;
			continue;
		}

		for (int fd = 3; fd <= max_fd; ++fd) // Iterates over all file descriptors to check for incoming messages
		{
			if (FD_ISSET(fd, &read_set) && fd != sockfd)
			{

				int count = recv(fd, buff_read, 1000, 0);
				if (count <= 0)
				{
					FD_CLR(fd, &actual_set);
					sprintf(buff_send, "server: client %d just left\n", arr_id[fd]);
					send_msg(fd);
					close(fd);
					if (arr_str[fd] != NULL)
						free(arr_str[fd]);
					break;
				}
				else
				{
					buff_read[count] = '\0';
					arr_str[fd] = str_join(arr_str[fd], buff_read);
					message = NULL;
					while (extract_message(&arr_str[fd], &message))
					{
						sprintf(buff_send, "client %d: ", arr_id[fd]);
						send_msg(fd);
						free(message);
						message = NULL;
					}
				}
			}
		}
	}
}
