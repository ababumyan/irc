template <CommandType::Type type>
void	Command<type>::validate(Client &sender, const std::vector<std::string> &arguments)
{
	(void) sender;
	(void) arguments;

}

template<>
void	Command<CommandType::pass>::validate(Client &sender, const std::vector<std::string> &arguments)
{
	if(sender.getIsPassed())
		throw AlreadyRegistered(sender.getNick());
	if(arguments.size() == 0)
		throw NeedMoreParams(sender.getNick(),"PASS");
	if (Server::getServer()->getPass() != arguments[0])
		throw PasswordMissmatch(sender.getNick());
}

template<>
void	Command<CommandType::user>::validate(Client &sender,const std::vector<std::string> &arguments)
{
	if(sender.getIsPassed() == false)
		throw NotRegistered(sender.getNick());
	if(arguments.size() < 4)
		throw NeedMoreParams(sender.getNick(),"USER");
	if(sender.getIsUsered())
		throw AlreadyRegistered(sender.getNick());
}

template<>
void	Command<CommandType::nick>::validate(Client &sender,const std::vector<std::string> &arguments)
{
	if(sender.getIsPassed() == false)
		throw NotRegistered(sender.getNick());
	if (arguments.size() == 0)
		throw NoNickNameGiven(sender.getName());
	if (ClientManager::getManager()->HasClient(arguments[0]))
		throw NicknameInUse(arguments[0],sender.getNick());
	if (!MessageController::getController()->IsValidNickname(arguments[0]))
		throw ErroneusNickname(sender.getNick(), arguments[0]);
}

template<>
void	Command<CommandType::ping>::validate(Client &sender,const std::vector<std::string> &arguments)
{
	if(sender.isDone() == false)
		throw NotRegistered(sender.getNick());
	if(arguments.size() == 0)
		throw NeedMoreParams(sender.getNick(),"PING");
}
 
template<>
void	Command<CommandType::pong>::validate(Client &sender,const std::vector<std::string> &arguments)
{
	if(sender.isDone() == false)
		throw NotRegistered(sender.getNick());
	if(arguments.size() == 0)
		throw NeedMoreParams(sender.getNick(),"PONG");
}
 
template<>
void	Command<CommandType::privmsg>::validate(Client &sender,const std::vector<std::string> &arguments)
{
	if(sender.isDone() == false)
		throw NotRegistered(sender.getNick());
	MessageController *messageController = MessageController::getController();
	ClientManager *client_managar = ClientManager::getManager();
	Server *server = Server::getServer();
	std::vector<std::string> args = messageController->Split(arguments[0],",");
	if(arguments.size() <= 1)
		throw NeedMoreParams(sender.getNick(),"PRIVMSG");
	for (size_t i = 0; i < args.size(); i++)
	{
		std::string channelName = messageController->GetChannelName(args[i]);

		if(server->HasChannel(channelName))
		{
			if(!(server->getChannel(channelName).HasMode(ModeType::write_)))
				throw CannotSendToChannel(sender.getNick(),channelName);
			if(!server->IsBot(sender) && !(server->getChannel(channelName).HasMember(sender.getSocket())))
				throw NoSuchNick(sender.getNick(),"PRIVMSG");
		}
		else if(client_managar->HasClient(args[i]) == false)
			throw NoSuchNick(sender.getNick(),args[i]);
						
	}
}
 
template<>
void	Command<CommandType::join>::validate(Client &sender,const std::vector<std::string> &arguments)
{
	if(sender.isDone() == false)
		throw NotRegistered(sender.getNick());
	if(arguments.size() == 0)
		throw NeedMoreParams(sender.getNick(),"JOIN");
	Server *server =  Server::getServer();
	MessageController *messageController = MessageController::getController();
	std::vector<std::string> args = messageController->Split(arguments[0],",");
	for (size_t i = 0; i < args.size(); i++)
	{
		if (!messageController->IsValidChannelName(args[i]))
			throw BadChannelMask(sender.getNick(), args[i]);
		std::string channelName = messageController->GetChannelName(args[i]);
		if (server->HasChannel(channelName))
		{
			Channel	channel = server->getChannel(channelName);
			if (channel.HasMode(ModeType::invite))
				throw InviteOnlyChannel(sender.getNick(),channelName);
			if (channel.HasMode(ModeType::private_) && !channel.CheckPassword(arguments[1]))
				throw BadChannelKey(sender.getNick(), channelName);
		}
	}
}
 
template<>
void	Command<CommandType::part>::validate(Client &sender,const std::vector<std::string> &arguments)
{
	if(sender.isDone() == false)
		throw NotRegistered(sender.getNick());
	Server *server = Server::getServer();
	if(arguments.size() == 0)
		throw NeedMoreParams(sender.getNick(),"PART");
	MessageController *messageController = MessageController::getController();
	std::vector<std::string> channels = messageController->Split(arguments[0],",");
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (!messageController->IsValidChannelName(channels[i]))
			throw BadChannelMask(sender.getNick(), channels[i]);
		std::string channelName = messageController->GetChannelName(channels[i]);
		if(!server->HasChannel(channelName))
			throw NoSuchChannel(sender.getNick(),channels[i]);
		Channel channel = server->getChannel(channelName);
		if(channel.HasMember(sender.getSocket()) == false)
			throw NotOnChannel(sender.getNick(),channelName);
	}
}
 
