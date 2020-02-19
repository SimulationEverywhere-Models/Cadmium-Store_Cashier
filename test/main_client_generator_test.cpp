/**
* Román Cárdenas Rodríguez
* ARSLab - Carleton University
* GreenLSI - Polytechnic University of Madrid
*
* Client Generator Test: Test of the cadmium implementation of CD++ random client generator model
*/

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
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/client_generator.hpp"

//C++ libraries
//#include <iostream>
//#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/

/***** Define output ports for coupled model *****/
struct out_served: public out_port<newClient<TIME>>{};

int main(){
    /****** Subnet atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> generator;
    generator = dynamic::translate::make_dynamic_atomic_model<ClientGenerator, TIME>("generator");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};
    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(out_served)};
    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {generator};
    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
            dynamic::translate::make_EOC<clientGenerator_defs<TIME>::out,out_served>("generator")
    };
    dynamic::modeling::ICs ics_TOP = {};
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
            "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/client_generator_test_output_messages.txt");
    struct oss_sink_messages {
        static ostream &sink(){
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/client_generator_test_output_state.txt");
    struct oss_sink_state {
        static ostream &sink(){
            return out_state;
        }
    };

    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    /************** Runner call ************************/
    dynamic::engine::runner<TIME, logger_top> r(TOP, {0});
    r.run_until(TIME("04:00:00:000"));
    return 0;
}
