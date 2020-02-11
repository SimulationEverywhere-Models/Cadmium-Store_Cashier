#include "message.hpp"

/***************************************************/
/***************** Output streams ******************/
/***************************************************/
template <typename  T>
ostream &operator << (ostream &os, const newClient<T> &msg) {
  os << msg.clientId << " " << msg.arrived;
  return os;
}

template <typename  T>
ostream &operator << (ostream &os, const availableEmployee<T> &msg) {
    os << msg.employeeId << " " << msg.arrived;
    return os;
}

template  <typename T>
ostream &operator << (ostream &os, const clientPairing<T> &msg) {
    os << msg.client << " " << msg.employeeId;
    return os;
}

template <typename T>
ostream &operator << (ostream &os, const servedClient<T> &msg) {
    os << msg.clientId << " " << msg.employeeId << " " << msg.arrived << " " << msg.dispatched << " " << msg.delay;
    return os;
}
