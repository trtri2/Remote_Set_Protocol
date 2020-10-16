#include "client_handler.hpp"

void
net::printResponse(model::Response* res) {
    if(res->getRet() == RET_SUCCESS) {
        cout << "[CLIENT] SUCCESS" << endl;
        if(res->getOpt() == OPT_CRT_SET) {
            std::string uuid = res->getUuids()[0];
            cout << "[CLIENT] New set ID " << uuid << endl;
        } else if(res->getOpt() == OPT_GET_SETS) {
            cout << "[CLIENT] Existing Set IDs:" << endl;
            for(auto const& uuid : res->getUuids()) {
                cout << "[CLIENT] \t" << uuid << endl;
            }
        } else if(res->getOpt() == OPT_GET_ITEMS) {
            cout << "[CLIENT] Items:" << endl;
            for(auto const& item : res->getItems()) {
                cout << "[CLIENT] \t" << item << endl;
            }
        } else if(res->getOpt() == OPT_GET_SIZE) {
            cout << "[CLIENT] Set size is : " << res->getSetSize() << endl;
        }
    } else if (res->getRet() == RET_SET_NOT_EXIST) {
        cout << "[CLIENT] SET DOES NOT EXIST" << endl;
    } else if (res->getRet() == RET_ITEM_NOT_EXIST) {
        cout << "[CLIENT] ITEM DOES NOT EXIST" << endl;
    } else if (res->getRet() == RET_ITEM_ALREADY_EXIST) {
        cout << "[CLIENT] ITEM ALREADY EXISTS" << endl;
    } else {
        cout << "[CLIENT] Non-supported return type" << endl;
    }
}

// Read the response from the socket and construct the bytes based on response size.
// Return nullptr if there is an error.
char* 
net::readResponseBytes(uint16_t fd) 
{
    // read opt code and body length size first
    char *responseBytes = new char[OPT_SIZE+LEN_SIZE];
    //maybe add error handling to return nullptr
    int numBytes = (net::ReadBytes(fd, responseBytes, (OPT_SIZE+LEN_SIZE)));
    char opt = responseBytes[0];
    if (opt != RET_SUCCESS){
        return responseBytes;
    } else{
    // read the body_length to determine the buffer size for the rest of the response body
        char *body = new char[LEN_SIZE];
        memcpy(body, responseBytes+OPT_SIZE, LEN_SIZE);
        int bodyLen = utils::bytesToInt(body);
        char *newResponseBytes = new char[(OPT_SIZE+LEN_SIZE+bodyLen)];
        memcpy(newResponseBytes, responseBytes, (OPT_SIZE+LEN_SIZE));

        // Handle memory 
        delete[] body;
        delete[] responseBytes;

        //maybe add error handling to return nullptr
        numBytes = numBytes + (net::ReadBytes(fd, newResponseBytes+(OPT_SIZE+LEN_SIZE), bodyLen));

        return newResponseBytes;
    }
    return nullptr;
}

// Send the request to the server and wait for a response.
// Once the response arrives, read the response bytes and pass them to a Response object.
void 
net::OnClientConnected(net::Client* client, uint16_t fd, void* data) 
{
    cout << "[CLIENT] Connected" << endl;
    
    model::Request* req = (model::Request*) data;
    // DEBUG LINE
    //printf("Request: OPT Code - %c, UUID - %s, Items - %s\n", req->getOpt(), req->getUuid().c_str(), utils::intToBytes(req->getItem()));
    uint16_t sentBytes = net::SendBytes(fd, req->toBytes(), req->getBytesCount());
    
    cout << "[CLIENT] " << sentBytes << " bytes were sent" << endl;
    
    char* responseBytes = readResponseBytes(fd);
    model::Response* res = new model::Response(req->getOpt(), responseBytes);
    printResponse(res);
}