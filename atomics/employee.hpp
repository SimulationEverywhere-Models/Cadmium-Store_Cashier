/**
* Román Cárdenas Rodríguez
* ARSLab - Carleton University
* GreenLSI - Polytechnic University of Madrid
*
* Employee: Cadmium implementation of CD++ bank employee model
*/

#ifndef STORE_CASHIER_EMPLOYEE_HPP
#define STORE_CASHIER_EMPLOYEE_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <cassert>
#include <string>
#include <random>

#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct employee_defs{
    struct inClient: public in_port<servedClient> {};
    struct outClient : public out_port<servedClient> {};
    struct outAvailable: public out_port<availableEmployee> {};
};

template<typename T>
class Employee {
public:
    // ports definition
    using input_ports = tuple<typename employee_defs::inClient>;
    using output_ports = tuple<typename employee_defs::outClient, typename employee_defs::outAvailable>;

    double mean;
    double stddev;
    default_random_engine generator;

    // state definition
    struct state_type {
        int employeeId;
        bool busy;
        int client;
        int nClientsServed;
        T nextTimeout;
    };
    state_type state;
    // default constructor does not work; you must at least provide an ID to the employee
    explicit  Employee(){
        assert(false && "you must at least give an ID to the employee");
    }
    // use this constructor to create a default employee. It will take 30 seconds to dispatch a single client
    explicit Employee(int id) {
        new (this) Employee(id, 30, 0);
    }
    // use this constructor to create an employee with a specific client dispatching rate
    explicit Employee(int id, double mean_time) {
        new (this) Employee(id, mean_time, 0);
    }
    // use this constructor to create an employee with a mean client dispatching rate as well as its standard deviation
    explicit Employee(int id, double meanTime, double stddevTime) {
        mean = meanTime;
        stddev = stddevTime;
        state.busy = false;
        state.employeeId = id;
        state.nClientsServed = 0;
        state.nextTimeout = T();
    }

    // internal transition
    void internal_transition() {
        state.busy = false;
        state.nextTimeout = numeric_limits<T>::infinity();
    }

    // external transition
    void external_transition(T e, typename make_message_bags<input_ports>::type mbs) {
        // first, if employee is busy, substract elapsed time to the foreseen timeout
        if (state.busy) {
            state.nextTimeout -= e;
        }
        // Then, check incoming clients
        vector<servedClient> bagPortIn;
        bagPortIn = get_messages<typename employee_defs::inClient>(mbs);
        for (auto msg: bagPortIn) {
            // Ignore any message which employee ID doesn't coincide with the employee
            if (msg.employeeId == state.employeeId) {
                // If employee receives a message while being busy, raise assertion error
                if (state.busy) {
                    assert(false && "One client at a time");
                }
                state.busy = true;
                state.nClientsServed ++;
                state.client = msg.clientId;
                state.nextTimeout = nextTimeout();
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
        // inform to the queue that employee is available
        vector<availableEmployee> bagPortAvailabilityOut;
        availableEmployee msg = availableEmployee(state.employeeId);
        bagPortAvailabilityOut.push_back(msg);
        get_messages<typename employee_defs::outAvailable>(bags) = bagPortAvailabilityOut;
        // If employee was attending a client, send message with served client
        if (state.busy) {
            vector<servedClient> bagPortClientOut;
            bagPortClientOut.push_back(servedClient(state.client, state.employeeId));
            get_messages<typename employee_defs::outClient>(bags) = bagPortClientOut;
        }
        return bags;
    }

    // time_advance function
    T time_advance() const {
        return state.nextTimeout;
    }

    T nextTimeout() {
        normal_distribution<double> distribution(mean, stddev);
        int rawTime = -1;
        while ((rawTime < 0) || (rawTime > mean + stddev * 5)) {
            rawTime = (int) round(distribution(generator));
        }
        int seconds = rawTime % 60;
        rawTime /= 60;
        int minutes = rawTime % 60;
        int hours = rawTime / 60;

        return T({hours, minutes, seconds});
    }

    friend ostringstream &operator << (ostringstream &os, const typename Employee<T>::state_type &i) {
        os << "< busy: " << i.busy << "& clients so far: " << i.nClientsServed << " >";
        return os;
    }
};

#endif //STORE_CASHIER_EMPLOYEE_HPP
