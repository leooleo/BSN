#include "bsn/data/DiastolicRawData.h"

namespace bsn {
    namespace data {

        DiastolicRawData::DiastolicRawData(const double &sensor_data) : RawData(sensor_data){}
        
        DiastolicRawData::DiastolicRawData(){}

        int32_t DiastolicRawData::ID() {
            return 885;
        }

        int32_t DiastolicRawData::getID() const {
            return DiastolicRawData::ID();
        }

    }
}