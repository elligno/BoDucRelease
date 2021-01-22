#pragma once

// c++ includes
#include <vector>
#include <list>
#include <map>
// Qt include
#include <QStringList>
// app includes
#include "BoDucFields.h"
#include "BoDucCmdText.h"

// forward declaration
class QFile;

namespace bdAPI
{
  // Design Note: shall be static class??
  /** Responsible to read command file (.csv,.pdf) that contains
   * one or many command and return it in map (temporary)  
  */
  class BoDucCmdFileReader 
  {
  public:
    // some alias for clarity  
    using vecofstr = std::vector<std::string>;
    using mapIntVecstr = std::map<int, vecofstr>;
    using fileofcmd = std::map<int, QStringList>; // new version
    using vecmapstr = std::vector<BoDucCmdFileReader::mapIntVecstr>;

  public:
    // parse and fill data structure 
    static std::vector<mapIntVecstr> readFiles( const QStringList& aFilesNameWithPath ,
      const std::string& aSplitBill = std::string("Signature"));

  private:
    static mapIntVecstr readFile( const std::string& aFileAnPath,
      const std::string& aSplitBill = "Ordered on");

    // just a test for future dev
    static mapIntVecstr readFile( QFile& aFileAndPath);

    /** Read a list of files (BoDuc command files)*/
    std::vector<BoDucCmdText> readFiles( const std::forward_list<std::unique_ptr<QFile>>& aListFiles, 
      QString cmdSep = QString("Signature"));
    BoDucFileListCmdTxt readFile( const std::unique_ptr<QFile>& aFile);

    static std::string check4Accent( const std::string &aFileAnPath);
   };
} // End of namespace