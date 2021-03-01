#pragma once

// Qt includes
#include <QTextStream>
#include <QFile>
#include <QDir>
// App include
#include "BoDucFields.h"

namespace bdAPI 
{
  /** Manage file CommandReport.txt which act a file data store*/
  class FileDataStore 
  {
  public:
    FileDataStore() 
    : m_fileName("CommandReport.txt"), 
      m_cmdReportFile(m_fileName),
      m_reportFolder(QString("ReportFiles")) // how do i set this variable? 
    {
      // set default report folder ("ReportFiles")
      // check if file exist?
      // if not exist create the file

      // construct a file 
      QFileInfo w_fileRep( m_reportFolder, m_cmdReportFile.fileName());
      QFile w_data(w_fileRep.absoluteFilePath());
      if( !w_fileRep.exists())
      {
        // create it, by open it we create the file, i mean TextStream
        // create a new file with the given name
        if( w_data.open( QFile::WriteOnly | QFile::Text))
        {
          // problem opening (creating the new file)
        }
      }
      // check if it is open first?
      w_data.close();
    }
    FileDataStore( const FileDataStore& aOther) = delete;
    FileDataStore& operator= ( const FileDataStore& aOther) = delete;
    ~FileDataStore() 
    {
      // check if it is open, then close
      if( m_cmdReportFile.isOpen())
      {
        m_cmdReportFile.close();
      }
    }

    bool open()
    {
      // file doesn't exist
//       if (!m_reportFolder.exists(m_cmdReport.fileName()))
//       {
//         QString w_filePath = w_fileRep.absoluteFilePath();
// 
//         // create a new file with the given name
//         QFile w_data(w_fileRep.absoluteFilePath());
//       }
    }

    void close()
    {
      // check if it is open, then close
      if( m_cmdReportFile.isOpen())
      {
        m_cmdReportFile.close();
      }
    }
    
    void save( const std::vector<bdAPI::BoDucFields>& aData2Report)
    {
      // write to file
      QFile w_data; // set file name of an existing one
      w_data.setFileName( m_cmdReportFile.absoluteFilePath());

      if (w_data.open( QFile::WriteOnly | QFile::Append | QFile::Text))
      {
        QTextStream out( &w_data);
        out << "\n";

        for ( const bdAPI::BoDucFields& w_bfield : aData2Report)
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

  private:
    QString m_fileName;
    QFile m_cmdReportFile;
    QDir m_reportFolder;   // how i set default value for this folder
  };
} // End of namespace
