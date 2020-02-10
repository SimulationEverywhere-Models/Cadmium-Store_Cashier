/**
* Román Cárdenas Rodríguez
* ARSLab - Carleton University
* GreenLSI - Polytechnic University of Madrid
*
* Client Generator: Cadmium implementation of CD++ random client generator model
*/

#ifndef STORE_CASHIER_CLIENT_GENERATOR_HPP
#define STORE_CASHIER_CLIENT_GENERATOR_HPP

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
struct clientGenerator_defs{
    struct out : public out_port<newClient> {};
};

template<typename T>
class ClientGenerator{
public:
    // ports definition
    using input_ports = tuple<>;
    using output_ports = tuple<typename clientGenerator_defs::out>;

    double mean;
    double stddev;
    default_random_engine generator;

    // state definition
    struct state_type{
        int clientIndex;
        T nextTimeout;
        state_type(): clientIndex(0), nextTimeout({}) {};
    };
    state_type state;

    // default constructor creates one client every 10 seconds
    ClientGenerator() {
        new (this) ClientGenerator(10, 0);
    }
    // use this constructor to set the client generation rate
    explicit ClientGenerator(double mean_time) {
        new (this) ClientGenerator(mean_time, 0);
    }
    // use this constructor to set client generation mean rate as well as its standard deviation.
    explicit ClientGenerator(double mean_time, double stddev_time) {
        mean = mean_time;
        stddev = stddev_time;
        state = state_type();
    }

    // internal transition
    void internal_transition() {
        state.clientIndex++;
        state.nextTimeout = nextTimeout();
    }

    // external transition
    void external_transition(T e, typename make_message_bags<input_ports>::type mbs) {}

    // confluence transition
    void confluence_transition(T e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(T(), move(mbs));
    }

    // output function
    typename make_message_bags<output_ports>::type output() const {
        typename make_message_bags<output_ports>::type bags;
        vector<newClient> bagPortOut;
        newClient msg = newClient(state.clientIndex);
        bagPortOut.push_back(msg);
        get_messages<typename clientGenerator_defs::out>(bags) = bagPortOut;
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

    friend ostringstream &operator << (ostringstream &os, const typename ClientGenerator<T>::state_type &i) {
        os << "< new client: " << i.clientIndex << "& in: " << i.nextTimeout << " >";
        return os;
    }
};

#endif //STORE_CASHIER_CLIENT_GENERATOR_HPP
