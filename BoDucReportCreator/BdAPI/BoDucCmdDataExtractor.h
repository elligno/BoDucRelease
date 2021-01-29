#pragma once

// API include
#include "BoDucCmdText.h"

namespace bdAPI 
{
  /** Algorithm to extract data according to file format (csv,txt,Rcsv)*/
  class BoDucCmdDataExtractor 
  {
  public:
    std::vector<BoDucFields> extractData( const BoDucFileListCmdTxt& aCmd2extract)
    {
      // steps algo
      // first check for the file extension
      // create corresponding algo for extracting data
      //
      // Design Note
      //  Current version, BoDucBonLivraisonAlgorithm is the algo 
      // for extracting data from a command according to conversion 
      // source to destination. This class shall be renamed "BoDucDataAlgorithmExtract".
      // Structure BonLivraison abstract class and subclass is deprecated (fillBoDucFields()).
      // It has nothing to do with bon livraison creation. Its an algorithm to extract data
      // according to file format (depending on the conversion tool e.g. pdf converter), the 
      // converted file format is different (sometimes information is displayed in a specific
      // formatting that we have to take account), and this what exactly "fillBoDucField" is for.
      // Class VictoBonLivraison inherit from BoDucBonLivraisonAlgorithm, override the method 
      // for the csv format (pdf converter tool that Francois use for conversion).
      // PdfMinerAlgo is more name representative, it inherit from BoDucBonlivraisonAlgorithm
      // for the txt format obtained from pdfminertool.
      // In the re-factoring, these class will be renamed (VictoBonLivraison -> CsvDataExtract).

      // debugging purpose
      return std::vector<BoDucFields>();
    }
  };
} // End of namespace
