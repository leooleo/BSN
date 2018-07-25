#include "../include/DataSender.hpp"

DataSender::DataSender(const int32_t &argc, char **argv) :
    TimeTriggeredConferenceClientModule(argc, argv, "handler"),
    m_buffer() {}

DataSender::~DataSender() {}

TCPSend sender("localhost",8000);

void DataSender::setUp() {
    // Recebe FilteredData
    addDataStoreFor(876, m_buffer);
    sender.set_port(getIdentifier());
    sender.connect();
}

void DataSender::tearDown(){    
    sender.disconnect();
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode DataSender::body(){    
    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {

        while(!m_buffer.isEmpty()){
            Container container = m_buffer.leave();
            std::cout << "Dado recebido: " << to_string(container.getData<RawData>().getSensorData()) << std::endl;
            sender.send(to_string(container.getData<RawData>().getSensorData()));
        }

    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}