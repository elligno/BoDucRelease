// App include
#include "PdfMinerReader.h"
// Boost include
#include <boost/algorithm/string.hpp> // string algorithm

namespace bdAPI 
{

  void PdfMinerReader::readNoCmd(const std::string& COline, BoDucFields& aBoDucField)
  {
    //std::string COline = *w_foundCmdNo;
    auto firstO = std::find_if(COline.cbegin(), COline.cend(), [](unsigned char c)
    {
      return c == 'O';
    });
    if (firstO != COline.cend())
    {
    }
    auto w_isDigit = std::all_of(firstO + 1, COline.cend(), [](unsigned char c)
    {
      return std::isdigit(c, std::locale());
    });

    if (true == w_isDigit)
    {
      aBoDucField.m_noCmd = COline; //*std::prev(begVec);
    }
  }

  void PdfMinerReader::readShippedTo(const vecofstr& aFieldValue, BoDucFields& aBoducF)
	{
		// concatenate the the string to build the whole address and set the BoDuc field
		std::string w_wholeAddress;
		for(const auto& val : aFieldValue)
		{
			if( boost::contains(val,"*** COVILAC***"))
			{
				continue;
			}
			w_wholeAddress += val + " ";
		}
		// set boduc stuff
		aBoducF.m_deliverTo = w_wholeAddress;
	}

	void PdfMinerReader::readDeliveryDate( const std::string& aFiedValue, BoDucFields& aBoducF)
	{
		aBoducF.m_datePromise = aFiedValue;
	}

  void PdfMinerReader::readProdDescr( const std::vector<std::string>& aCmdVec, BoDucFields& aBoDucField, std::string aStopWordCriteria /*= "Notes"*/)
  {
    auto begVec = aCmdVec.cbegin();

    // for some command we don't get
    auto w_begPos = std::next(begVec + 1);
    std::string w_prodCode = *w_begPos;
    std::vector<std::string> w_vecStr;
    w_vecStr.reserve(5);
    ++w_begPos;
    aBoDucField.m_prodCode = static_cast<signed long>(atoi(w_prodCode.c_str()));
    do
    {
      std::string w_desr = *w_begPos;
      w_vecStr.push_back(w_desr);
    } while( !boost::equals( *(++w_begPos), "Vivaco")); // not sure about this, pre-incr?? check first and then incr? post-incr
    if (w_vecStr.size() < w_vecStr.capacity())          // "iequals" is case insensitive, equals is case sensitive (could use iequals??)
    {
      w_vecStr.shrink_to_fit();
    }
    aBoDucField.m_produit = w_vecStr[0] + " " + w_vecStr[1];
  }

  void PdfMinerReader::readProdDescr( const BoDucProDescrArea& aProdescrArea, BoDucFields& aBoDucField)
  {
    // Alias
    using AreaPattern = BoDucProDescrAnalyzer::eAreaPattern;

    // algorithm to retrieve (don't really need it, nmay delete it in the next version)
    auto begVec = aProdescrArea.begin();

    // retrieve product code (always 1 line below rhe string "prod et descr")
    auto w_begPos = std::next( begVec,1);  //tmp for debug purpose
    std::string w_tst = std::string(*w_begPos);
    boost::trim_left(w_tst);

    if( w_tst == "1")
    {
      std::string w_prodCode = *std::next(begVec,2);
      aBoDucField.m_prodCode = static_cast<signed long>(::atoi(w_prodCode.c_str()));
    }
    else
    {
      w_begPos -= 1;
      aBoDucField.m_prodCode = static_cast<signed long>(::atoi((*w_begPos).c_str()));
    }

 //   std::string w_prodCode = *std::next(begVec + 1);
 //   aBoDucField.m_prodCode = static_cast<signed long>( ::atoi( w_prodCode.c_str()));

    // start to analyze the area to find proper algorithm
    BoDucProDescrAnalyzer w_proDescrAnalyzer;
    AreaPattern w_patrn = w_proDescrAnalyzer.startCriteriaPattern( aProdescrArea);
    auto w_vecAdressLine = w_proDescrAnalyzer.productAndDescription( aProdescrArea,w_patrn); // vector of address line 

    // check size of vector address (concatenate)
    if( w_vecAdressLine.size() == 1)
    {
      // just take it as it is
      aBoDucField.m_produit = w_vecAdressLine.front(); // only one line
    }
    else // can have at most 2 lines
    {
      // concatenate string
      aBoDucField.m_produit = w_vecAdressLine[0] + " " + w_vecAdressLine[1];
    }
  }
} // End of namespace

