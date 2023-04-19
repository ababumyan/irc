#if !defined(EXCEPTIONS_HPP)
#define EXCEPTIONS_HPP

#include <exception>
#include <iostream>
#include "Server.hpp"
#include <sstream>

template <typename T>
std::string NumberToString(T Number)
{
	std::ostringstream ss;
	ss << Number;
	return ss.str();
}

class IRCException : public std::exception
{
protected:
	int errorCode;
	std::string name;
	std::string errorMessage;

private:
	mutable std::string fullMessage;

protected:
	IRCException(const std::string &_name, const std::string &_message, int _errorCode)
		: errorCode(_errorCode), name(_name), errorMessage(_message) {}

public:
	virtual const char *what() const throw()
	{
		//: name@host ERR_CODE name:reason
		fullMessage = ":" + name + "@" + Server::getServer()->getHost() + " " + NumberToString(errorCode) + errorMessage;
		return fullMessage.c_str();
	}
	virtual ~IRCException() throw() {}
};

struct NoSuchNick : public IRCException
{
	NoSuchNick(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :No such nick/channel", 401) {}
};

struct NoSuchServer : public IRCException
{
	NoSuchServer(const std::string &_name, const std::string &serverName)
		: IRCException(_name, " " + serverName + " :No such server", 402) {}
};

struct NoSuchChannel : public IRCException
{
	NoSuchChannel(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :No such channel", 403) {}
};

struct CannotSendToChannel : public IRCException
{
	CannotSendToChannel(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :Cannot send to channel", 404) {}
};

struct TooManyChannels : public IRCException
{
	TooManyChannels(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :You have joined too many channels", 405) {}
};

struct TooManyTargets : public IRCException
{
	TooManyTargets(const std::string &_name, const std::string &target)
		: IRCException(_name, " " + target + " :Duplicate recipients. No message delivered", 407) {}
};

struct NoOrigin : public IRCException
{
	NoOrigin(const std::string &_name) : IRCException(_name, " :No origin specified", 409) {}
};

struct NoRecipient : public IRCException
{
	NoRecipient(const std::string &_name, const std::string &command)
		: IRCException(_name, " :No recipient given " + command, 411) {}
};

struct NoTextToSend : public IRCException
{
	NoTextToSend(const std::string &_name) : IRCException(_name, " :No text to send", 412) {}
};

struct NoTopLevel : public IRCException
{
	NoTopLevel(const std::string &_name, const std::string &mask)
		: IRCException(_name, " " + mask + " :No toplevel domain specified", 413) {}
};

struct WildTopLevel : public IRCException
{
	WildTopLevel(const std::string &_name, const std::string &mask)
		: IRCException(_name, " " + mask + " :Wildcard in toplevel domain", 414) {}
};

struct UnknownCommand : public IRCException
{
	UnknownCommand(const std::string &_name, const std::string &command)
		: IRCException(_name, " " + command + " :Unknown command", 421) {}
};

struct NoNickNameGiven :
 public IRCException
{
	NoNickNameGiven(const std::string &_name) : IRCException(_name, " :No nickname given", 431) {}
};

struct ErroneusNickname : public IRCException
{
	ErroneusNickname(const std::string &_name, const std::string &nickname)
		: IRCException(_name, " " + nickname + " :Erroneus nickname", 432) {}
};

struct NicknameInUse : public IRCException
{
	NicknameInUse(const std::string &_name, const std::string &nickname)
		: IRCException(_name, " " + nickname + " :Nickname is already in use", 433) {}
};

struct NickColission : public IRCException
{
	NickColission(const std::string &_name, const std::string &nickname)
		: IRCException(_name, " " + nickname + " :Nickname collision KILL", 436) {}
};

struct UserNotInChannel : public IRCException
{
	UserNotInChannel(const std::string &_name, const std::string &nick,
					 const std::string &channelName) : IRCException(_name, " " + nick + " " + channelName + " They aren't on that channel", 441) {}
};

struct NotOnChannel : public IRCException
{
	NotOnChannel(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :You're not on that channel", 442) {}
};

struct NotRegistered : public IRCException
{
	NotRegistered(const std::string &_name)
		: IRCException(_name, " :You have not registered", 451) {}
};

struct NeedMoreParams : public IRCException
{
	NeedMoreParams(const std::string &_name, const std::string &command)
		: IRCException(_name, " " + command + " :Not enough parameters", 461) {}
};

struct AlreadyRegistered : public IRCException
{
	AlreadyRegistered(const std::string &_name) : IRCException(_name, " :You may not reregister", 462) {}
};

struct PasswordMissmatch : public IRCException
{
	PasswordMissmatch(const std::string &_name) : IRCException(_name, " :Password incorrect", 464) {}
};

struct KeySet : public IRCException
{
	KeySet(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :Channel key already set", 467) {}
};

struct ChannelIsFull : public IRCException
{

	ChannelIsFull(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :Cannot join channel (+l)", 471) {}
};

struct UnknownMode : public IRCException
{
	UnknownMode(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :is unknown mode char to me", 472) {}
};

struct InviteOnlyChannel : public IRCException
{

	InviteOnlyChannel(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :Cannot join channel (+i)", 473) {}
};

struct BannedFromChannel : public IRCException
{
	BannedFromChannel(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :Cannot join channel (+b)", 474) {}
};

struct BadChannelKey : public IRCException
{
	BadChannelKey(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :Cannot join channel (+k)", 475) {}
};

struct BadChannelMask : public IRCException
{
	BadChannelMask(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :Bad channel mask", 476) {}
};

struct ChannelOpPrivsNeeded : public IRCException
{
	ChannelOpPrivsNeeded(const std::string &_name, const std::string &channelName)
		: IRCException(_name, " " + channelName + " :You're not channel operator", 482) {}
};

struct UModeUnknownFlag : public IRCException
{
	UModeUnknownFlag(const std::string &_name)
		: IRCException(_name, " :Unknown MODE flag", 501) {}
};

struct UsersDontMatch : public IRCException
{
	UsersDontMatch(const std::string &_name)
		: IRCException(_name, " :Cant change mode for other users", 502) {}
};

struct NoBotConnected : public IRCException
{
	NoBotConnected(const std::string &_name)
		: IRCException(_name, " :No bot has been connected", 666) {}
};

#endif // EXCEPTIONS_HPP
