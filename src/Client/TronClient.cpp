#include "TronClient.h"

#include "../SDLUtils/SDLUtils.h"
#include "../SDLUtils/Window.h"
#include "../SDLUtils/GameObject.h"
#include "GameUtils/GameManager.h"

TronClient::TronClient(const char *s, const char *p) : client_socket(s, p)
{};

TronClient::~TronClient() {}

void TronClient::init(int w, int h)
{
	Window::init("TRON", w, h);
	GameManager::init();

	InitData data;
	data.size = Vector2D(PLAYER_SIZE, PLAYER_SIZE);
    data.position = Vector2D(0, 0);

	// init connection
	std::thread([this]()
		{ client_message_thread(); }).detach();
	
	changeState(currentState);

	sendMatchMessage(MessageClient::ClientMessageType::REGISTER, &data);
	std::cout << "Trying to log...\n";

}

void TronClient::run()
{
	SDL_Event event;

	while (nextState != MessageServer::ServerState::SERVER_QUIT)
	{
		checkState();

		Uint32 startTime = Window().currRealTime();
		while (SDL_PollEvent(&event))
		{
			// Salir del server
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
			{
				currentState = MessageServer::ServerState::SERVER_QUIT;
				continue;
			}

			if ((event.type == SDL_KEYDOWN && currentState == MessageServer::ServerState::READY && event.key.keysym.scancode == SDL_SCANCODE_RETURN))
			{
				std::cout << "Start Game\n";
				sendGameMessage(MessageClient::InputType::PLAY);
				clearGameObjects();
				loadBackground("./assets/images/MenuReady.png");
				continue;
			}

			for (auto &o : objs_)
				if (o->isEnabled())
					o->handleInput(event);
		}

		if (currentState == MessageServer::ServerState::SERVER_QUIT)
			break;

		// update
		for (auto &o : objs_)
			if (o->isEnabled())
				o->update();

		refresh();

		Window().clearRenderer({0, 0, 0});

		// render
		for (auto &o : objs_){
			if (o->isEnabled())
				o->render();
		}

		Window().presentRenderer();

		Uint32 frameTime = Window().currRealTime() - startTime;
		if (frameTime < 20)
			SDL_Delay(20 - frameTime);
	}

	sendMatchMessage(MessageClient::ClientMessageType::QUIT);
	std::cout << "Quitting...\n";
}

void TronClient::shutdown()
{
	clearGameObjects();
}

void TronClient::client_message_thread()
{
    MessageServer server_recv_msg;
	Socket *net_socket = new Socket(client_socket);
	while (true)
	{
		client_socket.recv(server_recv_msg, net_socket);
		switch (server_recv_msg.m_type)
		{
		case MessageServer::ServerMessageType::UPDATE_STATE:
			nextState = server_recv_msg.m_state;
			break;
		case MessageServer::ServerMessageType::UPDATE_INFO:
			updateGOsInfo(&server_recv_msg);
			break;
		case MessageServer::ServerMessageType::ACTION:
		{
			switch (server_recv_msg.m_action)
			break;
		}
		}
	}
}

void TronClient::updateGOsInfo(MessageServer *msg)
{
	if (currentState == MessageServer::ServerState::PLAYING)
	{
		// player_1->setTransform(msg->m_pos_p1);
		// player_2->setTransform(msg->m_pos_p2);
	}
}

void TronClient::checkState()
{
	if (nextState != MessageServer::ServerState::EMPTY && currentState != nextState)
	{
		changeState(nextState); // this makes it so there's no problems with thread

		currentState = nextState;
		nextState = MessageServer::ServerState::EMPTY;

		printf("Current State: %d -- Next State: %d \n", (int)currentState, (int)nextState);
	}
}

void TronClient::changeState(const MessageServer::ServerState state)
{
	std::cout << "Estado cambiado: objetos" <<objs_.size() <<"\n";

	clearGameObjects();
	std::cout << objs_.size() <<"\n";

	switch (state)
	{
	case MessageServer::ServerState::WAITING:
	{
		std::cout << "Waiting...";
		loadBackground("./assets/images/MenuWaiting.png");
		break;
	}
	case MessageServer::ServerState::READY:
	{
		std::cout << "Match found.";
		loadBackground("./assets/images/MenuPlay.png");
		break;
	}
	case MessageServer::ServerState::PLAYING:
		std::cout << "Playing...";
		loadBackground("./assets/images/GameWithBoard.png");
		break;
	case MessageServer::ServerState::GAME_OVER:
	{
		std::cout << "Game Over.";
		break;
	}
	default:
		break;
	}
}

void TronClient::loadBackground(const std::string &textFile)
{
	GameObject* bg = new GameObject();
	bg->setTransform(0, 0);
	bg->setSize(Window().width(), Window().height());
	bg->setTexture(textFile);
	bg->setEnabled(true);
	objs_.push_back(bg);

}

void TronClient::sendGameMessage(MessageClient::InputType input)
{
	MessageClient login;
	login.m_type = MessageClient::ClientMessageType::HANDLE_INPUT;
	login.m_input = input;
	client_socket.send(login, client_socket);
}

void TronClient::sendMatchMessage(MessageClient::ClientMessageType msg, InitData *data)
{
	MessageClient login;
	login.m_type = msg;

	// if (data != nullptr)
	// 	login.setDefaultValues(GameManager::instance()->getRelativeScenerioLimits().getX(), // Left
	// 						   GameManager::instance()->getScenerioLimits().getX(),			// Right
	// 						   GameManager::instance()->getRelativeScenerioLimits().getY(), // Top
	// 						   GameManager::instance()->getScenerioLimits().getY() + 45,	// Bottom
	// 						   data->dim, data->rot);

	client_socket.send(login, client_socket);
	printf("Sending Match Message...\n");
}

void TronClient::refresh()
{
	objs_.erase(std::remove_if(objs_.begin(), objs_.end(),
							   [](const GameObject *e)
							   {
								    if (e->isEnabled())
									    return false;
								    else
								    {
								 	    if (e != nullptr)
									    {
								 		    delete e;
										    e = nullptr;
										    return true;
									    }
									    else
										    return false;
								   }
							   }),
				objs_.end());
}

void TronClient::clearGameObjects()
{
	for (unsigned int i = 0; i < objs_.size(); i++)
	{
		if (objs_[i] != nullptr)
			delete objs_[i];
		objs_[i] = nullptr;
	}
	objs_.clear();
}