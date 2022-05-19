
// C++ include
#include <iostream>
// Qt include
#include <QFileInfo>
// boost includes
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp> // string algorithm
#include <boost/algorithm/string/split.hpp> // splitting algo
// App includes
#include "BoDucParser.h"
#include "BoDucFields.h"
#include "PdfMinerAlgo.h"
#include "AcrobatCsvAlgorithm.h"
#include "RCsvAlgorithm.h"

namespace bdAPI 
{
  BoDucParser::BoDucParser( const eFileType aFilext /*= eFileType::csv*/)
 : m_fileExt(aFilext),
   m_fieldParserAlgo(nullptr)
  {
    m_bdStruct.reserve(50);
  }

  BoDucParser::BoDucParser( ExtractDataAlgorithm * aReader)
  : m_fileExt(eFileType::csv),
    m_fieldParserAlgo(aReader)
	{
    m_fileExt = eFileType::csv;
		m_bdStruct.reserve(50); // default 
	}

#if 0

  // deprecated
	void BoDucParser::extractData( const mapIntVecstr& aListOfCmd,
		BoDucBonLivraisonAlgorithm* aBonLivraisonAlgo)
	{
		auto w_begMap = aListOfCmd.cbegin();
		while( w_begMap != aListOfCmd.cend()) // process each cmd parsed from csv file
		{
			vecofstr w_cmd2Proceed = w_begMap->second;

      // when converting from pdf to txt, it introduce empty string element
      // delete all empty string element to be able to parse with our algorithm
      if( std::any_of( w_cmd2Proceed.cbegin(), w_cmd2Proceed.cend(), [](const std::string& aStr) { return aStr.empty(); }))
      {
        w_cmd2Proceed.erase( std::remove_if(w_cmd2Proceed.begin(), w_cmd2Proceed.end(), // delete empty element
          [] ( const std::string& s) // check if an empty string
        {
          return s.empty();
        }), w_cmd2Proceed.cend());
      }

      if (!std::any_of(w_cmd2Proceed.cbegin(), w_cmd2Proceed.cend(),
        [](const std::string& aStr)
      {
        // check for a valid
        return boost::contains(aStr, std::string("TM")) || boost::contains(aStr, std::string("TON"));
      }))
      {
        ++w_begMap; // next in the list
        continue;   //"TM" or "TON" not used in this command (not valid)
      }

      // filling the BoDuc struct	
      BoDucFields w_boducStruct;
      if (aBonLivraisonAlgo != nullptr)
      {
        aBonLivraisonAlgo->fillBoDucFields(w_cmd2Proceed, w_boducStruct);
      }
      // deprecated
      aBonLivraisonAlgo->addBoDucField(w_boducStruct);
      ++w_begMap; // next in the list
		}//while-loop
	}

#endif

  BoDucFields BoDucParser::extractData( const std::vector<std::string>& aTextCmd)
	{
    using namespace std;
    using BoDucDataAlgo = unique_ptr<ExtractDataAlgorithm>;

    BoDucDataAlgo w_bdataAlgo = nullptr;  //make_unique<VictoBonLivraison>();
    if( eFileType::csv == getFileExt())
    {
      w_bdataAlgo.reset( new AcrobatCsvAlgorithm());
    }
    else if(eFileType::rcsv == getFileExt())
    {
      w_bdataAlgo.reset( new RCsvAlgorithm());
    }
    else if( eFileType::pdf == getFileExt())
    {
      w_bdataAlgo.reset( new PdfMinerAlgo());
    }
    else
    {
      // Shall throw an exception or return an error message
      // NOTE debugging purpose, i do not think its a good 
      // practice to return default ctor
      return BoDucFields(); // or set a default algo
    }

    // filling the BoDuc struct	
    BoDucFields w_boducStruct;
    if( w_bdataAlgo != nullptr)
    {
      w_bdataAlgo->fillBoDucFields(aTextCmd, w_boducStruct);
    }

    return w_boducStruct;
	}

  // TODO: must add support to R csv file format (RCsvAlgorithm)
  QBoDucFields BoDucParser::extractData( const BoDucCmdText & aCmdTxt)
  {
    using namespace std;
    using BoDucDataAlgo = unique_ptr<ExtractDataAlgorithm>;

    BoDucDataAlgo w_bdataAlgo = nullptr;  //make_unique<VictoBonLivraison>();
    if (eFileType::csv == getFileExt())
    {
      w_bdataAlgo.reset( new AcrobatCsvAlgorithm());
    }
    else if (eFileType::rcsv == getFileExt())
    {
      w_bdataAlgo.reset( new RCsvAlgorithm());
    }
    else if (eFileType::pdf == getFileExt())
    {
      w_bdataAlgo.reset( new PdfMinerAlgo());
    }
    else
    {
      // Shall throw an exception or return an error message
      // NOTE debugging purpose, i do not think its a good 
      // practice to return default ctor
      return QBoDucFields {}; // or set a default algo
    }

    // filling the BoDuc struct	
    QBoDucFields w_boducStruct;
    if( w_bdataAlgo != nullptr)
    {
      w_boducStruct = w_bdataAlgo->fillBoDucFields( aCmdTxt);
    }

    return w_boducStruct;  // QBoDucFields{};
  }

