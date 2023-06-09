#include "../../includes/commands.hpp"

bool	parse_nick(std::string &nickname) {
	if (nickname.size() > 9) {
		// std::cerr << "[NICK] Error: nickname too long" << std::endl;
		return false;
	}
	if (nickname.find_first_not_of(NICK_VALID_CHARS) != std::string::npos) {
		// std::cerr << "[NICK] Error: invalid nickname" << std::endl;
		return false;
	}
	return true;
}

bool	is_nick_taken(Server &server, std::string &nickname) {
	for (std::map<int, Client*>::iterator it = server.getClients().begin();
			it != server.getClients().end(); it++) {
		if (it->second->getNickname() == nickname) {
			return true;
		}
	}
	return false;
}

bool	cmd_nick(Server &server, Client &client, std::vector<std::string> &input) {
	if (input.size() < 2) {
		client.send_message(ERR_NEEDMOREPARAMS(client.getNickname(), "NICK"));
		return false;
	}
	if (client.getNickname() == input[1]) {
		return true;
	}
	if (!parse_nick(input[1])) {
		client.send_message(ERR_ERRONEUSNICKNAME(input[1]));
		return false;
	}
	if (is_nick_taken(server, input[1])) {
		if (client.getNickname().empty())
		{
			while (is_nick_taken(server, input[1]))
				input[1] += "_";
		}
		else 
		{
			client.send_message(ERR_NICKNAMEINUSE(input[1]));
			return false;
		}
	}
	// if (is_nick_collision(server, input[1])) {
	// 	TODO: send ERR_NICKCOLLISION
	// 	return false;
	// }
	client.setNickname(input[1]);
	client.send_message(NICK(client.getNickname(), client.getHostname(), input[1]));
	if (!client.getNickname().empty() && !client.getUsername().empty() && !client.getRealname().empty())
		client.setRegistered(true);
	if (client.isRegistered() && client.isAuth())
		client.send_message(RPL_WELCOME(client.getNickname()));
	else if (!client.isAuth()) {
		server.removeClient(server.getClientByNick(client.getNickname()));
		return false;
	}
	return true;
}
