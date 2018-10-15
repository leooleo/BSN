#include "../include/ControllerModule.hpp"

using namespace odcore::data;

using namespace bsn::data;
using namespace bsn::operation;
using namespace bsn::range;

ControllerModule::ControllerModule(const int32_t &argc, char **argv) :
    TimeTriggeredConferenceClientModule(argc, argv, "ControllerModule"),
    data_buffer(){}

ControllerModule::~ControllerModule() {}

void ControllerModule::setUp() {
    addDataStoreFor(878, data_buffer);
    
    vector<string> low, mid, high;
    vector<Range> ranges;
    Operation operation;

    std::string sensorType = getKeyValueConfiguration().getValue<std::string>("global.type");


    low = operation.split(getKeyValueConfiguration().getValue<string>("global." + sensorType + "lowRange"), ',');
    mid = operation.split(getKeyValueConfiguration().getValue<string>("global." + sensorType + "midRange"), ',');
    high = operation.split(getKeyValueConfiguration().getValue<string>("global." + sensorType + "highRange"), ',');

    Range lowRange(stod(low[0]), stod(low[1]));
    Range midRange(stod(mid[0]), stod(mid[1]));
    Range highRange(stod(high[0]), stod(high[1]));

    ranges_array[0] = lowRange;
    ranges_array[1] = midRange;
    ranges_array[2] = highRange;
}

void ControllerModule::tearDown(){}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode ControllerModule::body(){
   
    Container container;
    double rawVal;
    int sensorStatus = 0;
    int freq = 1;

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
        
        while(!data_buffer.isEmpty()) {
            
            //Receber o dado do sensor
            container = data_buffer.leave();
            rawVal = container.getData<SensorStatusInfo>().getValue();

            //verificar o estado do sensor
            int newStatus;
            if(ranges_array[0].in_range(rawVal)) {
                newStatus = 0;
            } else if(ranges_array[1].in_range(rawVal)) {
                newStatus = 1;
            } else if(ranges_array[2].in_range(rawVal)) {
                newStatus = 2;
            } else {
                throw std::out_of_range("O valor não pertence a um dos estados!");
            }
            

            //se necessario, enviar uma mensagem ao DataCollector com a freq
            if(sensorStatus != newStatus) {

                switch(sensorStatus){
                    case 0:
                        freq = 1;
                        break;
                    case 1:
                        freq = 5;
                        break;
                    case 2: 
                        freq = 10;
                        break;
                }
                FreqUpdate fUpdate(freq);
                Container fUpdContainer(fUpdate);
                getConference().send(fUpdContainer);
            }

        }
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
