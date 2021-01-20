#pragma once

// C++ includes
#include <iostream>
#include <string>
// STL includes
#include <map>
#include <vector>
// Boost includes
#include <boost/operators.hpp>
// Qt include
#include <QString>

namespace bdAPI
{
	struct BoDucFields : public boost::equality_comparable<BoDucFields>,
		public boost::less_than_comparable<BoDucFields>
	{
		// alias <cmdNo, shippedTo, deliveryDate,prod code, prod, qty,silo>
		using bdTpl = std::tuple<std::string, std::string, std::string, long, std::string,float, std::string>;
		
		// default ctor
		BoDucFields();
    // not sure yet, i come back later with more details
    BoDucFields( const BoDucFields& aOther) = default;
    BoDucFields& operator= (const BoDucFields& aOther) = default;
		//move semantic
		BoDucFields( bdTpl&& aTpl);
    ~BoDucFields() = default;

		friend bool operator== (const BoDucFields& aField1, const BoDucFields& aField2)
		{
			return (aField1.m_noCmd == aField2.m_noCmd) ? true : false;
		}
		friend bool operator< (const BoDucFields& aField1, const BoDucFields& aField2)
		{
			return (aField1.m_datePromise < aField2.m_datePromise) ? true : false;
		}
		friend std::ostream& operator<< (std::ostream& stream, const BoDucFields& aBdF)
		{
			stream << aBdF.m_noCmd << " " << aBdF.m_datePromise << " " << aBdF.m_deliverTo <<
				" " << aBdF.m_produit << " " << aBdF.m_prodCode << " " << aBdF.m_silo << " " << aBdF.m_qty << "\n";

			return stream;
		}

    // QString converter to handle utf-8 character
    QString noCmd()       const { return QString(m_noCmd.data());       }
    QString deliverTo()   const { return QString(m_deliverTo.data());   }
    QString datePromise() const { return QString(m_datePromise.data()); }
    QString produit()     const { return QString(m_produit.data());     }
    QString silo()        const { return QString(m_silo.data());        }
    float   qty()         const { return m_qty; }

		std::string m_noCmd;
		std::string m_deliverTo;
		std::string m_datePromise;
		signed long m_prodCode;
		std::string m_produit;
		float m_qty; // better to set as a string
		//not always an integer (can be 2&3, 4-A and a string such as "Un seul silo")
		std::string m_silo;
    bdTpl m_bdTpl;
	};
} // End of namespace