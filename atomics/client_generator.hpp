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
template <typename T>
struct clientGenerator_defs{
    struct out : public out_port<newClient<T>> {};
};

template<typename T>
class ClientGenerator{
public:
    // ports definition
    using input_ports = tuple<>;
    using output_ports = tuple<typename clientGenerator_defs<T>::out>;

    float mean;
    float stddev;
    default_random_engine generator;

    // state definition
    struct state_type{
        int clientIndex;
        T nextTimeout;
        T clock;
        state_type(): clientIndex(0), nextTimeout(T()), clock(T()) {};
    };
    state_type state;

    // default constructor creates one client every 10 seconds
    ClientGenerator() {
        new (this) ClientGenerator(10, 0);
    }
    // use this constructor to set the client generation rate
    explicit ClientGenerator(float meanTime) {
        new (this) ClientGenerator(meanTime, 0);
    }
    // use this constructor to set client generation mean rate as well as its standard deviation.
    explicit ClientGenerator(float meanTime, float stddevTime) {
        mean = meanTime;
        stddev = stddevTime;
        state = state_type();
    }

    // internal transition
    void internal_transition() {
        state.clientIndex++;
        state.clock += state.nextTimeout;
        state.nextTimeout = nextTimeout(mean, stddev);
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
        vector<newClient<T>> bagPortOut;
        newClient msg = newClient(state.clientIndex, state.clock + state.nextTimeout);
        bagPortOut.push_back(msg);
        get_messages<typename clientGenerator_defs<T>::out>(bags) = bagPortOut;
        return bags;
    }

    // time_advance function
    T time_advance() const {
        return state.nextTimeout;
    }

    T nextTimeout(float meanIn, float stddevIn) {
        normal_distribution<double> distribution(meanIn, stddevIn);
        int rawTime = -1;
        while ((rawTime < 0) || ((float)rawTime > meanIn + stddevIn * 5)) {
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
