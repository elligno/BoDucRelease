// Qt includes
#include <QDir>
#include <QFile>
#include <QTextStream>
// App include
#include "BoDucBaseReport.h"

namespace bdAPI {

  BoDucBaseReport::BoDucBaseReport( QFile& aRepFile)
  : m_cmdReport(aRepFile)
  {
  }
  void BoDucBaseReport::createReport( const std::vector<bdAPI::BoDucFields>& aData2Report)
  {
    // maybe in ctor, would make more since we don't to set 
    // working folder each time we create a report. 
    setReportWorkingFolder();

    // construct a file 
    QFileInfo w_fileRep( m_reportFolder, m_cmdReport.fileName());

    // file doesn't exist
    if (!m_reportFolder.exists(m_cmdReport.fileName()))
    {
      QString w_filePath = w_fileRep.absoluteFilePath();

      // create a new file with the given name
      QFile w_data(w_fileRep.absoluteFilePath());

      // then create it write report
      if (w_data.open( QFile::WriteOnly | QFile::Text))
      {
        QTextStream out(&w_data);

        // first time we open it, create header
        out << "No Command " << "\t" << "Shipped To " << "\t" << "Deliver Date " << "\t"
          << "Code" << "\t" << "Product " << "\t" << "Quantity " << "\t" << "Silo" << "\n";

        for( const bdAPI::BoDucFields& w_bfield : aData2Report)
        {
          out << w_bfield.m_noCmd.c_str() << "\t" << w_bfield.m_deliverTo.c_str() << "\t" << w_bfield.m_datePromise.c_str() << "\t"
              << w_bfield.m_prodCode << "\t" << w_bfield.m_produit.c_str() << "\t" << w_bfield.m_qty << "\t" << w_bfield.m_silo.c_str() << "\n";
        }
        if (w_data.isOpen())
        {
          w_data.close(); // make sure we close we may want to re-open
        }
      }
    }
    else // file exist, just append data at the end
    {
      QFile w_data; // set file name of an existing one
      w_data.setFileName(w_fileRep.absoluteFilePath());

      if (w_data.open(QFile::WriteOnly | QFile::Append | QFile::Text))
      {
        QTextStream out(&w_data);
        out << "\n";

        for( const bdAPI::BoDucFields& w_bfield : aData2Report)
        {
          QString w_check = QString(w_bfield.m_deliverTo.data());

          out << w_bfield.m_noCmd.data() << "\t" << w_bfield.m_deliverTo.data() << "\t" << w_bfield.m_datePromise.data() << "\t"
            << w_bfield.m_prodCode << "\t" << w_bfield.m_produit.data() << "\t" << w_bfield.m_qty << "\t" << w_bfield.m_silo.data() << "\n";
//           out << w_bfield.noCmd() << "\t" << w_bfield.deliverTo() << "\t" << w_bfield.datePromise() << "\t"
//             << w_bfield.m_prodCode << "\t" << w_bfield.produit() << "\t" << w_bfield.m_qty << "\t" << w_bfield.silo() << "\n";
        }
        if (w_data.isOpen())
        {
          w_data.close();
        }
      }
    }
  }

  void BoDucBaseReport::setReportWorkingFolder()
  {
    //TODO: need to set working folder outside of the createDataReport method
    // and not hard coded (using string literal)
    // C:\JeanWorks\\BoDuc\App\\BoDucApp\BoDucAppGui i think correspond to current
    QDir w_currentDir = QDir::current(); // but we want a folder above
    QString w_dirName = w_currentDir.dirName(); // check
         
    //QDir w_workingDir(QString(R"(C:\JeanWorks\\BoDuc\App\BoDucApp\ReportFiles)"));
    //QDir w_reportFolder;
    
    // we need to move up why??
    if( w_currentDir.cdUp()) // folder higher of project folder (BoDucApp)
    {
      QString w_currNow = QDir::current().dirName(); // project folder
      QString w_pathNow = w_currentDir.absolutePath(); // BoDucApp

      m_reportFolder.setPath(w_pathNow);
      QString w_checkWork = m_reportFolder.dirName();

      // we should be at the BoDucApp level
      // need to check if folder exist, if not then create it 
      if (!m_reportFolder.exists( QString("ReportFiles")))
      {
        if (m_reportFolder.mkdir( QString("ReportFiles")))
        {
          m_reportFolder.cd( QString("ReportFiles"));
          //m_console->append("Folder ReportFiles created sucessfully");
          //std::cout << "Folder ReportFiles created sucessfully\n";
        }
      }
      else // set to the report folder
      {
        m_reportFolder.cd(QString("ReportFiles"));
      }
    }
  }

} // End of namespace

