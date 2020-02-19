/**
* Román Cárdenas Rodríguez
* ARSLab - Carleton University
* GreenLSI - Polytechnic University of Madrid
*
*/

#ifndef STORE_CASHIER_MAIN_HPP
#define STORE_CASHIER_MAIN_HPP

//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Messages structures
#include "../data_structures/message.hpp"

//Atomic model headers
#include "../atomics/client_generator.hpp"
#include "../atomics/queue.hpp"
#include "../atomics/employee.hpp"


//C++ headers
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>

using namespace std;
using namespace cadmium;

using TIME = NDTime;
using hclock=chrono::high_resolution_clock;

#endif //STORE_CASHIER_MAIN_HPP
