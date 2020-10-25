#include "net.hpp"

// Write "count" bytes of "msg" to "fd"
// Return number of written bytes
uint16_t 
net::SendBytes(uint16_t fd, char *msg, int count) {
    int sent = send(fd, msg, count, 0);
    return sent;
}

// Write "count" bytes of "msg" to "fd"
// Return number of written bytes
uint16_t 
net::SendBytes(uint16_t fd, const char *msg, int count) {
    int sent = send(fd, msg, count, 0);
    return sent;
}

// Write "count" bytes from "fd" to "buffer"
// Return number of read bytes
int 
net::ReadBytes(uint16_t fd, char* buffer, unsigned int count)
{
    int bytesRead = read(fd, buffer, count);
    return bytesRead;
}