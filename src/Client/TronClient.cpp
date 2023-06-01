#include "TronClient.h"

#include "../SDLUtils/SDLUtils.h"
#include "../SDLUtils/Window.h"
#include "../SDLUtils/GameObject.h"
#include "GameUtils/Tablero.h"
#include "GameUtils/GameManager.h"

TronClient::TronClient(const char *s, const char *p) : client_socket(s, p)
{};

TronClient::~TronClient() {}

void TronClient::init(int w, int h)
{
	Window::init("TRON", w, h);
	GameManager::init();

	// Initial Data must be different for each player (TO DO)
	InitData data;
	data.pos = Vector2D(0, 0);
    data.dir = Vector2D(0, 0);

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

			if (event.type == SDL_KEYDOWN){

			}
			if ((event.type == SDL_KEYDOWN && currentState == MessageServer::ServerState::READY && event.key.keysym.scancode == SDL_SCANCODE_SPACE))
			{
				std::cout << "Start Game\n";
				sendGameMessage(MessageClient::InputType::PLAY);
				clearGameObjects();
				loadBackground("./assets/images/MenuReady.png");
				continue;
			}

			for (auto &o : objs_)
				if (o->isEnabled())
					o->handleInput(this, event);

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

		// Delay so the rendering is not too fast
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
			{
				case MessageServer::ActionType::MOVE:
				{
					if(m_player_1 != nullptr && m_player_2 != nullptr)
					{
						m_player_1->playerUpdate();
						m_player_2->playerUpdate();
					}
				}
				break;
			}
			break;
		}
		}
	}
}

void TronClient::updateGOsInfo(MessageServer *msg)
{
	if (currentState == MessageServer::ServerState::PLAYING)
	{
		//std::cout << "Update GOs info\n";

		// TO DO: actualizar posición de los jugadores, tal vez sea necesario actualizar la direccion que lleva
        if (m_player_1 != nullptr) {
            Coor h(msg->m_pos_p1.getX(), msg->m_pos_p1.getY());
            Coor c(msg->m_dir_p1.getX(), msg->m_dir_p1.getY());

            //m_player_1->setPlayerHead(h);
            m_player_1->ChangeDir(c);
        }
        if (m_player_2 != nullptr){
            Coor h(msg->m_pos_p2.getX(), msg->m_pos_p2.getY());
            Coor c(msg->m_dir_p2.getX(), msg->m_dir_p2.getY());

            //m_player_2->setPlayerHead(h);
            m_player_2->ChangeDir(c);
        }
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
		// load game elements: players and score
		loadGame();
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

void TronClient::loadGame(){

	GameManager().Start();
	Tablero* tablero = GameManager().getTablero();

	if (tablero != nullptr){
		objs_.push_back(tablero);
	}else{
		std::cout << "El tablero no existe\n";
	}

	// Add both players with initial position and size
	Coor p1_coor = GameManager::instance()->getTablero()->getPlayerOneInitialPosition();
	m_player_1 = new Player(p1_coor);
	//m_player_1->setTransform(200, Window().height() / 2);
	m_player_1->setSize(GameManager::instance()->BOX_WIDTH, GameManager::instance()->BOX_WIDTH);
	m_player_1->setTexture("./assets/images/Player1.png");
	
	objs_.push_back(m_player_1);

	Coor p2_coor = GameManager::instance()->getTablero()->getPlayerTwoInitialPosition();
	m_player_2 = new Player(p2_coor);
	//m_player_2->setTransform(800, Window().height() / 2);
	m_player_2->setSize(GameManager::instance()->BOX_WIDTH, GameManager::instance()->BOX_WIDTH);
	m_player_2->setTexture("./assets/images/Player2.png");
	
	objs_.push_back(m_player_2);

	//std::cout << "Pos p1: " << m_player_1->getPlayerHead() << "\n";
	//std::cout << "Pos p2: " << m_player_2->getPlayerHead() << "\n";

	// Tell the server each client's initial pos and dir


}

void TronClient::sendGameMessage(MessageClient::InputType input)
{
	// Send an input message
	MessageClient login;
	login.m_type = MessageClient::ClientMessageType::HANDLE_INPUT;
	login.m_input = input;		
	client_socket.send(login, client_socket);
}

void TronClient::sendMatchMessage(MessageClient::ClientMessageType msg, InitData *data)
{
	MessageClient login;
	login.m_type = msg;

	if (data != nullptr)
		login.setDefaultValues(data->pos, data->dir);

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