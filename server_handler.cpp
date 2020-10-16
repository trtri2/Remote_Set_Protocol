#include "server_handler.hpp"

// Read a request from a client.
// Once the request arrives, process the request, and reply back to the client with the response.
void 
net::OnNewClientConnected(net::Server* server, uint16_t fd, void* data) 
{
    model::SetManager* sm = (model::SetManager*) data;

    cout << "[SERVER] New Connection..." << endl;
    char* requestBytes = new char[MAX_BUFFER_SIZE];

    int readBytes = net::ReadBytes(fd, requestBytes, REQUEST_SIZE);
    cout << "[SERVER] " << readBytes << " bytes were received" << endl;

    model::Request* req = new model::Request(requestBytes);
    model::Response* res = sm->HandleRequest(req);

    uint16_t sentBytes = net::SendBytes(fd, res->toBytes(), res->getBytesCount());
    
    cout << "[SERVER] " << sentBytes << " bytes were sent" << endl;
}