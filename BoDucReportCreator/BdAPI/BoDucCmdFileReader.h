#pragma once

// c++ includes
#include <vector>
#include <list>
#include <map>
// Qt include
#include <QStringList>
// app include
#include "BoDucFields.h"

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
    static std::vector<mapIntVecstr> readFiles( const std::list<std::string>& aFilesNameWithPath,
      const std::string& aSplitBill = "Ordered on");

  private:
    static mapIntVecstr readFile( const std::string& aFileAnPath,
      const std::string& aSplitBill = "Ordered on");

    // just a test for future dev
    static mapIntVecstr readFile( QFile& aFileAndPath);

    static std::string check4Accent( const std::string &aFileAnPath);

  };
} // End of namespace