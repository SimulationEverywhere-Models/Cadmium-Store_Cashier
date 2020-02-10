#include <string>

#include "message.hpp"

/***************************************************/
/***************** Output streams ******************/
/***************************************************/
ostream &operator << (ostream &os, const newClient &msg) {
  os << msg.clientId;
  return os;
}

ostream &operator << (ostream &os, const availableEmployee &msg) {
    os << msg.employeeId;
    return os;
}

ostream &operator << (ostream &os, const servedClient &msg) {
    os << msg.clientId << " " << msg.employeeId;
    return os;
}
