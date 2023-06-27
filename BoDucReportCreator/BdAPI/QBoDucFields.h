#pragma once

// STL includes
#include <vector>
// Boost includes
#include <boost/operators.hpp>
// Qt include
#include <QString>

namespace bdAPI 
{
  /** Aggregate ... to be completed*/
  struct QBoDucFields : public boost::equality_comparable<QBoDucFields>,
                        public boost::less_than_comparable<QBoDucFields>
  {
    QString m_noCmd;          /**< command tag*/
    QString m_deliverTo;      /**< shipping address*/
    QString m_datePromise;    /**< delivery date*/
    signed long m_prodCode;   /**< product code (digits)*/
    QString m_produit;        /**< product description*/
    float m_qty;             // better to set as a string
    // not always an integer (can be 2&3, 4-A and a string such as "UN SEUL SILO")
    QString m_silo;           /**< silo number*/

    friend bool operator== (const QBoDucFields& aField1, const QBoDucFields& aField2)
    {
      return (aField1.m_noCmd == aField2.m_noCmd) ? true : false;
    }
    friend bool operator< (const QBoDucFields& aField1, const QBoDucFields& aField2)
    {
      return (aField1.m_datePromise < aField2.m_datePromise) ? true : false;
    }
    friend std::ostream& operator<< ( std::ostream& stream, const QBoDucFields& aBdF)
    {
      stream << aBdF.m_noCmd.toStdString() << " " << aBdF.m_deliverTo.toStdString() << " " << aBdF.m_datePromise.toStdString()
        << " " << aBdF.m_prodCode <<  " " << aBdF.m_produit.toStdString() << " " << aBdF.m_qty << " "  << aBdF.m_silo.toStdString() << "\n";

      return stream;
    }
  };
} // End of namespace
