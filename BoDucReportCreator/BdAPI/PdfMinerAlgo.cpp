// C++ inc;udes 
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
//#include <windows.h>
// stl includes
#include <vector>
// boost includes
#include <boost/algorithm/string.hpp> // string algorithm
#include <boost/algorithm/string/split.hpp> // splitting algo
// App includes
//#include "VictoReader.h"
//#include "BoDucReaderfix.h" // include VictoReader
#include "PdfMinerReader.h"
#include "AddressParser.h"
#include "PdfMinerAlgo.h"
#include "BoDucProDescrAnalyzer.hpp"

namespace bdAPI 
{
	void PdfMinerAlgo::fillBoDucFields( const std::vector<std::string>& aCmdVec, BoDucFields& aBoDucField)
	{
		// contains the original algorithm steps algorithm
		using namespace std;
		using namespace boost;
		using namespace boost::algorithm;

		// support algorithm for malfunctions in the Shipped To field
		unique_ptr<PdfMinerReader> w_boducReader = make_unique<PdfMinerReader>();

		// order of these keywords is relevant for the algorithm
		vector<string> w_tmpFix = { string("Shipped to"), string("Date promise"),
			 string("Qty ordered"), string("produit et description") };

    // some initialization
    auto w_keyWordIter = w_tmpFix.begin();

    // check first line contains a string start with "CO"
    auto w_foundCmdNo = std::find_if( aCmdVec.cbegin(), aCmdVec.cend(),
      []( const std::string& aStr)
    {
      return boost::starts_with(aStr, "CO");
    });

    // this algorithm is not complete, it still under construction
    // some case need to be considered
    if( w_foundCmdNo != aCmdVec.cend())
    {
      w_boducReader->readNoCmd( *w_foundCmdNo, aBoDucField);

      // next look for "shipped to" which can be in different layout
      // first check if next line contains "Shipped To", if so 
      // the shipped address is below (to contact)
      // otherwise take all lines to contact and split in half
      // last part is the "shipped to" address
      // after that set begin iterator to contact lines

      // store part of the complete address
      std::vector<string> w_addressArea;
      auto w_begArea = std::next(w_foundCmdNo, 1);
      do // address may take 2 cells
      {
        if (!contains(*w_begArea, *w_keyWordIter) &&
          !contains(*w_begArea, std::string("Sold to")))
        {
          w_addressArea.push_back(*w_begArea);
        }
        std::advance(w_begArea, 1); //aCmdVec[++vecPos];
      } while (!contains(*w_begArea, "Contact"));

      // checking address format and return "shipped to" 
      AddressParser w_checkAddress(w_addressArea);
      AddressParser::ePattern w_addrsPattern = w_checkAddress.getPattern(); // debugging purpose
      std::vector<std::string> w_shippedAdrs = w_checkAddress.getShippedAddress();
      w_boducReader->readShippedTo(w_shippedAdrs, aBoDucField);
    }// if

    // next keyword
    std::advance( w_keyWordIter, 1); // next token should be "Date promise"

    // initialize the iterator at the beginning
		// hold the iterator position ready for next search
    //auto iterPos = w_foundCmdNo; aCmdVec.begin();

    auto begVec = w_foundCmdNo;
		// go through each line and look for specific key word 
// 		for( const std::string& w_field2Look : w_tmpFix)
// 		{
			//int linenb(0);  int() does it set to 0, i think so
	//		short vecPos(0); line number
			//auto begVec = iterPos; aCmdVec.begin();

			// go through all element cmd lines
			while( begVec != aCmdVec.cend())
			{
				const std::string& str2look = *begVec; // get line as string
				if( contains( str2look, w_keyWordIter->c_str())) // check for keyword
				{
//           if( ::strcmp("Shipped to", w_field2Look.c_str()) == 0)
// 					{
// 					  // store part of the complete address
// 						std::vector<string> w_addressPart;
// 						auto w_posBeg = std::next(begVec);
// 						do // address may take 2 cells
// 						{  
// 							w_addressPart.push_back(*w_posBeg);
// 							std::advance(w_posBeg,1); //aCmdVec[++vecPos];
// 						} while( !contains(*w_posBeg, "Contact"));
// 
// 						// reader is a ... to be completed
// 						w_boducReader->readShippedTo(w_addressPart, aBoDucField);
// 						iterPos = w_posBeg; // ready to continue and search for keyword
// 						break;
// 					}
					// delivery date 
					if( ::strcmp("Date promise", w_keyWordIter->c_str()) == 0)
					{
            // date is 2-lines below founded keyword, the add 2 to the iterator
						auto w_fieldValue = std::next(begVec, 2);
						w_boducReader->readDeliveryDate(*w_fieldValue, aBoDucField);
						//iterPos = w_fieldValue;
            std::advance(begVec,1); // next command line
            std::advance(w_keyWordIter,1); // next token to look for
            continue; //break; next line
					}
					if( ::strcmp( "Qty ordered", w_keyWordIter->c_str()) == 0)
					{
            // format of qty ordered and silo are always the same
					//	auto w_qty = std::next(begVec);
            std::string w_asd = *std::next(begVec, 1);
            boost::trim_left(w_asd);
           // std::string w_test = w_asd;

          //  if( std::all_of( w_test.cbegin(), w_test.cend(), [](unsigned char c)
          //  {

            // 
            if( std::isdigit( w_asd[0], std::locale()))
           // }))
            {
               aBoDucField.m_qty = ::atof(( *std::next(begVec,1)).c_str());
               aBoDucField.m_silo = *std::next(begVec, 4);
               std::advance( begVec, 1); // next command line
               std::advance( w_keyWordIter, 1); // next token to look for
               continue;
            }
            else // take that the next 
            {
              // case where qty ordered and silo inside "prod et descr" area (blais facture)
              // we add 4 lines qty,fab,silo
              std::vector<std::string> w_prdoDescr( std::next(begVec, 4), aCmdVec.cend()); // cmd number usually 1
              
              // call prodDecription algo or parser/reader
              w_boducReader->readProdDescr( w_prdoDescr, aBoDucField); // as it use to be in csv format
              break;
            }
// 						w_qty += 2;
// 						if( boost::contains(*w_qty, "Silo"))
// 						{
// 							std::string w_silo = *(w_qty+1);
            //aBoDucField.m_silo = *std::next(begVec, 4); // *(w_qty + 1);
// 						}
					//	w_boducReader->readQtySilo( *std::next(begVec,1), aBoDucField);
						//iterPos = w_qty+1; ??
						//break;

            // What for?? we will never get here since else case above takes care 
            // of all other situation
           // ++begVec;  next command line
            //++w_keyWordIter;  next token to look for
           // continue;
					}
					// product description
          if( ::strcmp( "produit et description", w_keyWordIter->c_str()) == 0)
					{
            BoDucProDescrArea w_prodescrArea( std::vector<std::string>(begVec, begVec + 10));
            w_boducReader->readProdDescr(w_prodescrArea, aBoDucField);
						break; // we are at the end of tokens list
					}
				}
				++begVec;
	//			++vecPos;
			}//while-loop
//		}

      if (aBoDucField.m_produit.empty())
      {
        // must scan again
        begVec = w_foundCmdNo;
        while (begVec != aCmdVec.cend())
        {
          const std::string& str2look = *begVec; // get line as string
          if( contains(str2look, "produit et description")) // check for keyword
          {
     //       if (::strcmp("produit et description", w_keyWordIter->c_str()) == 0)
      //      {
              BoDucProDescrArea w_prodescrArea(std::vector<std::string>(begVec, begVec + 10));
              w_boducReader->readProdDescr(w_prodescrArea, aBoDucField);
              break; // we are at the end of tokens list
          }//if
          ++begVec;
        }//while-loop
      }
	}

#if 0

	void PdfMinerAlgo::addBoDucField(const BoDucFields& aField2Add)
	{
		m_vecOfStruct.push_back(aField2Add);
	}

	bool PdfMinerAlgo::containsBoDucField(const BoDucFields & aField2Look)
	{
		return false;
	}

	bool PdfMinerAlgo::removeBoDucField(const BoDucFields & aField2)
	{
		return false;
	}
	// need a re-factoring
	void PdfMinerAlgo::getBoDucStruct(std::vector<BoDucFields>& aVec2Fill)
	{
		aVec2Fill.reserve(m_vecOfStruct.size());
		aVec2Fill = m_vecOfStruct;
	}

	size_t PdfMinerAlgo::getnbOfCmd()
	{
		return size_t();
	}

#endif

} // End of namespace

