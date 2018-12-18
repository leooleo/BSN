#ifndef BATTERY_LEVEL_H
#define BATTERY_LEVEL_H

#include "opendavinci/odcore/data/SerializableData.h"
#include "opendavinci/odcore/serialization/Deserializer.h"
#include "opendavinci/odcore/serialization/SerializationFactory.h"
#include "opendavinci/odcore/serialization/Serializer.h"

#include <stdint.h>

namespace bsn {
    namespace data {
        
        class BatteryLevel : public odcore::data::SerializableData {
        
            public:
                BatteryLevel(const double &/*units*/);
 
                BatteryLevel(); // construtor
                virtual ~BatteryLevel();// destrutor 
                
                //Boas praticas do manual do OpenDaVINCI
                BatteryLevel(const BatteryLevel &/*obj*/);
                BatteryLevel& operator=(const BatteryLevel &/*obj*/);
            
            // Métodos abstratos
            public:
                virtual int32_t getID() const;
                virtual const std::string getShortName() const;
                virtual const std::string getLongName() const;

                static int32_t ID();
                static const std::string ShortName();
                static const std::string LongName();
        
            public:
                virtual std::ostream& operator<<(std::ostream &out) const;
                virtual std::istream& operator>>(std::istream &in);
        
                virtual const std::string toString() const;
                
            // setters e getters
            public:
                void setUnits(const double &/*units*/);
                double getUnits() const;
            
            private:
                double units;
        };
    }
}

#endif