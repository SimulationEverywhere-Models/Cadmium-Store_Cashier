#include "main.hpp"


/***** Define output ports for coupled model *****/
template <typename T>
struct out_served : public out_port<servedClient<T>>{};

int main(int argc, char ** argv) {
    // Default values for the scenario
    int n_employees = 3;         // There are three cashiers
    float mean_cashiers = 30;   // Each cashier takes 30 seconds for dispatching a new client
    float mean_clients = 10;      // The system generates 1 client every 10 seconds
    float stddev_employees = 0;  // There is no standard deviation to the dispatching rate
    float stddev_clients = 0;     // There is no standard deviation to the generation rate
    TIME sim_time = TIME("00:30:00:000");   // By default, we simulate 30 minutes


    if (argc > 7) {
        cout << "Program used with more arguments than accepted. Last arguments will be ignored." << endl;
    } else if (argc < 7) {
        cout << "Program used with less arguments than accepted. Missing parameters will be set to their default value." << endl;
    }
    if (argc != 7) {
        cout << "Correct usage:" << endl;
        cout << "\t" << argv[0] << " <SIMULATION_TIME> <N_CASHIERS> <MEAN_TIME_TO_DISPATCH_CLIENT> <MEAN_TIME_BETWEEN_NEW_CLIENTS> <DISPATCHING_STDDEV> <NEW_CLIENTS_STDDEV>"<<endl;
    }

    argc = (argc > 7)? 7: argc;
    stringstream ss;
    switch(argc) {
        case 7:
            sscanf(argv[6], "%f", &stddev_clients);
        case 6:
            sscanf(argv[5], "%f", &stddev_employees);
        case 5:
            sscanf(argv[4], "%f", &mean_clients);
        case 4:
            sscanf(argv[3], "%f", &mean_cashiers);
        case 3:
            sscanf(argv[2], "%d", &n_employees);
        case 2:
            sim_time = TIME(argv[1]);
        default:
            cout << "";
    }

    cout << endl << "CONFIGURATION OF THE SCENARIO:" << endl;
    cout << "\tSimulation time: " << sim_time << endl;
    cout << "\tNumber of Employees: " << n_employees << endl;
    cout << "\tMean time required by employee to dispatch clients: " << mean_cashiers << " seconds (standard deviation of " << stddev_employees << ")" << endl;
    cout << "\tMean time between new clients: " << mean_clients << " seconds (standard deviation of " << stddev_clients << ")" << endl << endl;

    auto start = hclock::now(); //to measure simulation execution time
    // Vector containing all the elements of the scenario
    vector<shared_ptr<dynamic::modeling::model>> submodels_store;
    /****** Client generator atomic model instantiation *******************/
    // Let's use the default constructor -> One client is created every 10 seconds
    submodels_store.push_back(dynamic::translate::make_dynamic_atomic_model<ClientGenerator, TIME>("client_generator"));

    /****** Queue atomic model instantiation *******************/
    submodels_store.push_back(dynamic::translate::make_dynamic_atomic_model<Queue, TIME>("queue"));

    /****** Employees atomic models instantiation *******************/
    for (int i = 0; i < n_employees; i++) {
        submodels_store.push_back(dynamic::translate::make_dynamic_atomic_model<Employee, TIME>("employee_" + to_string(i), i, mean_cashiers, stddev_employees));
        // submodels_store.push_back(dynamic::translate::make_dynamic_atomic_model<Employee, TIME, int, float, float>("employee_" + to_string(i), i, mean_cashiers, stddev_employees));
    }

    /*******STORE CASHIER SIMULATOR COUPLED MODEL********/
    dynamic::modeling::Ports iports_store = {};
    dynamic::modeling::Ports oports_store = {typeid(out_served<TIME>)};
    dynamic::modeling::EICs eics_store = {};
    dynamic::modeling::EOCs eocs_store;
    for (int i = 0; i < n_employees; i++) {
        eocs_store.push_back(dynamic::translate::make_EOC<employee_defs<TIME>::outClient, out_served<TIME>>("employee_" + to_string(i)));
    }
    dynamic::modeling::ICs ics_store = {
            dynamic::translate::make_IC<clientGenerator_defs<TIME>::out, queue_defs<TIME>::inNewClient>("client_generator", "queue"),
    };
    for (int i = 0; i < n_employees; i++) {
        ics_store.push_back(dynamic::translate::make_IC<queue_defs<TIME>::outPairedClient, employee_defs<TIME>::inClient>("queue", "employee_" + to_string(i)));
        ics_store.push_back(dynamic::translate::make_IC<employee_defs<TIME>::outAvailable, queue_defs<TIME>::inAvailableEmployee>("employee_" + to_string(i),"queue"));
    }
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

    r.run_until(sim_time);

    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(hclock::now() - start).count();
    cout << "Simulation took:" << elapsed << "sec" << endl;
    return 0;
}
