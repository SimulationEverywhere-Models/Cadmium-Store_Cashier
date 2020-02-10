#ifndef STORE_CASHIER_MESSAGE_HPP
#define STORE_CASHIER_MESSAGE_HPP

#include <cassert>
#include <iostream>
#include <string>

using namespace std;

/*******************************************/
/**************** Client *******************/
/*******************************************/
struct newClient {
    int clientId;
    explicit newClient(int id) : clientId(id) {}
};

struct availableEmployee {
    int employeeId;
    explicit availableEmployee(int id) : employeeId(id) {}
};

struct servedClient {
    int clientId;
    int employeeId;
    explicit servedClient(int client, int employee) : clientId(client), employeeId(employee) {}
};

ostream &operator << (ostream &os, const  newClient &msg);
ostream &operator << (ostream &os, const  availableEmployee &msg);
ostream &operator << (ostream &os, const  servedClient &msg);

#endif //STORE_CASHIER_MESSAGE_HPP
