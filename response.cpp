#include "response.hpp"

model::Response::Response(char opt, char ret)
:_opt(opt), _ret(ret)
{  

}

// Decode the byte array to Response members
model::Response::Response(char opt, const char* msg)
:_opt(opt)
{
    _opt = opt;
    _ret = msg[0];
    char *bodyLenBuffer = new char[LEN_SIZE];
    memcpy(bodyLenBuffer, msg+RET_SIZE, LEN_SIZE);
    int bodyLen = utils::bytesToInt(bodyLenBuffer);
    if (_ret == RET_SUCCESS && bodyLen > 0){
        // depending on opt code.. the msg will have different params
        int offset = RET_SIZE+LEN_SIZE;
        if(_opt == OPT_CRT_SET){
            char setID[bodyLen+1];
            memcpy(setID, msg+offset, KEY_SIZE);
            setID[KEY_SIZE] = '\0';
            std::string str(setID);
            this->addUuid(setID);
        } else if (_opt == OPT_GET_ITEMS){
            int numItems = bodyLen / ITEM_SIZE;
            char *itemBuffer = new char[ITEM_SIZE];
            for(int i = 0; i < numItems; i++){
                memcpy(itemBuffer, msg+offset, ITEM_SIZE);
                offset+=ITEM_SIZE;
                this->addItem(utils::bytesToInt(itemBuffer));
            }
            delete[] itemBuffer;
        } else if (_opt == OPT_GET_SIZE){
            char *sizeBuffer = new char[SET_LEN_SIZE];
            memcpy(sizeBuffer, msg+offset, SET_LEN_SIZE);
            _set_size = utils::bytesToInt(sizeBuffer);
            delete[] sizeBuffer;
        } else if (_opt == OPT_GET_SETS){
            int numItems = bodyLen / KEY_SIZE;
            char *setBuffer = new char[KEY_SIZE];
            for (int i = 0; i < numItems; i++){
                memcpy(setBuffer, msg+offset, KEY_SIZE);
                offset+=KEY_SIZE;
                this->addUuid(std::string(setBuffer));
            }
            delete[] setBuffer;
        }
    }
    // Handle memory
    delete[] bodyLenBuffer;

}

model::Response::Response(const Response& orig)
{

}

model::Response::~Response()
{

}

// Return the number of bytes of the Response
int
model::Response::getBytesCount() 
{
    int bytesCount = RET_SIZE + LEN_SIZE + (this->getLen()*LEN_WORD_SIZE);
    return bytesCount;
}

// Encode the Response object to byte array (based on its content)
char* 
model::Response::toBytes() 
{
    int bytesCount = getBytesCount();
    char* msg = new char[bytesCount];
    // Return Code (1 byte)
    msg[0] = this->getRet();
    // Body Length (4 bytes)
    memcpy(msg+OPT_SIZE, utils::intToBytes(this->getLen()*LEN_WORD_SIZE), LEN_SIZE);
    if (this->getRet() != RET_SUCCESS){
        return msg;
    }
    int offset = OPT_SIZE + LEN_SIZE;
    if (this->getOpt() == OPT_CRT_SET){
        //get the last element of vector uuid, since that is most recently pushed back.
        std::string setID = this->getUuids().back();
        memcpy(msg+offset, setID.c_str(), KEY_SIZE);
    }else if (this->getOpt() == OPT_GET_ITEMS){
        for(auto it = _items.begin(); it != _items.end(); ++it){
            memcpy(msg+offset, utils::intToBytes(*it), ITEM_SIZE);
            offset+=ITEM_SIZE;
        }
    } else if (this->getOpt() == OPT_GET_SIZE){
        memcpy(msg+offset, utils::intToBytes(this->getSetSize()), SET_LEN_SIZE);
    } else if (this->getOpt() == OPT_GET_SETS){
        for(auto it = _uuids.begin(); it != _uuids.end(); ++it){
            memcpy(msg+offset, it->c_str(), KEY_SIZE);
            offset+=KEY_SIZE;
        }
    }
    return msg;    
}

const char 
model::Response::getOpt() 
{
    return _opt;
}

const char 
model::Response::getRet() 
{
    return _ret;
}

// Return the number of 4-byte words of the Response body
int 
model::Response::getLen()
{
    if(_ret != RET_SUCCESS) {
        return 0;
    }
    int bytesCount = 0;
    int wordCount = 0;
    if (this->getOpt() == OPT_CRT_SET){
        // return one set id (32 bytes)
        bytesCount = KEY_SIZE;
    }else if (this->getOpt() == OPT_GET_ITEMS){
        // return # items in the queried set (4 byte per item)
        bytesCount = (ITEM_SIZE * this->getItems().size());
    } else if (this->getOpt() == OPT_GET_SIZE){
        // return the set size of the queried set (4 byte ID that represents the number of items in the set)
        // assume size field is a 4byte field
        bytesCount = SET_LEN_SIZE;
    } else if (this->getOpt() == OPT_GET_SETS){
        // return # sets (32 byte per set ID) 
        // get count of getUuids vector
        bytesCount = (KEY_SIZE * this->getUuids().size());
    }
    wordCount = bytesCount / INT_SIZE;
    return wordCount;
}

void
model::Response::addItem(int item) 
{
    _items.push_back(item);
}

void 
model::Response::addUuid(std::string uuid)
{
    _uuids.push_back(uuid);
}

void 
model::Response::setSetSize(int size)
{
    _set_size = size;
}

const std::vector<int>
model::Response::getItems() 
{
    return _items;
}

const std::vector<std::string>
model::Response::getUuids() 
{
    return _uuids;
}

int 
model::Response::getSetSize()
{
    return _set_size;
}
