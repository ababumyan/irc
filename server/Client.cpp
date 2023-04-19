#include "Client.hpp"
#include <sys/socket.h>
#include "MessageController.hpp"
#include "Server.hpp"

Client::Client()
    : name("default name")
{

}

Client::Client(int _socketfd)
    : isPassed(false), isUsered(false), isNicked(false), name(""), fd(_socketfd)
{ }

Client::~Client()
{

}

std::string	Client::getName() const { return (name); }

std::string	Client::getNick() const { return (nick); }

int Client::getSocket() const { return (fd); }


bool Client::getIsPassed()const 
{
	return this->isPassed;
}

bool Client::getIsNicked()const 
{
	return this-> isNicked;
}

bool Client::getIsUsered()const 
{
	return this->isUsered;
}

void Client::setIsPassed(bool isPassed)
{
	this->isPassed = isPassed;
}
void Client::setIsUsered(bool is_usered)
{
	this->isUsered = is_usered;
}
void Client::setIsNicked(bool is_Nicked)
{
	this->isNicked = is_Nicked;
}
void Client::setName(std::string const &name)
{
	this->name = name;
}
void Client::setNick(std::string const &nick)
{
	if((this->nick == "") == false)
        	this->SendMessageWithSocket(this->fd,":" + this->nick + " NICK " + nick);
	this->nick = nick;
}

bool Client::isDone()
{
	if(isPassed && isUsered && isNicked)
		return true;
	return false;
}


std::string	Client::GetFormattedText() const
{
	std::string formatted = ":";

	if (isNicked)
		formatted += nick;
	if (isUsered)
		formatted += "!" + name;
	formatted += "@" + Server::getServer()->getHost();
	return (formatted);
}


void	Client::SendMessage(const Client &reciever,
	const std::string &commmand, const std::string message) const
{
	//:senderNickname!name@host COMMAND recieverNickname:message
	std::string finalizedMessage = GetFormattedText() + " "
		+ commmand + " " + reciever.getNick() + " : " + message;
	SendMessageWithSocket(reciever.getSocket(), finalizedMessage);
}

void	Client::SendSelf(const std::string &message) const
{
	SendMessageWithSocket(fd, message);
}

void	Client::SendPongMessage(const std::string &message) const
{
	std::string finalizedMessage = GetFormattedText()
		+ " PONG :" + message;
	SendSelf(finalizedMessage);
}

void	Client::SendMessageFromChannel(const Client &reciever, const std::string &command,
	const std::string &channel, const std::string message) const
{
	//:senderNickname!name@host COMMAND recieverNickname:message
	std::string finalizedMessage = GetFormattedText() + " "
		+ command + " #" + channel  + " :" + message;
	SendMessageWithSocket(reciever.getSocket(), finalizedMessage);
}

bool	Client::operator==(const Client &rhs) const
{
	return (this->fd == rhs.fd);
}