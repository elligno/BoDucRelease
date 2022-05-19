
// C++ includes 
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
// stl includes
#include <vector>
// boost includes
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp> // string algorithm
#include <boost/algorithm/string/split.hpp> // splitting algo
// App includes
//#include "VictoReader.h"
#include "BoDucReaderfix.h" // include VictoReader
#include "BoDucFields.h"
#include "AcrobatCsvAlgorithm.h"
//#include "Refactoring/TestAddressParserImpl.h"

//bdAPI::VictoBonLivraison::VictoBonLivraison()
// : m_fieldKey({ "No Command","Shipped to","Date promise","produit et description" })
// {
// 	m_vecOfStruct.reserve(1000); // hard code for now
// }


// Receive the BoDuc command in a vector of string which represent
// each line of the order form (converted from pdf to csv)
// Then go through each line and search for keywords
// "No Command","Shipped to","Date promise", "produit et description"
// According to those keywords, we retrieve the information associate with it.
// We set the field values of the BoDuc struct, field requested by client
// to create a report that is to be used to dispatch command to the transport
// ... allows you to manage orders, dispatching, loading, routing, ...
// Shall make use of the iterator such as transform iterator to retrive the 
// information
void bdAPI::AcrobatCsvAlgorithm::fillBoDucFields( const std::vector<std::string>& aCmdVec, 
	                                              BoDucFields& aBoDucField)
{
	// contains the original algorithm steps algorithm
	using namespace std;
	using namespace boost;
	using namespace boost::algorithm;

	// Reader support some kind of algorithm to extract data
	// In many cases data is in mess state and some work must 
	// be done to remove that extra information.
	// Reader support algorithm to do that job
#if 1
	// create a reader to retrieve field value 
	//unique_ptr<BoDucReader> w_boducReader = make_unique<VictoReader>(); original version
	unique_ptr<BoDucReader> w_boducReader = make_unique<BoDucReaderFix>(); // support algorithm for malfunctions in the Shipped To field
#else
	// new implementation of the rebuildShippedTo() method
	unique_ptr<BoDucReader> w_boducReader = make_unique<TestShippedToImpl>();
#endif
	assert(nullptr!= w_boducReader); // sanity check

	// this is a temporary fix because we have some problem 
	// with the list initializer
	vector<string> w_tmpFix = { string("No Command"),string("Shipped to"),
		string("Date promise"),string("produit et description") };

	// go through each line and look for specific key word 
	for( const std::string& w_field2Look : w_tmpFix)
	{
		// work around to take account the CSV formatting when cmd are tied together  
		if( ::strcmp(w_field2Look.c_str(), "No Command") == 0)
		{
			w_boducReader->readNoCmd(aCmdVec, aBoDucField);
			continue;
		}

		//int linenb(0); // int() does it set to 0, i think so
		short vecPos(0); //line number
		auto begVec = aCmdVec.begin();

		// go through all element cmd lines
		while( begVec != aCmdVec.cend())
		{
			const std::string& str2look = *begVec; // get line as string
			if( contains( str2look, w_field2Look.c_str())) // check for keyword
			{
				// Design Note
				// we don't really need vecPos, since we have the iterator
				// could just compute current line number by using distance
				// algorithm of the standard library.
				// Could also use the std::next(begVec,1) which return a copy
				// of the iterator. The we are ready to extract the field value.

				// then retrieve value (values are one line below, 
				// reason why we increment our iterator position) 
				std::string w_fieldValue = aCmdVec[++vecPos]; // we have the field value

				// NOTE field values comes in different format, so for each
				// of them we need to do some clean-up and ....
				// retrieve the address to be delivered to  
				if( ::strcmp("Shipped to", w_field2Look.c_str()) == 0)
				{
          std::string w_addressValue = w_fieldValue; // initialize
					
					//std::string w_completeAddress;              store part of the complete address
					std::vector<string> w_addressPart;

					do // address may take 2 cells, if more it's because of 
					{  // conversion mismatch and generally we use the last 2 lines
						trim_right_if(w_addressValue, is_any_of(","));
						w_addressPart.push_back(w_addressValue);
						//w_completeAddress += w_addressValue + std::string(" ");
						w_addressValue = aCmdVec[++vecPos];
					} while (!contains(w_addressValue, "Contact"));

          // Design Note:
          //  willbe moved in other class in the next version 
          // check for some symetry in our address
//           AddressParser w_checkAddress(w_addressPart);
//           if( w_checkAddress.hasSymmetry())
//           {
//             // do something (fix Duplicate)
//           }

					//  jean b testing some algo (symetric algorithm for sold T o and shipping To are the same)
          //  for a 4 lines symetric
          // check Symmetry 
//           if( w_addressPart.size()==4) // 4-lines
//           {
//             // sometimes address Sold To and Shipped To are concatenated
//             // both are under Shipped To (one after one). We test if it 
//             // is the case by permuting lines (last/first)
//             std::vector<std::string> w_vecPermuted; w_vecPermuted.reserve(4);
//             w_vecPermuted.emplace_back(w_addressPart[0]);
//             w_vecPermuted.emplace_back(w_addressPart[2]);
//             w_vecPermuted.emplace_back(w_addressPart[1]);
//             w_vecPermuted.emplace_back(w_addressPart[3]);
// 
//             // check duplicate (sometimes sold to and shipped to has the same address)
//             // by making them unique
//             w_vecPermuted.erase( std::unique(w_vecPermuted.begin(), w_vecPermuted.end()), w_vecPermuted.end());
// 
//             // if they are the same, we should have only line left 
//             if (w_vecPermuted.size()==2)
//             {
//               AddressParser w_buildShipAddress;
//             }
//           }

					// reader is a
					w_boducReader->readShippedTo(w_addressPart, aBoDucField);
					break;
				}

				// delivery date 
				if( ::strcmp("Date promise", w_field2Look.c_str()) == 0)
				{
					w_boducReader->readDeliveryDate(w_fieldValue, aBoDucField);
					break;
				}

				// product description
				if( ::strcmp("produit et description", w_field2Look.c_str()) == 0)
				{
					short w_tmp = vecPos;
					// for some command we don't get
					std::string w_prodDescr1 = aCmdVec[vecPos++]; // product description

				  // check if the first number is a digit
					std::locale loc;
					if (!std::isdigit( static_cast<unsigned char>(w_prodDescr1[0]), loc))
					{
						// need to do something, check for next line
						w_prodDescr1 = aCmdVec[++w_tmp]; // next line

						// we are reading the value field for product code 
						w_boducReader->readProdCode(w_prodDescr1, aBoDucField);
						
						// we are reading the value field for product description 
						w_boducReader->readProdDescr(w_prodDescr1, aBoDucField);
						
						// we are reading the value field for quantity and silo number
						auto w_lineVec = aCmdVec.cbegin() + vecPos;
						while( w_lineVec != aCmdVec.cend())
						{
							w_boducReader->readQtySilo(*w_lineVec, aBoDucField);
							if( aBoDucField.m_qty != 0.f && !aBoDucField.m_silo.empty())
							{
								break; // found it, no point to go further
							}
							++w_lineVec;
						}
					}
					break;
				}
			}//if-contains
			++begVec;
			++vecPos;
		}//while-loop
	}//for-loop
}

