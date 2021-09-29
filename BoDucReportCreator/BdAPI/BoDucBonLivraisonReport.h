#pragma once

// Qt includes
#include <QTextStream>
#include <QFile>
#include <QDir>
// App include
#include "BoDucBonLivraison.h"

namespace bdAPI {

  class BonLivraisonReport 
  {
  public:
    BonLivraisonReport(): m_dirName(), m_nameSuffix("_BonLivraison.txt") {}

    // method to create Bon livraison
    void createReport( const BoDucBonLivraison& aBon2Create);
    void setFolderName( const QDir& aDirName) { m_dirName = aDirName; }

  private:
    QDir m_dirName;
    QFile m_bonReportFile;  // ???
    QString m_nameSuffix;  // last part of the file name 
  };
} // End of namespace