#include "IMessenger.hpp"
#include "MessageController.hpp"
#include <stdio.h>
#include <sys/socket.h>

IMessenger::~IMessenger() { }

void	IMessenger::SendMessageWithSocket(int clientSocket,
	const std::string &message) const
{
	if (send(clientSocket, (message + "\n").c_str(), message.length() + 2, 0) < 0)
		perror("send");
}