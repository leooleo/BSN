#include "SensorModule.h"

#include "opendavinci/odcore/base/FIFOQueue.h"
#include "opendavinci/generated/odcore/data/dmcp/PulseAckMessage.h"
#include "opendavinci/odcore/base/Thread.h"

#include <iostream>

using namespace std;


using namespace odcore::base;
using namespace odcore::base::module;
using namespace odcore::data;
using namespace odcore::data::dmcp;

using namespace openbasn::data;
using namespace openbasn::message;
using namespace openbasn::model::sensor;


SensorModule::SensorModule(const int32_t &argc, char **argv) :
    TimeTriggeredConferenceClientModule(argc, argv, "sensor"),
    m_id(getIdentifier()),
    m_buffer(),
    m_clock(0),
    m_sensor(),
    m_emergency_state(false) {}

SensorModule::~SensorModule() {}

void SensorModule::setUp() {
    //setup module buffer
    addDataStoreFor(871, m_buffer);
    addDataStoreFor(872, m_buffer);

    SensorModule::getSensorConfiguration();
}

void SensorModule::tearDown() {}

void SensorModule::getSensorConfiguration(){
    int32_t sensortypes = getKeyValueConfiguration().getValue<int32_t>("global.sensortypes");
    
    for(int32_t i = 0; i < sensortypes; i++) {
        string sensor_type;
        int32_t sensor_id = i+1;

        sensor_type = getKeyValueConfiguration().getValue<string>("global.sensortype."+ to_string(sensor_id));

        if((getKeyValueConfiguration().getValue<bool>("sensor."+ sensor_type +".active") == 1)){
            bool   active = (getKeyValueConfiguration().getValue<bool>("sensor."+ sensor_type +".active") == 1);
            float  samplerate = getKeyValueConfiguration().getValue<float>("sensor."+ sensor_type +".samplerate");
            string mean_behavior = getKeyValueConfiguration().getValue<string>("sensor."+ sensor_type +".mean.behavior");
            double stddev = getKeyValueConfiguration().getValue<double>("sensor."+ sensor_type +".stddev");
            double mean = getKeyValueConfiguration().getValue<double>("sensor."+ sensor_type +".mean."+mean_behavior);

            m_sensor = Sensor(sensor_id, samplerate, active, mean, stddev);
        }
    }
}

void SensorModule::sendSensorData(SensorData sensordata){
    Container container(sensordata);
    getConference().send(container);
    CLOG1 << sensordata.toString() << " sent at " << TimeStamp().getYYYYMMDD_HHMMSS() << endl;
}

string SensorModule::categorize(uint32_t type, double data) {
    
    switch(type){

        case Sensor::THERMOMETER:
            if(37 >= data && data > 35){
                return "low"; 
            } else if ( (35 >= data && data > 30) || (38 >= data && data > 37) ) {
                return "moderate";
            } else if ( (50 >= data && data > 38) || (30 >= data && data > 0) ) {
                return "high";
            } else {
                return "unknown"; 
            }
            break;

        case Sensor::ECG:
            if ( (150 >= data && data > 120) || (80 >= data && data > 0) ) {
                return "high";
            } else if (120 >= data && data > 80) {
                return "low";
            } else {
                return "unknown";
            }
            break;

        case Sensor::OXIMETER:
            if( 100 >= data && data > 94) {
                return "low";
            } else if ( 94 >= data && data > 90) {
                return "moderate";
            } else if ( 90 >= data && data > 0) {
                return "high";
            } else {
                return "unknown";
            }
            break;

        default:
            return "unknown";
            break;
    }
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode SensorModule::body() {

    TimeStamp previous;
    string sensor_risk="low";
    bool flag = true;

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
        
        /* while(!m_buffer.isEmpty()){
            Container c = m_buffer.leave();

            if(c.getDataType() == Alert::ID()){
                m_emergency_state=true;
            }

        } */
        if(flag){
            sensor_risk = SensorModule::categorize(m_sensor.getType(),m_sensor.getData());
            flag=false;
        } 

        m_clock = ((TimeStamp()-previous).toMicroseconds())/1000000L;
        CLOG1<<"M_CLOCK: "<<m_clock<<endl;
        if(!m_emergency_state) {

            if(((sensor_risk=="high" || sensor_risk=="unknown") && m_clock>1) 
                ||(sensor_risk=="moderate" && m_clock>5)
                ||(sensor_risk=="low" && m_clock>15)) {

                    SensorModule::sendSensorData(SensorData(m_id, m_sensor.getType(), m_sensor.getData(), sensor_risk));
                    flag=true;
                    previous = TimeStamp();
                }

            
        } else {
            SensorModule::sendSensorData(SensorData(m_id, m_sensor.getType(), m_sensor.getData(), sensor_risk));
            flag=true;
            previous = TimeStamp();
        } 
        
        PulseAckMessage pulseackmessage;
        Container c(pulseackmessage);
        getConference().send(c);
    }
    
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}