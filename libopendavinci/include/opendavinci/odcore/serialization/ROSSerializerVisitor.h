/**
 * OpenDaVINCI - Portable middleware for distributed components.
 * Copyright (C) 2015 Christian Berger
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef OPENDAVINCI_CORE_SERIALIZATION_ROSSERIALIZERVISITOR_H_
#define OPENDAVINCI_CORE_SERIALIZATION_ROSSERIALIZERVISITOR_H_

#include <sstream>
#include <string>

#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/serialization/Serializer.h"
#include "opendavinci/odcore/base/Visitor.h"

namespace odcore {
    namespace serialization {

class Serializable;

        using namespace std;

        /**
         * This class provides a serialization visitor to encode data
         * in ROS format.
         */
        class ROSSerializerVisitor : public Serializer, public odcore::base::Visitor {
            private:
                /**
                 * "Forbidden" copy constructor. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the copy constructor.
                 */
                ROSSerializerVisitor(const ROSSerializerVisitor &);

                /**
                 * "Forbidden" assignment operator. Goal: The compiler should warn
                 * already at compile time for unwanted bugs caused by any misuse
                 * of the assignment operator.
                 */
                ROSSerializerVisitor& operator=(const ROSSerializerVisitor &);

            public:
                ROSSerializerVisitor();

                virtual ~ROSSerializerVisitor();

                virtual void getSerializedData(ostream &o);

                /**
                 * This method sets the message identifier.
                 *
                 * @param messageId
                 */
                void setMessageID(const uint8_t &messageId);

            private:
                virtual uint32_t writeValue(ostream &o, const Serializable &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const bool &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const char &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const unsigned char &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const int8_t &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const int16_t &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const uint16_t &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const int32_t &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const uint32_t &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const int64_t &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const uint64_t &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const float &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const double &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const string &v) {(void)o; (void)v; return 0;}
                virtual uint32_t writeValue(ostream &o, const void *data, const uint32_t &size) {(void)o; (void)data; (void)size; return 0;}

            private:
                virtual void write(const uint32_t &id, const Serializable &s);
                virtual void write(const uint32_t &id, const bool &b);
                virtual void write(const uint32_t &id, const char &c);
                virtual void write(const uint32_t &id, const unsigned char &uc);
                virtual void write(const uint32_t &id, const int8_t &i);
                virtual void write(const uint32_t &id, const int16_t &i);
                virtual void write(const uint32_t &id, const uint16_t &ui);
                virtual void write(const uint32_t &id, const int32_t &i);
                virtual void write(const uint32_t &id, const uint32_t &ui);
                virtual void write(const uint32_t &id, const int64_t &i);
                virtual void write(const uint32_t &id, const uint64_t &ui);
                virtual void write(const uint32_t &id, const float &f);
                virtual void write(const uint32_t &id, const double &d);
                virtual void write(const uint32_t &id, const string &s);
                virtual void write(const uint32_t &id, const void *data, const uint32_t &size);

            private:
                virtual void beginVisit(const int32_t &id, const string &shortName, const string &longName);
                virtual void endVisit();

                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const Serializable &s);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const bool &b);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const char &c);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const unsigned char &uc);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const int8_t &i);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const int16_t &i);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const uint16_t &ui);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const int32_t &i);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const uint32_t &ui);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const int64_t &i);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const uint64_t &ui);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const float &f);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const double &d);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const string &s);
                virtual void write(const uint32_t &id, const string &longName, const string &shortName, const void *data, const uint32_t &size);

            public:
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, Serializable &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, bool &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, char &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, unsigned char &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, int8_t &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, int16_t &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, uint16_t &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, int32_t &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, uint32_t &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, int64_t &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, uint64_t &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, float &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, double &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, string &v);
                virtual void visit(const uint32_t &id, const string &longName, const string &shortName, void *data, const uint32_t &size);

            private:
                uint8_t m_messageId;
                uint32_t m_size;
                stringstream m_buffer;
        };

    }
} // odcore::serialization

#endif /*OPENDAVINCI_CORE_SERIALIZATION_ROSSERIALIZERVISITOR_H_*/
