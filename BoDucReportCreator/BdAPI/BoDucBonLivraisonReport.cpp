// Qt include
#include <QMessageBox>
// App include
#include "BoDucBonLivraisonReport.h"

namespace bdAPI 
{
  void BonLivraisonReport::createReport( const BoDucBonLivraison& aBon2Create)
  {
    // what are the steps for creating bon livraison
//       if( aBon2Create.isEmpty())
//       {
//         return;
//       }

      // debug purpose
    auto w_unitNo = aBon2Create.unitNo();
    QString w_uniteON( std::to_string( aBon2Create.unitNo()).data());

    // retrieve vector of cmd for this unit that user has saved
  //  std::vector<bdAPI::BoDucFields>& w_unitVec = m_cmdUnitSaved[aBon2Create.unitNo()];
    using tplbondata = BoDucBonLivraison::tplcmdata;
    QVector<tplbondata> w_vecCmd = aBon2Create.getBonLivraison();

    //QDir w_reportFolder; 
    // not sure how to handle the folder where report is saved
    // look for a specific folder name ??
//     QString w_currentPath = QDir::currentPath();
//     if( m_dirName.dirName() == w_path)
//     {
//     }
    // just a test 
    QString w_checkPath = m_dirName.absolutePath();

    QString w_bonLivraisonFile = w_uniteON + m_nameSuffix;
    QFileInfo w_fileRep( m_dirName.absolutePath(), w_bonLivraisonFile);

    // create a new file with the given name
    QFile w_data( w_fileRep.absoluteFilePath());
    if( !w_fileRep.exists(w_bonLivraisonFile))
    {
      QTextStream out(&w_data);
      // then create it write report
      if( w_data.open(QFile::WriteOnly | QFile::Text))
      {
        for( auto i = 0; i < w_vecCmd.size(); ++i)
        {
          tplbondata w_val2File = w_vecCmd.at(i);
          out << w_uniteON << "\t" << std::get<0>(w_val2File) << "\t" << std::get<1>(w_val2File) << "\t"
            << std::get<2>(w_val2File) << "\t" << std::get<3>(w_val2File) << "\t" << "\n";
        }
      }
    }
    else // file exist
    {
      // open and append it
      if (w_data.open( QFile::WriteOnly | QFile::Text | QFile::Append))
      {
        QTextStream out(&w_data);
        for (int i = 0; i < w_vecCmd.size(); ++i)
        {
          tplbondata w_val2File = w_vecCmd.at(i);
          out << w_uniteON << "\t" << std::get<0>(w_val2File) << "\t" << std::get<1>(w_val2File) << "\t"
            << std::get<2>(w_val2File) << "\t" << std::get<3>(w_val2File) << "\t" << "\n";
        }
      }
    }
    w_data.close();
  }

} // End of namespace