	bool BoDucParser::useTM( const std::vector<std::string>& aVecOfCmdLines)
	{
		using namespace std;
		using namespace boost;
		using namespace boost::algorithm;

		bool retTM = false;
		auto lineIter = aVecOfCmdLines.cbegin();
		do
		{
			lineIter++;
		} while( !contains(*lineIter, std::string("produit et description")));

		using dist = iterator_traits<std::string::iterator>::difference_type;
		dist w_startStr = distance(aVecOfCmdLines.cbegin(), lineIter);
		auto begVec = aVecOfCmdLines.cbegin() + w_startStr;
		while (begVec != aVecOfCmdLines.cend())
		{
			if( contains( *begVec, std::string("TM")) || contains(*begVec, std::string("TON")))
			{
				retTM = true;
				break;
			}
			++begVec;
		}

		return retTM;
	}

  // REMARK some file has dot in their name, user must remove these
  // before processing because unexpected behavior
  bool BoDucParser::isFileExtOk( const QStringList& aListOfiles) const
  {
    eFileType w_check = getFileExt(); // debug purpose
    // 
    QString w_fileExt = "csv"; // as default
    if( getFileExt() == eFileType::csv)
    {
      w_fileExt = "csv";
    }
    else if (getFileExt() == BoDucParser::eFileType::pdf)
    {
      w_fileExt = "pdf";
    }
    else
    {
      throw "Not a valid file extension"; // not sure about this one
    }
     
    /*bool w_checkFileExt =*/ 
    return std::all_of(aListOfiles.constBegin(), aListOfiles.constEnd(),
      [w_fileExt](const QString& aFileName)
    {
      QFileInfo w_file2Look(aFileName);
      QString w_fExt = w_file2Look.completeSuffix();
      return (w_fExt.compare(w_fileExt) == 0); // equal (set to txt for debugging purpose, but it should be pdf)
    });
  }

  bool BoDucParser::hasAnyTM_TON( const std::vector<std::string>& aCmdText)
  {
    if (getFileExt() == BoDucParser::eFileType::pdf)
    {
      // need to parse the whole file, format not the same as 
      auto findUM = std::find_if(aCmdText.cbegin(), aCmdText.cend(), [](const std::string& aStr2Find)
      {
        return boost::contains(aStr2Find, std::string("U/M"));
      });

      if( findUM!= aCmdText.cend()) // didn't reach the end, found something
      {
        auto w_lookAt = *std::next(findUM, 1);
        if( (::strcmp( w_lookAt.data(), "TM")) == 0 || (::strcmp(w_lookAt.data(), "TON")) == 0)
        {
          return true;
        }
        else
        {
          // last check because for some file "TON" is moved somewhere at the end of the file
          // let's check if this the case here
          auto foundTON = std::find_if(findUM, aCmdText.cend(), [](const std::string& aStr2Find)
          {
            return boost::contains(aStr2Find, std::string("TON"));
          });
          if( foundTON != aCmdText.cend())
          {
            return true;
          }
          return false;
        }
      }
    }
//     auto w_begMap = aListOfCmd.cbegin();
//     while (w_begMap != aListOfCmd.cend()) // process each cmd parsed from csv file
//     {
//       vecofstr w_cmd2Proceed = w_begMap->second;

    // this part shall be done outside, we pass a const object and trying to modify it
    // blank space shall be remove before and pass the trimmed vector 
      // when converting from pdf to txt, it introduce empty string element
      // delete all empty string element to be able to parse with our algorithm
//       if( std::any_of( aCmdText.cbegin(), aCmdText.cend(), [](const std::string& aStr) { return aStr.empty(); }))
//       {
//         aCmdText.erase( std::remove_if( aCmdText.begin(), aCmdText.end(), // delete empty element
//           []( const std::string& s) // check if an empty string
//         {
//           return s.empty();
//         }), aCmdText.cend());
//       }

    // first locate "produit et description string" and then start searching 
    //  for those 2 "TM" and "TON" 
    // problem that we had we were searching the whole content, but the string "TON"
    // is contained in "BROMPTON" for example. We need to narrow our search.
    // These two character string are located near product description, but this is the case
    // with csv file converted by pdf tool (Francois). But if you look in txt file, those 
    // obtained with pdfminer conversion its not the case.   
    auto findProdDescr = std::find_if( aCmdText.cbegin(), aCmdText.cend(), [](const std::string& aStr2Find)
    { 
      return boost::contains(aStr2Find, std::string("produit et description"));
    });

    auto distFromBeg   = std::distance(aCmdText.cbegin(), findProdDescr);
    auto startSearch   = std::next(aCmdText.begin(), distFromBeg);

    return( std::any_of( startSearch, aCmdText.cend(),
      []( const std::string& aStr)
      {
        // check for a valid
        return boost::contains(aStr, std::string("TM")) || boost::contains(aStr, std::string("TON"));
      }));

//       {
//         ++w_begMap; // next in the list
//         continue;   //"TM" or "TON" not used in this command (not valid)
//       }
 //    }

   // return false;
  }

} // End of namespace

