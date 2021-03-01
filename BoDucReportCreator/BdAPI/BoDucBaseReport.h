#pragma once

#include "BoDucFields.h"

// forward declaration
class QFile;

namespace bdAPI 
{
  /** Base class for all kind of BoDuc report*/
  // Current version save data to command report file (datastore) 
  class BoDucBaseReport 
  {
  public:

    // not sure if i should pass it as reference or value? good question
    BoDucBaseReport( QFile& aRepFile);

    /** Base method to be overriden by subclass according to report type*/
    // it shall be abstract but for now we keep it this way
    virtual void createReport( const std::vector<bdAPI::BoDucFields>& aData2Report);

    void setReportWorkingFolder();

    /** */
    QString reportFileName() const;

    /** */
    void setFormat() {}

  private:
    QFile& m_cmdReport;
    QDir m_reportFolder;
  };

  // Done in a separate file
//   // Create a bon livraison report
//   class BoDucBonLivraison : BoDucBaseReport 
//   {
//   public:
//     void setUnitNo(unsigned aUnitNo) { m_unitNo = aUnitNo; }
//     unsigned unitNo() { return m_unitNo; }
//   private:
//     unsigned m_unitNo; /**< unit number*/
//   };
} //End of namespace
