/*
 * Módulo do Nó Sensor
 * 
 * @author Ricardo Diniz Caldas
 * @version v1.0
 */

#include "SensorNodeModule.h"

SensorNodeModule::SensorNodeModule(const int32_t &argc, char **argv) :
    TimeTriggeredConferenceClientModule(argc, argv, "sensornode"),
    m_id(getIdentifier()),
    m_sensor_type(),
    m_status("baixo"),
    m_data_queue(),
    m_status_log(),
    m_ref() {}

SensorNodeModule::~SensorNodeModule() {}

// CONFIGURAÇÃO
void SensorNodeModule::setUp() {
    m_sensor_type = m_id+1; // configuração do tipo de sensor

    clock_gettime(CLOCK_REALTIME, &m_ref); // referência para medidas de tempo  

    string path = "output/";
    string filename = "sensornode" + to_string(m_id);
    m_status_log.open(path + filename + "_status_log.csv");
    m_status_log << "Elapsed Time(s), Sensor Status, Time Since Last (s)\n";
}

// DESTRUIÇÃO
void SensorNodeModule::tearDown() {
}

bool SensorNodeModule::controllerFSM(int t){
    bool exe = false;
    
    if(m_status=="baixo"){
        exe = (t>=10)?true:false;
    } else if (m_status=="moderado") {
        exe = (t>=5)?true:false;
    } else if (m_status=="alto") {
        exe = (t>=1)?true:false;
    } 

    return exe;
}


string SensorNodeModule::generateData(string actual_status){

    string category;
    int p = (rand() % 100) + 1;

    if(actual_status == "baixo"){
        if(1 <= p && p <= 5) {
            category = "alto";
        } else if (5 < p && p <= 30) {
            category = "moderado";
        } else {
            category = actual_status;
        }
    } else if(actual_status == "moderado") {
        if(1 <= p && p <= 15) {
            category = "alto";
        } else if (15 < p && p <= 30) {
            category = "baixo";
        } else {
            category = actual_status;
        }
    } else if(actual_status == "alto"){
        if(1 <= p && p <= 5) {
            category = "baixo";
        } else if (5 < p && p <= 30) {
            category = "moderado";
        } else {
            category = actual_status;
        }
    } else {
        category = actual_status;
    }

    return category;
}


string SensorNodeModule::statusAnalysis(string categorized_data, string actual_status) {

    string new_status;
    int l=0, m=0, h=0;

    if(m_data_queue.size()>=5){
        m_data_queue.pop_front();
    }

    m_data_queue.push_back(categorized_data);

    for(uint32_t i = 0; i < m_data_queue.size(); i++) {
        if(m_data_queue[i]=="baixo"){
            l++;
        } else if(m_data_queue[i]=="moderado") {
            m++;
        } else if (m_data_queue[i]=="alto"){
            h++;
        }
    }

    if(l>=3){
        new_status = "baixo";
    } else if (m>=3) {
        new_status = "moderado";
    } else if (h>=3) {
        new_status = "alto";
    } else {
        new_status = actual_status;
    }

    return new_status;
}

void SensorNodeModule::sendSensorData(SensorData sensordata){
    Container container(sensordata);
    getConference().send(container);
    CLOG1 << sensordata.toString() << " sent at " << TimeStamp().getYYYYMMDD_HHMMSS() << endl;
}

timespec SensorNodeModule::elapsedTime(timespec &now, timespec &ref) {

    timespec diff;

    if ((now.tv_nsec - ref.tv_nsec) < 0) {
        diff.tv_sec = now.tv_sec - ref.tv_sec - 1;
        diff.tv_nsec = now.tv_nsec - ref.tv_nsec + 1000000000L;
    } else {
        diff.tv_sec = now.tv_sec - ref.tv_sec;
        diff.tv_nsec = now.tv_nsec - ref.tv_nsec;
    }

    return diff;
}

// CORPO
odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode SensorNodeModule::body() {

    timespec ts;        // timestamp do processador

    bool first_exec = true;
    int gd_duration, sta_duration, ssd_duration, print_duration, total_duration;
    double gd_sum = 0;
    double sta_sum = 0;
    double ssd_sum = 0;
    double print_sum = 0;
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;

    srand(time(NULL));  // raíz da função de randomização
    bool exe;           // variável do atuador
    int cycles = 0;     // contador de ciclos desde a ultima execução

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
        
        cycles++;
        exe = controllerFSM(cycles);    // para execução com controlador
        //exe = true;                   // para execuçao sem controlador

        if(exe){
            /*GERAR DADOS*/
            t1 = high_resolution_clock::now();
            string categorized_data = generateData(m_status);
            t2 = high_resolution_clock::now();
            gd_duration = duration_cast<microseconds>(t2 - t1).count();

            /*CAPTURAR INSTANTE DO PROCESSADOR*/
            clock_gettime(CLOCK_REALTIME, &ts);

            /*ANALISAR DADOS*/
            t1 = high_resolution_clock::now();
            m_status = statusAnalysis(categorized_data, m_status);
            t2 = high_resolution_clock::now();
            sta_duration = duration_cast<microseconds>(t2 - t1).count();

            /*ENVIAR ESTADO*/
            t1 = high_resolution_clock::now();
            SensorNodeModule::sendSensorData(SensorData(m_id, m_sensor_type, m_status, ts));
            t2 = high_resolution_clock::now();
            ssd_duration = duration_cast<microseconds>(t2 - t1).count();

            cycles = 0;

            t1 = high_resolution_clock::now();
            cout << "Actual status: " << m_status << " | Data sampled: " << categorized_data << " at " << TimeStamp().getYYYYMMDD_HHMMSSms() << endl;
            timespec t_esy = elapsedTime(ts, m_ref);
            m_status_log << (double)((t_esy.tv_sec) + (t_esy.tv_nsec/1E9)) << ",";
            m_status_log << ((m_status=="baixo")?1:(m_status=="moderado")?2:3) << ",";
            m_status_log << " " << "\n";
            t2 = high_resolution_clock::now();
            print_duration = duration_cast<microseconds>(t2 - t1).count();

            total_duration = gd_duration + sta_duration + ssd_duration + print_duration;

            if (first_exec)
            {
                gd_sum = (100 * ((float)gd_duration / (float)total_duration));
                ssd_sum = (100 * ((float)ssd_duration / (float)total_duration));
                sta_sum = (100 * ((float)sta_duration / (float)total_duration));
                print_sum = (100 * ((float)print_duration / (float)total_duration));
                first_exec = false;
            }
            else
            {
                gd_sum = (gd_sum + 100 * ((float)gd_duration / (float)total_duration)) / 2;
                ssd_sum = (ssd_sum + 100 * ((float)ssd_duration / (float)total_duration)) / 2;
                sta_sum = (sta_sum + 100 * ((float)sta_duration / (float)total_duration)) / 2;
                print_sum = (print_sum + 100 * ((float)print_duration / (float)total_duration)) / 2;
            }

            cout << "Time: \n";
            cout << "   generateData " << gd_sum << "\n";
            cout << "   statusAnalysis " << sta_sum << "\n";
            cout << "   sendSensorData " << ssd_sum << "\n";
            cout << "   prints " << print_sum << "\n";
        }
    }
    
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}