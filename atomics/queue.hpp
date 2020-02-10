/**
* Román Cárdenas Rodríguez
* ARSLab - Carleton University
* GreenLSI - Polytechnic University of Madrid
*
* Queue: Cadmium implementation of CD++ queue of clients
*/

#ifndef STORE_CASHIER_QUEUE_HPP
#define STORE_CASHIER_QUEUE_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <cassert>
#include <string>
#include <random>
#include <queue>
#include <boost/range/irange.hpp>

#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct queue_defs{
    struct outServedClient : public out_port<servedClient> {};
    struct inNewClient : public in_port<newClient> {};
    struct inAvailableEmployee: public in_port<availableEmployee> {};
};

template<typename T>
class Queue{
public:
    // ports definition
    using input_ports=tuple<typename queue_defs::inNewClient, typename queue_defs::inAvailableEmployee>;
    using output_ports=tuple<typename queue_defs::outServedClient>;

    // state definition
    struct state_type{
        queue<newClient> waitingClients;
        queue<availableEmployee> availableEmployees;
        vector<servedClient> servedClients;
    };
    state_type state;

    // default constructor
    Queue() {
        state.waitingClients = {};
        state.availableEmployees = {};
        state.servedClients = {};
    }

    // internal transition
    void internal_transition() {
        state.servedClients = {};  // Just remove previous pairings
    }

    // external transition
    void external_transition(T e, typename make_message_bags<input_ports>::type mbs) {
        // First step: register all the new available employees
        vector<availableEmployee> bagPortEmployees;
        bagPortEmployees = get_messages<typename queue_defs::inAvailableEmployee>(mbs);
        for (auto msg: bagPortEmployees) {
            // If there are clients waiting, pair new employee with the first of them
            if (!state.waitingClients.empty()) {
                newClient client = state.waitingClients.front();
                state.waitingClients.pop();
                state.servedClients.push_back(servedClient(client.clientId, msg.employeeId));
            // Otherwise, add new employee to the list of available employees
            } else {
                state.availableEmployees.push(msg);
            }
        }
        // Second step: deal with new clients
        vector<newClient> bagPortClients;
        bagPortClients = get_messages<typename queue_defs::inNewClient>(mbs);
        for (auto msg: bagPortClients) {
            // If there are available employees, pair new client with one of them
            if (!state.availableEmployees.empty()) {
                availableEmployee employee = state.availableEmployees.front();
                state.availableEmployees.pop();
                state.servedClients.push_back(servedClient(msg.clientId, employee.employeeId));
            // Otherwise, add new client to the end of the queue
            } else {
                state.waitingClients.push(msg);
            }
        }
    }

    // confluence transition
    void confluence_transition(T e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(T(), move(mbs));
    }

    // output function
    typename make_message_bags<output_ports>::type output() const {
        typename make_message_bags<output_ports>::type bags;
        vector<servedClient> bagPortOut;
        for (auto msg: state.servedClients) {
            bagPortOut.push_back(msg);
        }
        get_messages<typename queue_defs::outServedClient>(bags) = bagPortOut;
        return bags;
    }

    // time_advance function
    T time_advance() const {
        return (state.servedClients.empty()) ? numeric_limits<T>::infinity() : T();
    }

    friend ostringstream &operator << (ostringstream &os, const typename Queue<T>::state_type &i) {
        os << "< clients: " << i.waitingClients.size() << "& employees: " << i.availableEmployees.size() << " >";
        return os;
    }
};

#endif //STORE_CASHIER_QUEUE_HPP
