// C++ inc;udes 
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
//#include <windows.h>
// stl includes
#include <vector>
// boost includes
//#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp> // string algorithm
#include <boost/algorithm/string/split.hpp> // splitting algo
// App includes
//#include "VictoReader.h"
//#include "BoDucReaderfix.h" // include VictoReader
#include "PdfMinerReader.h"
#include "BoDucFields.h"
#include "PdfMinerAlgo.h"

namespace bdAPI 
{
	void PdfMinerAlgo::fillBoDucFields( const std::vector<std::string>& aCmdVec, BoDucFields& aBoDucField)
	{
		// contains the original algorithm steps algorithm
		using namespace std;
		using namespace boost;
		using namespace boost::algorithm;

		// support algorithm for malfunctions in the Shipped To field
		unique_ptr<BoDucReader> w_boducReader = make_unique<PdfMinerReader>();

		// order of these keywords is relevant for the algorithm
		vector<string> w_tmpFix = { string("Shipped to"), string("Date promise"),
			 string("Qty ordered"), string("produit et description") };

		// initialize the iterator at the beginning
		// hold the iterator position ready for next search
		auto iterPos = aCmdVec.begin();

		// go through each line and look for specific key word 
		for( const std::string& w_field2Look : w_tmpFix)
		{
			int linenb(0); // int() does it set to 0, i think so
	//		short vecPos(0); line number
			auto begVec = iterPos; //aCmdVec.begin();

			// go through all element cmd lines
			while( begVec != aCmdVec.cend())
			{
				const std::string& str2look = *begVec; // get line as string
				if( contains( str2look, w_field2Look.c_str())) // check for keyword
				{
					// we have the field value (set to Command No: format CO1234567)
			//		std::string w_fieldValue = aCmdVec[--vecPos];
					if( boost::istarts_with( *std::prev(begVec),"CO"))
					{
						aBoDucField.m_noCmd = *std::prev(begVec);
					}

					if( ::strcmp("Shipped to", w_field2Look.c_str()) == 0)
					{
					  // store part of the complete address
						std::vector<string> w_addressPart;
						auto w_posBeg = std::next(begVec);
						do // address may take 2 cells
						{  
							w_addressPart.push_back(*w_posBeg);
							std::advance(w_posBeg,1); //aCmdVec[++vecPos];
						} while( !contains(*w_posBeg, "Contact"));

						// reader is a ... to be completed
						w_boducReader->readShippedTo(w_addressPart, aBoDucField);
						iterPos = w_posBeg; // ready to continue and search for keyword
						break;
					}
					// delivery date 
					if( ::strcmp("Date promise", w_field2Look.c_str()) == 0)
					{
						auto w_fieldValue = std::next(begVec, 2);
						w_boducReader->readDeliveryDate(*w_fieldValue, aBoDucField);
						iterPos = w_fieldValue;
						break;
					}
					if (::strcmp("Qty ordered", w_field2Look.c_str()) == 0)
					{
						auto w_qty = std::next(begVec);
						std::string w_test = *w_qty;
						aBoDucField.m_qty = atof(w_test.c_str());
						w_qty += 2;
						if (boost::contains(*w_qty, "Silo"))
						{
							std::string w_silo = *(w_qty+1);
							aBoDucField.m_silo = *(w_qty + 1);
						}
						//w_boducReader->readQtySilo(*w_fieldValue, aBoDucField);
						iterPos = w_qty+1;
						break;
					}
					// product description
					if( ::strcmp("produit et description", w_field2Look.c_str()) == 0)
					{
						// for some command we don't get
						auto w_begPos = std::next(begVec+1);
						std::string w_prodCode = *w_begPos;
						std::vector<std::string> w_vecStr;
						w_vecStr.reserve(5);
						++w_begPos;
						aBoDucField.m_prodCode = static_cast<signed long>( atoi( w_prodCode.c_str()));
						do 
						{
							std::string w_desr = *w_begPos;
							w_vecStr.push_back(w_desr);
						} while( !boost::iequals(*(++w_begPos),"Vivaco"));
						if (w_vecStr.size()<w_vecStr.capacity())
						{
							w_vecStr.shrink_to_fit();
						}
						aBoDucField.m_produit = w_vecStr[0] + " " + w_vecStr[1];
						//std::string w_prodDescr1 = *std::next(begVec); //aCmdVec[vecPos++]; // product description
						break;
					}
				}
				++begVec;
	//			++vecPos;
			}
		}
	}

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

} // End of namespace

