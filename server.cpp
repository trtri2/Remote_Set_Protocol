#include "server.hpp"

net::Server::Server(void* data)
{
    setup(data, DEFAULT_PORT);
}

net::Server::Server(void* data, int port)
{
    setup(data, port);
}

net::Server::Server(const Server& orig)
{
}

net::Server::~Server()
{
	std::cout << "[SERVER] [DESTRUCTOR] Destroying Server...\n";
	close(_listenFd);
}

// Create a socket and set up the address/port
// Don't modify _data
void 
net::Server::setup(void* data, int port)
{
    _data = data;
    _listenFd = socket(AF_INET, SOCK_STREAM, 0);
    _servAddr.sin_family = AF_INET;
    _servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    _servAddr.sin_port = htons(port);
}

// Configure the socket to use the SO_REUSEADDR option.
// When a server shutdowns and restarts quickly, it needs to wait for some seconds before it could bind to the same addr/port.
// This is common in TCP servers.
void 
net::Server::initializeSocket()
{
	std::cout << "[SERVER] initializing socket\n";

	int optValue = 1;
    int retTest = setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &optValue, sizeof(optValue));

	printf("[SERVER] setsockopt() ret %d\n", retTest);

	if (retTest < 0) {
        perror("[SERVER] [ERROR] setsockopt() failed");
		shutdown();
    }
}

// Bind the socket to address/port
void 
net::Server::bindSocket()
{
    if (bind(_listenFd, (struct sockaddr *) &_servAddr, sizeof(_servAddr)) < 0){
        perror("[SERVER] [ERROR] bind() failed");
        shutdown();
    }
}

// Listen to incoming connections
void 
net::Server::startListen()
{
    if (listen(_listenFd, LISTENQ_SIZE) < 0){
        perror("[SERVER] [ERROR] listen() failed");
        shutdown();
    }
}

// Close the listening socket
void 
net::Server::shutdown()
{
    close(_listenFd);
    exit(1);
}

// Accept incoming connections.
// You need to call "newConnectionCallback" once a connection is established.
void 
net::Server::handleNewConnection()
{
  	std::cout << "[SERVER] [CONNECTION] Waiting for a new connection\n";
    _connFd = accept(_listenFd, (struct sockaddr *) NULL, NULL);
    if (_connFd < 0){
        perror("[SERVER] [ERROR] accept() failed");
    }
    newConnectionCallback(this, _connFd, _data);
}

// Handle incoming connections.
void 
net::Server::loop()
{
    while (true){
        handleNewConnection();
        close(_connFd);
    }
}

void 
net::Server::init()
{
    initializeSocket();
    bindSocket();
    startListen();
}

void 
net::Server::onConnect(void(*ncc)(Server*, uint16_t, void*))
{
    newConnectionCallback = ncc;
}