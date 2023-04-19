#if !defined(IMESSENGER_HPP)
#define IMESSENGER_HPP

#include <iostream>

class IMessenger
{
	public:
		virtual ~IMessenger();
		void	SendMessageWithSocket(int clientSocket, const std::string &message) const;
};


#endif // IMESSENGER_HPP
