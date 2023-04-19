#include "Channel.hpp"
#include "Exceptions.hpp"
#include "ClientManager.hpp"
#include <algorithm>
#include "MessageController.hpp"
#include "Server.hpp"
#include "Exceptions.hpp"

Channel::Channel() {}

Channel::~Channel() {}

void Channel::AddMember(int clientSocket)
{
	if (members.size() == 0)
		SetAdmin(clientSocket);
	Client addingClient = ClientManager::getManager()->getClient(clientSocket);
	members.insert(std::pair<int, Client>(clientSocket, addingClient));
}

void Channel::KickMember(int admin, int memberSocket)
{
	//":" + admin_->getPrefix() + " KICK " + name_ + " " + client->getNick() + " :" + reason
	/* const Client &admin_class = ClientManager::getManager()->getClient(admin);
	std::string message = ":" + admin_class.GetFormattedText() + " KICK " + name + " " + memberNick + " : BYE!!!";
	std::map<std::string */
	ValidateCanModifyAdmin(admin, memberSocket);
	LeaveMember(memberSocket);
}

void	Channel::LeaveMember(int memberSocket)
{
	ValidateAdminIsInChannel(memberSocket);
	members.erase(memberSocket);
	if (IsAdmin(memberSocket))
		DeleteAdmin(memberSocket);
}

void	Channel::LeaveIfMember(int memberSocket)
{
	if (members.count(memberSocket) != 0)
	{
		members.erase(memberSocket);
		if (IsAdmin(memberSocket))
			DeleteAdmin(memberSocket);
	}
}

void Channel::MakeAdmin(int admin, int newAdmin)
{
	ValidateCanModifyAdmin(admin, newAdmin);
	SetAdmin(newAdmin);
}

void Channel::SetAdmin(int newAdmin)
{
	admins.push_back(newAdmin);
}

int	Channel::GetAdmin() { return *(admins.begin()); }

std::string	Channel::GetNickWithSocket(int socket) const
{
	std::map<int, Client>::const_iterator it = members.find(socket);
	return it->second.getNick();
}


void Channel::RemoveFromAdmins(int admin, int removingAdmin)
{
	ValidateCanModifyAdmin(admin, removingAdmin);

	DeleteAdmin(removingAdmin);
}

void	Channel::DeleteAdmin(int removingAdmin)
{
	std::vector<int>::iterator it = std::find(admins.begin(), admins.end(), removingAdmin);
	admins.erase(it);
	if (admins.empty() && !members.empty())
		SetAdmin(members.begin()->first);
}


void Channel::ValidateCanModifyAdmin(int admin, int modifyingMember) const
{
	ValidateAdmin(admin);
	ValidateClientIsInServer(modifyingMember);
	ValidateClientIsInChannel(admin, modifyingMember);
}

void Channel::ValidateAdmin(int admin) const
{
	ValidateClientIsInServer(admin);
	ValidateAdminIsInChannel(admin);
	if (!IsAdmin(admin))
		throw ChannelOpPrivsNeeded(GetNickWithSocket(admin), this->name);
}

void Channel::ValidateClientIsInChannel(int admin, int client) const
{
	if (!HasMember(client))
		throw UserNotInChannel(GetNickWithSocket(admin), GetNickWithSocket(client), this->name);
}

void Channel::ValidateAdminIsInChannel(int admin) const
{
	if (!HasMember(admin))
		throw NotOnChannel(GetNickWithSocket(admin), this->name);
}

void Channel::ValidateClientIsInServer(int client) const
{
	if (!ClientManager::getManager()->HasClient(client))
		throw NoSuchNick(GetNickWithSocket(client), this->name);
}

bool Channel::IsAdmin(int memberSocket) const
{
	std::vector<int>::iterator it = std::find(admins.begin(), admins.end(), memberSocket);
	return (it != admins.end());
}

bool Channel::HasMember(int memberSocket) const
{
	return (members.count(memberSocket));
}

void Channel::Broadcast(const Client &sender,
	const std::string &message, const std::string &command) const
{
	for (std::map<int, Client>::const_iterator it = members.begin();
		it != members.end(); it++)
	{
		if (!(it->first == sender.getSocket()))
			sender.SendMessageFromChannel(it->second, command, this->name, message);
	}
}

void	Channel::SendChannelReply(const std::string &message) const
{
	Server *server = Server::getServer();
	for (std::map<int, Client>::const_iterator it = members.begin();
		it != members.end(); it++)
	{
		server->SendMessageToClient(it->second, message);
		std::cout << message <<std::endl;
	}
}

