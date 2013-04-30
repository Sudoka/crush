#include "Server.h"


Server::Server(unsigned int port):m_server(port)
{
	m_start = false;
	m_reload = false;
	m_hThread = NULL;
	string error_file = SERVER_ERROR_FILE;
	ConfigSettings::config->getValue("server_errorLogFile", error_file);
	m_f_error = ofstream(error_file);
	if(!m_f_error) {
		cerr << "error opening log file: " << error_file << endl;
	}
}


void Server::start() {
	m_start = true;
	if(m_hThread == NULL) {
		unsigned int threadID;
		m_hThread = (HANDLE)_beginthreadex( NULL, // security
		0,             // stack size
		Server::StaticLoopEntry,// entry-point-function
		this,           // arg list holding the "this" pointer
		0,		
		&threadID );
	}
}

void Server::restart() {
	m_start = true;
}

void Server::reload() {
	m_reload = true;
}

void Server::reloadConfig() {
	ConfigSettings::config->reloadSettingsFile();

	//add variables to update here
}

void Server::setUpResourceMine() {
	D3DXVECTOR3 m_pos1(0,0,0);
	Quaternion m_dir1(0.0, 0.0, 0.0, 1.0);
	m_resourceMine = new Mine();
	S_Resource *res = m_resourceMine->respawn();
	m_gameState.push_back(res);
	m_world.entities.push_back(res);

	m_resourceMine->setStart(milliseconds_now());
}

void Server::setUpAsteroids() {



	Quaternion m_dir3(0, 0, 0, 1);


	D3DXVECTOR3 m_pos4(0,0,-15);
	S_Asteroid *test5 = new S_Asteroid(m_pos4, m_dir3);
	m_gameState.push_back(test5);
	m_world.entities.push_back(test5);

	D3DXVECTOR3 m_pos5(0,0,15);
	S_Asteroid *test6 = new S_Asteroid(m_pos5, m_dir3);
	m_gameState.push_back(test6);
	m_world.entities.push_back(test6);

}

void Server::setUpBoundaries() {
		
	float bound = 200;
	Boundary left = Boundary(D3DXVECTOR3(1.0f,0.0f,0.0f), D3DXVECTOR3(-bound,0.0f,0.0f));
	Boundary right = Boundary(D3DXVECTOR3(-1.0f,0.0f,0.0f), D3DXVECTOR3(bound,0.0f,0.0f));
	Boundary top = Boundary(D3DXVECTOR3(0.0f,-1.0f,0.0f), D3DXVECTOR3(0.0f,bound,0.0f));
	Boundary down = Boundary(D3DXVECTOR3(0.0f,1.0f,0.0f), D3DXVECTOR3(0.0f,-bound,0.0f));
	Boundary front = Boundary(D3DXVECTOR3(0.0f,0.0f,1.0f), D3DXVECTOR3(0.0f,0.0f,-bound));
	Boundary back = Boundary(D3DXVECTOR3(0.0f,0.0f,-1.0f), D3DXVECTOR3(0.0f,0.0f,bound));
		
	m_world.boundaries.push_back(left);
	m_world.boundaries.push_back(right);
	m_world.boundaries.push_back(top);
	m_world.boundaries.push_back(down);
	m_world.boundaries.push_back(front);
	m_world.boundaries.push_back(back);
		
}

void Server::initializeGameState() {
	m_gameState.clear();
	m_playerMap.clear();
	m_world.entities.clear();

	setUpResourceMine();


	setUpAsteroids();

	setUpBoundaries();

}


void Server::removeDisconClients() {
	vector<unsigned int> disconClients = m_server.getDisconClients();
	for (auto it = disconClients.begin(); it != disconClients.end(); it++){
		auto player = m_playerMap.find(*it);
		if(player != m_playerMap.end()) {
			player->second->m_playerName = "";
			//would normally remove here, but no good way of removing yet
			//may not even implement this
		}
	}
}


void Server::loop() {
	for(;;) {
		if(m_start) {			
			initializeGameState();
			reloadConfig();
			cout << "CRUSH Server has started" << endl;
			m_start = false;
		} else if (m_reload) {
			reloadConfig();
			cout << "Config file reloaded" << endl;
			m_reload = false;
		}		
		startTick();

		//(optional) currently just removes the name from a player's
		//ship who has disconnected
		removeDisconClients();

		addNewClients();

		m_clientInput = m_server.getEvents();
		if(!m_clientInput.empty()) {
			moveClients();
		}

		if(m_resourceMine->checkRespawn(milliseconds_now())) {
			S_Resource * res = m_resourceMine->respawn();
			m_gameState.push_back(res);
			m_world.entities.push_back(res);
		}
		m_world.update(1.0f/60.0f);
		m_server.broadcastGameState(m_gameState);
	
		endOfTick();
	}
}

void Server::addNewClients() {
	vector<pair<unsigned int, string>> cc = m_server.getNewClientIDs();
	//get new client ids and if they aren't in the playerMap add them
	for(auto it= cc.begin(); it != cc.end(); it++) {
		auto player = m_playerMap.find(it->first);
		if(player == m_playerMap.end()) {
			spawnShip(it->first);
			spawnMothership(it->first);
		}
	}
}

void Server::spawnShip(unsigned int client_id) {
	// Temp
	/*Quaternion m_dir3(0, 0, 0, 1);
	D3DXVECTOR3 m_pos7((FLOAT)-10*(client_id+1),2,12);
	S_TractorBeam *test8 = new S_TractorBeam(m_pos7, m_dir3, 0);
	m_gameState.push_back(test8);
	m_world.entities.push_back(test8);*/

	D3DXVECTOR3 m_pos((FLOAT)-30*(client_id+1),2,2);
	Quaternion m_dir(0, 0, 0, 1);
	S_Ship *tmp = new S_Ship(m_pos, m_dir, client_id);
	m_playerMap.insert(pair<unsigned int, S_Ship*>(client_id,tmp));
	m_gameState.push_back(tmp);
	m_world.entities.push_back(tmp);
}

void Server::spawnMothership(unsigned int client_id) {
	D3DXVECTOR3 m_pos((FLOAT)-50*(client_id+1),2,-8.0);
	Quaternion m_dir(0, 0, 0, 1);
	S_Mothership *tmp = new S_Mothership(m_pos, m_dir, client_id);
	m_mothershipMap.insert(pair<unsigned int, S_Mothership*>(client_id,tmp));
	m_gameState.push_back(tmp);
	m_world.entities.push_back(tmp);
}


void Server::moveClients() {
	//add client inputs to every player that we have input for
	for(auto it = m_clientInput.begin(); it != m_clientInput.end(); it++) {
		auto player = m_playerMap.find(it->first);
		if(player != m_playerMap.end()) {
			player->second->addPlayerInput(*(it->second));
		}
	}
}

long long Server::milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	} else {
		return GetTickCount();
	}
}

inline void Server::startTick() {
	m_startTick = milliseconds_now();
}

inline void Server::endOfTick() {
	double sleeptime = (double)1000/60 - (milliseconds_now() - m_startTick);
	if(sleeptime <= 0) {
		m_f_error << "Clock tick violated! " << sleeptime << endl;
	} else {
		Sleep((DWORD)sleeptime);
	}
}


Server::~Server(void)
{
	TerminateThread(m_hThread, 0);
	CloseHandle(m_hThread);
}



