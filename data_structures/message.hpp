#ifndef STORE_CASHIER_MESSAGE_HPP
#define STORE_CASHIER_MESSAGE_HPP

#include <cassert>
#include <iostream>
#include <string>

using namespace std;

/*******************************************/
/************** Messages *******************/
/*******************************************/

template <typename T>
struct newClient {
    int clientId;
    T arrived;
    newClient(): clientId(-1), arrived(T()) {};
    explicit newClient(int id, T when) : clientId(id), arrived(when) {}
};

template <typename T>
struct availableEmployee {
    int employeeId;
    T arrived;
    explicit availableEmployee(int id, T when) : employeeId(id), arrived(when) {}
};

template<typename T>
struct clientPairing {
    newClient<T> client;
    int employeeId;
    explicit clientPairing(newClient<T> client_, int employee): client(client_), employeeId(employee) {}
};

template <typename T>
struct servedClient {
    int clientId, employeeId;
    T arrived, dispatched, delay;
    explicit servedClient(int client, int employee, T from, T to) : clientId(client), employeeId(employee),
                                                                    arrived(from), dispatched(to), delay(to - from) {}
};

/***************************************************/
/***************** Output streams ******************/
/***************************************************/

template <typename T>
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

#endif //STORE_CASHIER_MESSAGE_HPP
