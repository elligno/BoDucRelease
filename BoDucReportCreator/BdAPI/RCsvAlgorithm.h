#pragma once

// Application includes
#include "ExtractDataAlgorithm.h"

namespace bdAPI 
{
  /** Extract data from Csv file format (converted with R pdf tool).
   *
   *   Csv file format are different according the tool used to
   *   convert from pdf file. BoDuc report creator use third-party
   *   tool to convert file in text format such as csv and then 
   *   extract data (data mining) extraction. R pdf_tool is one of 
   *   tool that convert pdf to csv in a clean format easy to read.
  */
  class RCsvAlgorithm : public ExtractDataAlgorithm 
  {
  public:
    /** return the BoDucFields ... to be completed*/
    void fillBoDucFields( const std::vector<std::string>& aCmdVec, BoDucFields& aBoDucField) override;

    /** Returns a BoDucField struct (fill the data structure) read from a text command format*/
    QBoDucFields fillBoDucFields( const BoDucCmdText& aCmdVec) override;
  };
} // End of namespace
