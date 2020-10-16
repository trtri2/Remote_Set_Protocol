#include "request.hpp"
#include <typeinfo>
model::Request::Request(char opt)
{  
    setupFlags(opt);
    setup(opt, "", 0);
}

model::Request::Request(char opt, std::string uuid)
{  
    setupFlags(opt);
    setup(opt, uuid, 0);
}

model::Request::Request(char opt, std::string uuid, int item)
{  
    setupFlags(opt);
    setup(opt, uuid, item);
}

// Decode the byte array to Request members (opt, uuid and item)
model::Request::Request(const char* msg)
{
    char opt = msg[0];
    setupFlags(opt);
    if (this->hasUuid()){
        char uuid[KEY_SIZE+1];
        memcpy(uuid, msg+OPT_SIZE, KEY_SIZE);

        // null terminator for char*
        uuid[KEY_SIZE] = '\0';
        std::string str(uuid);
        if (this->hasItem()){
            char bytes[ITEM_SIZE];     
            memcpy(bytes, msg+OPT_SIZE+KEY_SIZE, ITEM_SIZE);
            int item = utils::bytesToInt(bytes);
            setup(opt, uuid, item);
        }else{
            setup(opt, uuid, 0);
        }

    }else{
        setup(opt, "", 0);
    }
    
    //DEBUG LINE
    // printf("Request: OPT Code - %x, UUID - %s, Items - %d\n", this->getOpt(), this->getUuid().c_str(), this->getItem());
}

void 
model::Request::setup(char opt, std::string uuid, int item)
{
    _opt = opt;
    _uuid = uuid;
    _item = item;
}

void 
model::Request::setupFlags(char opt)
{
    if(opt == OPT_CRT_SET || opt == OPT_GET_SETS) {
        _has_item = false;
        _has_uuid = false;
    } else {
        _has_item = false;
        _has_uuid = true;
        
        if (opt == OPT_ADD_ITEM || opt == OPT_REM_ITEM) {
            _has_item = true;
        }
    }
}

model::Request::Request(const Request& orig)
{
}

model::Request::~Request()
{
}

const char 
model::Request::getOpt() 
{
    return _opt;
}

std::string 
model::Request::getUuid() 
{
    return _uuid;
}

int 
model::Request::getItem() 
{
    return _item;
}

bool 
model::Request::hasUuid() 
{
    return _has_uuid;
}

bool 
model::Request::hasItem() 
{
    return _has_item;

}

// Return the number of bytes of the Request
int
model::Request::getBytesCount() 
{
    // Each request has a minimum of one for OP code.
    int bytesCount = OPT_SIZE;
    if (this->hasUuid()){
        bytesCount = bytesCount + KEY_SIZE;
        if (this->hasItem()){
            bytesCount = bytesCount + ITEM_SIZE;
        } 
    }
    return bytesCount;
}

// Encode the Request object to byte array (based on its content)
char* 
model::Request::toBytes() 
{
    int bytesCount = getBytesCount();
    char* msg = new char[bytesCount];
    msg[0] = this->getOpt();
    if (this->hasUuid()){
        //generate random Uuid for now
        memcpy(msg+OPT_SIZE, this->getUuid().c_str(), KEY_SIZE);
        if (this->hasItem()){
            memcpy(msg+OPT_SIZE+KEY_SIZE, utils::intToBytes(this->getItem()), ITEM_SIZE);
        }
    }
    return msg;    
}