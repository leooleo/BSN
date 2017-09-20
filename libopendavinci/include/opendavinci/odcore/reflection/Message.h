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

#ifndef OPENDAVINCI_CORE_REFLECTION_MESSAGE_H_
#define OPENDAVINCI_CORE_REFLECTION_MESSAGE_H_

#include <memory>
#include <vector>

#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/data/SerializableData.h"
#include "opendavinci/odcore/base/Visitable.h"
#include "opendavinci/odcore/reflection/Field.h"
#include "opendavinci/generated/odcore/data/reflection/AbstractField.h"

namespace odcore { namespace base { class Visitor; } }

namespace odcore {
    namespace reflection {

        using namespace std;

        /**
         * This class is a generic Message representation.
         */
        class OPENDAVINCI_API Message : public odcore::data::SerializableData, public odcore::base::Visitable {
            public:
                Message();

                /**
                 * Copy constructor.
                 *
                 * @param obj Reference to an object of this class.
                 */
                Message(const Message &obj);

                virtual ~Message();

                /**
                 * Assignment operator.
                 *
                 * @param obj Reference to an object of this class.
                 * @return Reference to this instance.
                 */
                Message& operator=(const Message &obj);

            private:
                // Implementation of serialization of a Message's content is by design disabled.
                virtual ostream& operator<<(ostream &out) const;

                // Implementation of serialization of a Message's content is by design disabled.
                virtual istream& operator>>(istream &in);

            public:
                virtual void accept(odcore::base::Visitor &v);

                virtual int32_t getID() const;

                virtual const string getShortName() const;

                virtual const string getLongName() const;

                virtual const string toString() const;

            public:
                void setID(const int32_t &id);
                void setShortName(const string &sn);
                void setLongName(const string &ln);

            private:
                /**
                 * This method retrieves the current value from the list of
                 * fields, visits the value, and updates it in the case that
                 * the Visitor might have altered the value.
                 *
                 * @param v Visitor.
                 * @param f Field to visit.
                 */
                template<typename T>
                void visitPrimitiveDataType(odcore::base::Visitor &v, std::shared_ptr<odcore::data::reflection::AbstractField> &f) {
                    // Read value.
                    T value = dynamic_cast<odcore::reflection::Field<T>*>(f.operator->())->getValue();
                    // Visit value.
                    v.visit(f->getFieldIdentifier(), f->getLongFieldName(), f->getShortFieldName(), value);
                    // Update value.
                    dynamic_cast<odcore::reflection::Field<T>*>(f.operator->())->setValue(value);
                }

            public:
                /**
                 * This method inserts a field to this generic message representation at the beginning of the list of fields.
                 *
                 * @param Field to be added.
                 */
                void insertField(const std::shared_ptr<odcore::data::reflection::AbstractField> &f);

                /**
                 * This method adds a field to this generic message representation at the end of the list of fields.
                 *
                 * @param Field to be added.
                 */
                void addField(const std::shared_ptr<odcore::data::reflection::AbstractField> &f);

                /**
                 * This method returns the number of fields.
                 *
                 * @return The number of fields in this message.
                 */
                uint32_t getNumberOfFields() const;

                /**
                 * This method tries to find a field with the given identifier.
                 *
                 * @param ID to find.
                 * @param found Flag modified by this method indicating if the field was found.
                 * @return found Be aware to always check 'found' whether the field was found.
                 */
                std::shared_ptr<odcore::data::reflection::AbstractField> getFieldByIdentifier(const uint32_t &id, bool &found);

                /**
                 * This method tries to extract the specified scalar type from AbstractField.
                 *
                 * @param id to find.
                 * @param found Flag modified by this method indicating if the field was found.
                 * @param extracted Flag modified by this method indicating if the field was successfully extracted.
                 * @return Extracted value.
                 */
                template<typename T>
                T getValueFromScalarField(const uint32_t &id, bool &found, bool &extracted) {
                    T value = 0;
                    extracted = false;
                    std::shared_ptr<odcore::data::reflection::AbstractField> af = getFieldByIdentifier(id, found);
                    if (found) {
                        extracted = true;
                        switch(af->getFieldDataType()) {
                            case odcore::data::reflection::AbstractField::BOOL_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<bool>*>(af.get())->getValue());
                            break;
                            case odcore::data::reflection::AbstractField::UINT8_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<uint8_t>*>(af.get())->getValue());
                            break;
                            case odcore::data::reflection::AbstractField::INT8_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<int8_t>*>(af.get())->getValue());
                            break;
                            case odcore::data::reflection::AbstractField::UCHAR_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<unsigned char>*>(af.get())->getValue());
                            break;
                            case odcore::data::reflection::AbstractField::CHAR_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<char>*>(af.get())->getValue());
                            break;
                            case odcore::data::reflection::AbstractField::UINT16_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<uint16_t>*>(af.get())->getValue());
                            break;
                            case odcore::data::reflection::AbstractField::INT16_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<int16_t>*>(af.get())->getValue());
                            break;
                            case odcore::data::reflection::AbstractField::UINT32_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<uint32_t>*>(af.get())->getValue());
                            break;
                            case odcore::data::reflection::AbstractField::INT32_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<int32_t>*>(af.get())->getValue());
                            break;
                            case odcore::data::reflection::AbstractField::UINT64_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<uint64_t>*>(af.get())->getValue());
                            break;
                            case odcore::data::reflection::AbstractField::INT64_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<int64_t>*>(af.get())->getValue());
                            break;
                            case odcore::data::reflection::AbstractField::FLOAT_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<float>*>(af.get())->getValue());
                            break;
                            case odcore::data::reflection::AbstractField::DOUBLE_T:
                                value = static_cast<T>(dynamic_cast<odcore::reflection::Field<double>*>(af.get())->getValue());
                            break;
                            default:
                                extracted = false;
                            break;
                        }
                    }
                    return value;
                }

            private:
                int32_t m_ID;
                string m_shortName;
                string m_longName;
                vector<std::shared_ptr<odcore::data::reflection::AbstractField> > m_fields;
        };

    }
} // odcore::reflection

#endif /*OPENDAVINCI_CORE_REFLECTION_MESSAGE_H_*/