#if 0

void bdAPI::VictoBonLivraison::addBoDucField(const BoDucFields & aField2Add)
{
	m_vecOfStruct.push_back(aField2Add);
}

bool bdAPI::VictoBonLivraison::containsBoDucField(const BoDucFields & aField2Look)
{
	// check for a specific field with given values
	return false;
}

bool bdAPI::VictoBonLivraison::removeBoDucField(const BoDucFields & aField2)
{
	return false;
}

void bdAPI::VictoBonLivraison::getBoDucStruct( std::vector<BoDucFields>& aVec2Fill)
{
	// check size before going any further, if size is zero, then nothing top copy 

	// this is a temporary fix for now, want to have it up and running
	aVec2Fill.reserve(m_vecOfStruct.size());
	aVec2Fill = m_vecOfStruct; // use copy/assign ctor
	// shall use the move semantic , afterward m_vecOfStruct shall be empty 
	// need to be careful with that,  
	//aVec2Fill = std::move(m_vecOfStruct);

//	aVec2Fill.insert(m_vecOfStruct.cend(), m_vecOfStruct.cbegin(),m_vecOfStruct.cend());
//	std::vector<BoDucFields>::size_type w_siz = m_vecOfStruct.size(); // victo = 15
  //std::copy( m_vecOfStruct.cbegin(), m_vecOfStruct.cend(), aVec2Fill.begin());
}
#endif