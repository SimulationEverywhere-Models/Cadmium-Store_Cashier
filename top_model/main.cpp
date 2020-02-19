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
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs

//C++ headers
#include <algorithm>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;
using hclock=chrono::high_resolution_clock;

/***** Define input port for coupled models *****/

/***** Define output ports for coupled model *****/
template <typename T>
struct out_served : public out_port<servedClient<T>>{};

int main(int argc, char ** argv) {
    auto start = hclock::now(); //to measure simulation execution time
    /****** Client generator atomic model instantiation *******************/
    // Let's use the default constructor -> One client is created every 10 seconds
    shared_ptr<dynamic::modeling::model> generator = dynamic::translate::make_dynamic_atomic_model<ClientGenerator, TIME>("client_generator");

    /****** Queue atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> queue = dynamic::translate::make_dynamic_atomic_model<Queue, TIME>("queue");

    /****** Subnet atomic models instantiation *******************/
    // The first employee is very efficient: he/she needs only 20 seconds to dispatch a client
    shared_ptr<dynamic::modeling::model> employee_1 = dynamic::translate::make_dynamic_atomic_model<Employee, TIME, int, double>("employee_1", 1, 20);
    // However, the second employee is new: he/she needs a mean of 30 seconds per client.
    // Moreover, he/she doesn't know well the procedure, and sometimes it takes shorter/longer time.
    // Let's configure a standard deviation of 4 seconds
    shared_ptr<dynamic::modeling::model> employee_2 = dynamic::translate::make_dynamic_atomic_model<Employee, TIME,int, double, double>("employee_2", 2, 30, 4);

    /*******ABP SIMULATOR COUPLED MODEL********/
    dynamic::modeling::Ports iports_store = {};
    dynamic::modeling::Ports oports_store = {typeid(out_served<TIME>)};
    dynamic::modeling::Models submodels_store = {generator, queue, employee_1, employee_2};
    dynamic::modeling::EICs eics_store = {};
    dynamic::modeling::EOCs eocs_store = {
        dynamic::translate::make_EOC<employee_defs<TIME>::outClient, out_served<TIME>>("employee_1"),
        dynamic::translate::make_EOC<employee_defs<TIME>::outClient, out_served<TIME>>("employee_2"),
    };
    dynamic::modeling::ICs ics_store = {
        dynamic::translate::make_IC<clientGenerator_defs<TIME>::out, queue_defs<TIME>::inNewClient>("client_generator", "queue"),
        dynamic::translate::make_IC<queue_defs<TIME>::outPairedClient, employee_defs<TIME>::inClient>("queue", "employee_1"),
        dynamic::translate::make_IC<employee_defs<TIME>::outAvailable, queue_defs<TIME>::inAvailableEmployee>("employee_1","queue"),
        dynamic::translate::make_IC<queue_defs<TIME>::outPairedClient, employee_defs<TIME>::inClient>("queue", "employee_2"),
        dynamic::translate::make_IC<employee_defs<TIME>::outAvailable, queue_defs<TIME>::inAvailableEmployee>("employee_2","queue"),
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> store;
    store = make_shared<dynamic::modeling::coupled<TIME>>(
            "store", submodels_store, iports_store, oports_store, eics_store, eocs_store, ics_store);

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/store_cashier_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/store_cashier_output_state.txt");
    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };
    
    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    auto elapsed1 = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Model Created. Elapsed time: " << elapsed1 << "sec" << endl;

    /************** Runner call ************************/ 
    dynamic::engine::runner<NDTime, logger_top> r(store, {0});

    elapsed1 = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Runner Created. Elapsed time: " << elapsed1 << "sec" << endl;

    r.run_until(TIME("00:02:00:000"));

    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Simulation took:" << elapsed << "sec" << endl;
    return 0;
}