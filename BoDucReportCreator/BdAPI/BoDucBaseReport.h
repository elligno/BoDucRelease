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
    BoDucBaseReport( const QFile& aRepFile);

    /** Base method to be overriden by subclass according to report type*/
    // it shall be abstract but for now we keep it this way
    virtual void createReport( const std::vector<bdAPI::BoDucFields>& aData2Report);

    /** R-value version*/
    virtual void createReport(std::vector<bdAPI::BoDucFields>&& aData2Report);
    
    /** */
    void setReportWorkingFolder(QDir aWorkingFolder) { m_reportFolder = aWorkingFolder; }

    /** */
    QString reportFileName() const { return m_cmdReport.fileName(); }

    /** */
    void setFormat() {}

  private:
    QFile m_cmdReport;  //<** */
    QDir m_reportFolder;
  };
} //End of namespace
