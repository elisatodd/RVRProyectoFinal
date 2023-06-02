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

	InitData data;
	data.pos = Vector2D(0, 0);
    data.dir = Vector2D(0, 0);

	// init connection
	std::thread([this]()
		{ client_message_thread(); }).detach();
	
	changeState(currentState);

	sendMatchMessage(MessageClient::ClientMessageType::REGISTER, &data);
	std::cout << "[Client]: Trying to log...\n";

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
			handleInput(event);
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

		if (m_score_p1 != nullptr) m_score_p1->render();
		if (m_score_p2 != nullptr) m_score_p2->render();

		Window().presentRenderer();

		// Delay so the rendering is not too fast
		Uint32 frameTime = Window().currRealTime() - startTime;
		if (frameTime < 20)
			SDL_Delay(20 - frameTime);
	}

	sendMatchMessage(MessageClient::ClientMessageType::QUIT);
	//std::cout << "[Client]: Quitting...\n";
}

void TronClient::handleInput(const SDL_Event &event)
{
	// Will tell the server what input the client has pressed

	// Exit the server
	if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
	{
		currentState = MessageServer::ServerState::SERVER_QUIT;
		return;
	}

	if (event.type == SDL_KEYDOWN){ // In-Game input
		if (currentState == MessageServer::ServerState::PLAYING){
			switch(event.key.keysym.scancode){
				case SDL_SCANCODE_W : case SDL_SCANCODE_UP :{
					sendGameMessage(MessageClient::InputType::UP);
					break;
				}
				case SDL_SCANCODE_A : case SDL_SCANCODE_LEFT :{
					sendGameMessage(MessageClient::InputType::LEFT);
					break;
				}
				case SDL_SCANCODE_S : case SDL_SCANCODE_DOWN :{
					sendGameMessage(MessageClient::InputType::DOWN);
					break;
				}
				case SDL_SCANCODE_D : case SDL_SCANCODE_RIGHT :{
					sendGameMessage(MessageClient::InputType::RIGHT);
					break;
				}
			}
		}// Menu input
		else if (currentState == MessageServer::ServerState::READY && event.key.keysym.scancode == SDL_SCANCODE_SPACE){
			std::cout << "Start Game\n";
			sendGameMessage(MessageClient::InputType::PLAY);
			clearGameObjects();
			loadBackground("./assets/images/MenuReady.png");
		}
	} 
}

void TronClient::shutdown()
{
	clearGameObjects();

	delete m_score_p1;
	delete m_score_p2;
}

void TronClient::client_message_thread()
{
	// Manages the messages received
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
				// Change rendering info according to server
				updateGOsInfo(&server_recv_msg);
				break;
			case MessageServer::ServerMessageType::ACTION:
			{
				// Update game logic according to server
				switch (server_recv_msg.m_action)
				{
					case MessageServer::ActionType::MOVE:
					{
						if(m_player_1 != nullptr && m_player_2 != nullptr)
						{
							m_player_1->playerUpdate();
							m_player_2->playerUpdate();
						}
						break;
					}
				}
				break;
			}
			case MessageServer::ServerMessageType::ROUND_FINISHED:
			{
				if(m_player_1 != nullptr && m_player_2 != nullptr){
					m_player_1->ResetPosition();
					m_player_2->ResetPosition();
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

        if (m_player_1 != nullptr) {
            Coor h(msg->m_pos_p1.getX(), msg->m_pos_p1.getY());
            Coor c(msg->m_dir_p1.getX(), msg->m_dir_p1.getY());

            m_player_1->ChangeDir(c);
        }
        if (m_player_2 != nullptr){
            Coor h(msg->m_pos_p2.getX(), msg->m_pos_p2.getY());
            Coor c(msg->m_dir_p2.getX(), msg->m_dir_p2.getY());

            m_player_2->ChangeDir(c);
        }

		updateScores(msg->m_score_p1, msg->m_score_p2);
	}
}

void TronClient::checkState()
{
	// If a change in state is needed, it's done here
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
	//std::cout << "Estado cambiado: objetos" <<objs_.size() <<"\n";

	clearGameObjects();
	std::cout << objs_.size() <<"\n";

	switch (state)
	{
	case MessageServer::ServerState::WAITING:
	{ // Is waiting for an opponent to connect
		std::cout << "[Client]: Waiting...\n";
		loadBackground("./assets/images/MenuWaiting.png");
		break;
	}
	case MessageServer::ServerState::READY:
	{ // Opponent found: can start playing
		std::cout << "[Client]: Match found.\n";
		clearGameObjects();
		loadBackground("./assets/images/MenuPlay.png");
		break;
	}
	case MessageServer::ServerState::PLAYING:
	{	// Game starts
		std::cout << "[Client]: Playing...\n";
		loadBackground("./assets/images/GameWithBoard.png");
		// load game elements: players and score
		loadGame();
		break;
	}
	case MessageServer::ServerState::GAME_OVER:
	{
		std::cout << "[Client]: Game Over.\n";
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
		std::cout << "[Client]: Board is non-existent\n";
	}

	// Add both players with initial position and size --> will be updated by server later
	Coor p1_coor = GameManager::instance()->getTablero()->getPlayerOneInitialPosition();
	m_player_1 = new Player(p1_coor);
	m_player_1->setSize(GameManager::instance()->BOX_WIDTH, GameManager::instance()->BOX_WIDTH);
	m_player_1->setTexture("./assets/images/Player1.png");
	
	objs_.push_back(m_player_1);

	Coor p2_coor = GameManager::instance()->getTablero()->getPlayerTwoInitialPosition();
	m_player_2 = new Player(p2_coor);
	m_player_2->setSize(GameManager::instance()->BOX_WIDTH, GameManager::instance()->BOX_WIDTH);
	m_player_2->setTexture("./assets/images/Player2.png");
	
	objs_.push_back(m_player_2);

	// Create score items
	m_score_p1 = new TextObject(Window().renderer(), "./assets/fonts/Valorant Font.ttf", 24, "Score P1: 0", {255, 255, 255});
	m_score_p1->setPosition(250, 45);

	m_score_p2 = new TextObject(Window().renderer(), "./assets/fonts/Valorant Font.ttf", 24, "Score P2: 0", {255, 255, 255});	
	m_score_p2->setPosition(500, 45);
}

void TronClient::updateScores(int s1, int s2){

	if (m_score_p1 != nullptr && s1 != m_score_p1->getScore()){

		std::string newText1 = "Score:" + std::to_string(s1);
		m_score_p1->setScore(s1);

		std::cout << "\tSCORES\n";
		std::cout << "P1 -> " << m_score_p1->getScore();		
		std::cout << " P2 -> " << m_score_p2->getScore() <<"\n";

		// ERROR : updating text texture causes an unexpected error
		//m_score_p1->setText(newText1);
	}

	if (m_score_p2 != nullptr && s2 != m_score_p2->getScore()){
		std::string newText2 = "Score:" + std::to_string(s2);
		m_score_p2->setScore(s2);

		std::cout << "\tSCORES\n";
		std::cout << "P1 -> " << m_score_p1->getScore();		
		std::cout << " P2 -> " << m_score_p2->getScore() <<"\n";

		//m_score_p2->setText(newText2);
	}

}

void TronClient::sendGameMessage(MessageClient::InputType input)
{
	// Send an input message to the server
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
	printf("[Client]: Sending Match Message...\n");
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