template<>
void	Command<CommandType::kick>::validate(Client &sender,const std::vector<std::string> &arguments)
{
	std::string chName = arguments[0];
	if(sender.isDone() == false)
		throw NotRegistered(sender.getNick());
	if(arguments.size() != 2)
		throw NeedMoreParams(sender.getNick(),"KICK");
	if (!MessageController::getController()->IsValidChannelName(chName))
		throw BadChannelMask(sender.getNick(), chName);
	std::string channelName = MessageController::getController()->GetChannelName(chName);
	Server *server = Server::getServer();
	if(server->HasChannel(channelName) == false)
		throw NoSuchChannel(sender.getNick(),channelName);
}

void	ValidateChannelMode(const Client &sender, const std::vector<std::string> &arguments)
{
	Server *server = Server::getServer();
	MessageController *messageController = MessageController::getController();
	ClientManager	*clientManager = ClientManager::getManager();

	std::string channelName = messageController->GetChannelName(arguments[0]);
	if(server->HasChannel(channelName) == false)
		throw NoSuchChannel(sender.getNick(),channelName);
	Channel &channel = server->getChannel(channelName);
	if (arguments.size() == 1)
		return ;

	std::string modeString = arguments[1];
	size_t index = modeString.find_first_not_of("+-wriokb");
	if (index != std::string::npos)
		throw UnknownMode(sender.getNick(), modeString);
	int plusSign  = messageController->SignCount(modeString, '+');
	int minusSign = messageController->SignCount(modeString, '-');
	if ((plusSign != 0 && plusSign != 1) ||
		(minusSign != 0 && minusSign != 1))
		throw UnknownMode(sender.getNick(), modeString);
	if (*modeString.rbegin() == '+' || *modeString.rbegin() == '-')
		throw UnknownMode(sender.getNick(), modeString);

	std::string addingModes = messageController->GetModesString(modeString, '+');
	std::string removingModes = messageController->GetModesString(modeString, '-');

	std::cout << "ADDING modes: " << addingModes << std::endl;
	std::cout << "REMOVING modes: " << removingModes << std::endl;
	for(size_t i = 0; i < addingModes.length(); ++i)
	{
		char set = addingModes[i];
		if(set == 'o' || set == 'k')
		{
			if(channel.IsAdmin(sender.getSocket()) == false)
				throw UsersDontMatch(sender.getNick());
			if (arguments.size() < 3)
				throw NeedMoreParams(sender.getNick(),"MODE");
		}
		int clientSocket = clientManager->GetClientSocket(arguments[i + 1]);
		if (set == 'o' && !channel.HasMember(clientSocket))
				throw UserNotInChannel(sender.getName(),sender.getNick(), channelName);
	}
	for(size_t i = 0; i < removingModes.length(); ++i)
	{
		char set = removingModes[i];
		if(set == 'o')
		{
			if(channel.IsAdmin(sender.getSocket()) == false)
				throw UsersDontMatch(sender.getNick());
			if (arguments.size() < 3)
				throw NeedMoreParams(sender.getNick(),"MODE");
			int clientSocket = clientManager->GetClientSocket(arguments[i + 1]);
			if (!channel.HasMember(clientSocket))
				throw UserNotInChannel(sender.getName(),sender.getNick(), channelName);
		}
	}
}

template<>
void	Command<CommandType::mode>::validate(Client &sender,const std::vector<std::string> &arguments)
{
	if(sender.isDone() == false)
		throw NotRegistered(sender.getNick());

	ClientManager *clientManager = ClientManager::getManager();
	MessageController *messageController = MessageController::getController();

	if (messageController->IsValidChannelName(arguments[0]))
	{
		ValidateChannelMode(sender, arguments);
	}
	else if (clientManager->HasClient(arguments[0]))
	{
		if (sender.getNick() != arguments[0])
			throw UsersDontMatch(arguments[0]);
	}
	else
		throw NoSuchNick(sender.getNick(), arguments[0]);
}

template <>
void	Command<CommandType::who>::validate(Client &sender, const std::vector<std::string> &arguments)
{
	(void) arguments;
	if(sender.isDone() == false)
		throw NotRegistered(sender.getNick());
	if(arguments.size() == 0)
		throw NeedMoreParams(sender.getNick(),"WHO");
	std::string target = arguments[0];
	MessageController *controller = MessageController::getController();
	Server *server = Server::getServer();
	if(controller->IsValidChannelName(target))
	{
		std::string channelName = controller->GetChannelName(arguments[0]);
		if(server->HasChannel(channelName) == false)
			throw NoSuchChannel(sender.getNick(),arguments[0]);
		if(server->getChannel(channelName).HasMember(sender.getSocket()) == false)
			throw NotOnChannel(sender.getNick(),arguments[0]);
	}
	if(ClientManager::getManager()->HasClient(sender.getSocket()) == false)
		throw NoSuchNick(sender.getNick(),"WHO");
}

template<>
void	Command<CommandType::bot>::validate(Client &sender, const std::vector<std::string> &arguments)
{
	if(sender.isDone() == false)
		throw NotRegistered(sender.getNick());
	Server *server = Server::getServer();
	MessageController *controller = MessageController::getController();
	ClientManager *manager = ClientManager::getManager();
	if (arguments.size() == 0)
		throw NeedMoreParams(sender.getNick(), "/bot");
	if (!server->IsBotConnected())
		throw NoBotConnected(sender.getNick());
	if (arguments.size() == 1)
		return ;
	std::string channelName = controller->GetChannelName(arguments[1]);
	if (controller->IsValidChannelName(arguments[1]))
	{
		if (!server->HasChannel(channelName))
			throw NoSuchChannel(sender.getNick(), channelName);
		Channel &channel = server->getChannel(channelName);
		if (!channel.HasMember(sender.getSocket()))
			throw UserNotInChannel(sender.getNick(), sender.getNick(), channelName);
	}
	else if (!manager->HasClient(arguments[1]))
		throw NoSuchNick(sender.getNick(), channelName);
}