void	Channel::SendJoinReply(const Client &client) const
{
	Server *server = Server::getServer();
	std::string message_body;
	for (std::map<int, Client>::const_iterator it = members.begin(); it != members.end();it++)
	{
		std::string sign = " :+";
		if(IsAdmin(it->first))
			sign = " :@";
		message_body = "353 " + client.getNick() + " = " + "#" + name + sign + it->second.getNick() ;
		server->SendMessageToClient(client, message_body);
		std::cout << message_body <<std::endl;
	}
	//366 a2 #a :End of /NAMES list
	message_body = "366 " +  client.getNick() + " " + "#"+ name + " :End of /NAMES list";
	server->SendMessageToClient(client, message_body);
	std::cout << message_body <<std::endl;
}

void	Channel::SendWhoReply(const Client &client) const
{
	Server	*server = Server::getServer();
	std::string host = server->getHost();
	for (std::map<int, Client>::const_iterator it = members.begin(); it != members.end();it++)
	{
		std::string sign = " +";
		if(IsAdmin(it->first))
			sign = " @";
		std::string message_body = "352 " + client.getNick() + " "+ "#"+name 
			+ " " + it->second.getName() + " " + host + " IRC Server "
				 + it->second.getNick() + " " + sign + " :0 " + it->second.getName();
		server->SendMessageToClient(client, message_body);
	}
}

void Channel::PrintData()
{
	std::cout << "CHANNEL: " << name << std::endl;
	std::cout << "PASSWORD: " << password << std::endl;
	std::cout << "MEMBERS: " << std::endl;
	for (std::map<int, Client>::iterator it = members.begin();
		it != members.end(); it++)
	{
		std::cout << "Nick: " << it->first << ", fd: " << it->second.getSocket() << std::endl;
	}
	std::cout << std::endl;
	std::cout << "ADMINS: " << std::endl;
	for (std::vector<int>::iterator it = admins.begin();
		it != admins.end(); it++)
	{
		std::cout << "Socket: " << *it << std::endl;
	}
	std::cout << "MODES: " << std::endl;
	std::cout << "WRITEONLY: " << (mode & ModeType::write_) << std::endl
		<< "READONLY: " << (mode & ModeType::read) << std::endl
		<< "INVITEONLY: " << (mode & ModeType::invite) << std::endl
		<< "PRIVATE: " << (mode & ModeType::private_) << std::endl;
 	std::cout << std::endl;
}

void	Channel::SetPassword(const std::string &_password)
{
	password = _password;
}

bool	Channel::CheckPassword(const std::string &_checkingPass) const
{
	return (password == _checkingPass);
}


int Channel::getMemberCount()
{
	return members.size();
}

Channel::Channel(std::string const &_name) : name(_name), password(""), mode(ModeType::none) { }

int Channel::HasMode(ModeType::Mode modeToCheck) const
{
	return (mode & modeToCheck);
}

void Channel::AddMode(ModeType::Mode newMode)
{
	mode |= newMode;
}

void Channel::RemoveMode(ModeType::Mode mode)
{
	this->mode ^= mode;
}

std::string	Channel::ModeInfo() const
{
	std::string modesString = "";

	std::cout <<"Password: " <<  password << std::endl;
	if (mode & ModeType::read)
		modesString += 'r';
	if (mode & ModeType::write_)
		modesString += 'w';
	if (mode & ModeType::invite)
		modesString += 'i';
	if (mode & ModeType::private_)
		modesString += 'k';
	if (mode & ModeType::private_)
		modesString += (" " + password);
	return (modesString);
}

void Channel::ChannelWhoResponse(Client const &client)
{

	/*"352 " + client->getNick() + " " + name_ + " " + (*it)->getUser() + " " + 
	(*it)->getHost() + " ft_irc " + (*it)->getNick() 
	+ " " + "H" + " :1 " + (*it)->getReal()*/
	std::string message;
	std::map<int, Client>::iterator it = members.begin();
	for (; it != members.end(); ++it)
	{
		message = "352 " + client.getNick() + " " + "#" +name + " " + it->second.getName() +
		" 127.0.0.1 irc " + it->second.getNick() + " H :1 " + it->second.getName();
		SendMessageToClient(client,message);
	}
	message = "315 " + client.getNick() + " " + name + " :End of WHO list";
	SendMessageToClient(client,message);
}

void Channel::ChannelJoinResponse(Client const &client)
{
	
	std::string message = client.GetFormattedText() + " JOIN " + "#" + name;
	std::map<int, Client>::iterator it = members.begin();
	if(it == members.end())
		std::cout << "errrorrr" << std::endl;
	for (; it != members.end() ; it++)
	{
		SendMessageWithSocket(it->second.getSocket(),message);
		std::cout << "sockett = "<<it->second.getSocket() << std::endl;
	}
	SendJoinReply(client);
}