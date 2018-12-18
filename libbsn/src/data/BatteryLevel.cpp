#include "bsn/data/BatteryLevel.h"

namespace bsn {
    namespace data {
        
        using namespace std;

        BatteryLevel::BatteryLevel() : units() {}
        
        BatteryLevel::BatteryLevel(const double &_units) : 
            units(_units) {}
        
        BatteryLevel::~BatteryLevel() {}
        
        BatteryLevel::BatteryLevel(const BatteryLevel &obj) :
            SerializableData(),
            units(obj.getUnits()){}
        
        BatteryLevel& BatteryLevel::operator=(const BatteryLevel &obj) {
            units = obj.getUnits();        
            return (*this);
        }
        
        int32_t BatteryLevel::ID() {
            return 881;
        }
        const string BatteryLevel::ShortName() {
            return "BatteryLevel";
        }
        const string BatteryLevel::LongName() {
            return "data.BatteryLevel";
        }
        
        int32_t BatteryLevel::getID() const {
            return BatteryLevel::ID();
        }
        const string BatteryLevel::getShortName() const {
            return BatteryLevel::ShortName();
        }
        const string BatteryLevel::getLongName() const {
            return BatteryLevel::LongName();
        }
        
        void BatteryLevel::setUnits(const double &_units) {
            units = _units;
        }

        double BatteryLevel::getUnits() const {
            return units;
        }

        ostream& BatteryLevel::operator<<(ostream &out) const {
            odcore::serialization::SerializationFactory& sf=odcore::serialization::SerializationFactory::getInstance();
            std::shared_ptr<odcore::serialization::Serializer> s = sf.getQueryableNetstringsSerializer(out);
            
            s->write(1, units);


            return out;
        }
        
        istream& BatteryLevel::operator>>(istream &in) {
            odcore::serialization::SerializationFactory& sf=odcore::serialization::SerializationFactory::getInstance();
            std::shared_ptr<odcore::serialization::Deserializer> d = sf.getQueryableNetstringsDeserializer(in);

            d->read(1, units);

            return in;
        }
        
        const string BatteryLevel::toString() const {
            stringstream sstr;

            sstr << "UpdatedData#" << units << endl;

            return sstr.str();
        }
    }
}