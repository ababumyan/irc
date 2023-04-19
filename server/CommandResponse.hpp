#if !defined(COMMAND_RESPONSE_HPP)
#define COMMAND_RESPONSE_HPP


#include "Client.hpp"

class CommandResponse : public IMessenger
{
public:
	void	SendMessageToClient(const Client &client, const std::string &message) const;
	void	SendJoinMessage(const Client &client,std::string const &channelName)const;
	void	SendReplyName(const Client &client)const;
	void	PartMessage(const Client &client, std::string const &channelName)const;
	void	KickMessage(const Client &client, std::string const &channelName,std::string const &admin)const;
	void	WhoMessage(const Client &client, const std::string &channelName) const;
	void	UserModeMessage(const Client &client) const;
	void	ChannelModeMessage(const Client &client, const std::string &channelName) const;

};


#endif // COMMAND_RESPONSE_HPP
