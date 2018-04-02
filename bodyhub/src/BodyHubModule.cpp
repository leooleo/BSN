/*
 * Módulo do Nó Sensor
 * 
 * @author Ricardo Diniz Caldas
 * @version v1.0
 */
#include <chrono>
#include "BodyHubModule.h"

using namespace std::chrono;

BodyHubModule::BodyHubModule(const int32_t &argc, char **argv) :
    TimeTriggeredConferenceClientModule(argc, argv, "bodyhub"),
    m_id(getIdentifier()),
    m_buffer(),
    m_health_status("-"),
    m_sensor(),
    m_status_log(),
    m_ref() {}

BodyHubModule::~BodyHubModule() {}


// CONFIGURAÇÃO
void BodyHubModule::setUp() {
    addDataStoreFor(873, m_buffer); // "Avisa" ao buffer que vai receber dados do tipo SensorNodeData

    clock_gettime(CLOCK_REALTIME, &m_ref); // referência para medidas de tempo  

    // Abre arquivo para persistencia de dados
    string path = "output/";    
    m_status_log.open(path+"bodyhub_status_log.csv");
    m_status_log << "ID do sensor, Estado do Termômetro, Estado do ECG, Estado do Oxímetro, Estado do Paciente, Enviado às (s), Recebido às (s), Processado às (s), Diff (s)\n";
}

// DESTRUIÇÃO
void BodyHubModule::tearDown() {
    m_status_log.close(); // Fecha arquivo para persistencia de dados
}

string BodyHubModule::calculateHealthStatus(){
    double hr = 0;
    for(uint32_t i = 0; i < m_sensor.size(); i++){

        if (m_sensor[i] == "baixo") {
            hr += 0.1;
        } else if (m_sensor[i] == "moderado") {
            hr += 1;
        } else if (m_sensor[i] == "alto") {
            hr += 5;
        } 
    }
    
    return (hr<=0)?"unknown":(hr<1)?"bom":(hr<5)?"medio":(hr<20)?"ruim":"unknown";
}

void BodyHubModule::updateHealthStatus(SensorData sensordata){
    m_sensor[sensordata.getSensorType()-1] = sensordata.getSensorStatus();
    m_health_status = BodyHubModule::calculateHealthStatus();
}

timespec BodyHubModule::elapsedTime(timespec &now, timespec &ref) {

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

void BodyHubModule::persistHealthStatus(uint32_t sensor_id, timespec t_sen, timespec t_rec){
    
    timespec t_proc;
    clock_gettime(CLOCK_REALTIME, &t_proc);

    timespec sent = elapsedTime(t_sen, m_ref);
    timespec received = elapsedTime(t_rec, m_ref);
    timespec processed = elapsedTime(t_proc, m_ref);

    m_status_log << sensor_id << ",";

    for(uint32_t i = 0; i < m_sensor.size(); i++){
        m_status_log << m_sensor[i] << ",";
    }

    m_status_log << m_health_status << ",";
    m_status_log << (double)((sent.tv_sec) + (sent.tv_nsec/1E9)) << ",";
    m_status_log << (double)((received.tv_sec) + (received.tv_nsec/1E9)) << ",";
    m_status_log << (double)((processed.tv_sec) + (processed.tv_nsec/1E9)) << ",";

    timespec result = elapsedTime(t_rec, t_sen);
    m_status_log << (result.tv_sec) + (result.tv_nsec/1E9) << "\n";
}

void BodyHubModule::printHealthStatus(){
    cout << "----------------------------------------"<<endl;
    for(uint32_t i = 0; i < 3; i++){
        cout << ((i==0)?"Thermometer: ":(i==1)?"ECG: ":"Oximeter: ");
        cout << m_sensor[i] << endl;
    }
    cout << "Health Status: " << m_health_status << endl;
    cout << "----------------------------------------"<<endl;
}

// CORPO
odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode BodyHubModule::body() { 
    
    timespec ts; // timestamp

    bool first_exec = true;
    int uhs_duration,phs_duration, emg_duration, print_duration, total_duration;
    double uhs_sum = 0;
    double phs_sum = 0;
    double emg_sum = 0;
    double print_sum = 0;
    
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;

    bool is_emergency = false; // variável booleana utilizada para avisar sobre estados de emergencia
    uint32_t sensor_id = 0;    // varável utilizada para capturar e persistir o id do sensor que enviou os dados

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
        
        while(!m_buffer.isEmpty()){            

            clock_gettime(CLOCK_REALTIME, &ts); //captura novo timestamp do processador para imprimir no registro

            // CONSOME DADO
            Container container = m_buffer.leave();

            if (container.getDataType() == SensorData::ID()) {

                // ATUALIZA ESTADO DO PACIENTE
                t1 = high_resolution_clock::now();                
                BodyHubModule::updateHealthStatus(container.getData<SensorData>());
                sensor_id = container.getData<SensorData>().getSensorID();
                t2 = high_resolution_clock::now();

                uhs_duration = duration_cast<microseconds>( t2 - t1 ).count();
                // DETECTA EMERGÊNCIA
                t1 = high_resolution_clock::now(); 
                is_emergency=(container.getData<SensorData>().getSensorStatus() == "alto")?true:false;
                CLOG1<<"Emergencia?"<<is_emergency<<endl;
                t2 = high_resolution_clock::now(); 

                emg_duration = duration_cast<microseconds>( t2 - t1 ).count();

                // PERSISTE
                t1 = high_resolution_clock::now(); 
                BodyHubModule::persistHealthStatus(sensor_id, container.getData<SensorData>().getSentTimespec(), ts);
                t2 = high_resolution_clock::now(); 

                phs_duration = duration_cast<microseconds>( t2 - t1 ).count();
            }

            //imprime dados atuais
            t1 = high_resolution_clock::now(); 
            BodyHubModule::printHealthStatus();
            t2 = high_resolution_clock::now(); 

            print_duration = duration_cast<microseconds>( t2 - t1 ).count();

            total_duration = phs_duration + uhs_duration + emg_duration + print_duration;

            if(first_exec){
                uhs_sum = (100* ((float)uhs_duration / (float)total_duration ));
                phs_sum = (100* ((float)phs_duration / (float)total_duration ));
                emg_sum = (100* ((float)emg_duration / (float)total_duration ));
                print_sum = (100* ((float)print_duration / (float)total_duration ));
                first_exec = false;
            }
            else{
                uhs_sum = (uhs_sum + 100* ((float)uhs_duration / (float)total_duration ))/2;
                phs_sum = (phs_sum + 100* ((float)phs_duration / (float)total_duration ))/2;
                emg_sum = (emg_sum +100* ((float)emg_duration / (float)total_duration ))/2;
                print_sum = (print_sum +100* ((float)print_duration / (float)total_duration ))/2;
  
            }

            cout << "Time: \n";
            cout << "   uhs " << uhs_sum<< "\n";
            cout << "   phs " << phs_sum<< "\n";
            cout << "   emg " <<  emg_sum << "\n";
            cout << "   print " <<  print_sum << "\n";
            
        }            
        
    }
    
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
