// C++ includes 
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
	// stl includes
#include <map>
#include <vector>
// boost includes
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>       // string algorithm
#include <boost/algorithm/string/split.hpp> // splitting algo
#include <boost/algorithm/string/predicate.hpp>
	// app include
#include "VictoReader.h"
#include "BoDucUtility.h"

	namespace bdAPI
	{
		VictoReader::VictoReader()
		{
			std::cout << "Nothing to do\n";
		}

		VictoReader::~VictoReader()
		{
			std::cout << "Nothing to do\n";
		}

		//
		// Services that are needed to fill the BoDuc field structure
		//
		void VictoReader::readNoCmd(const vecofstr& aCmdVec, BoDucFields& aBoducF)
		{
			using namespace std;
			using namespace boost;
			using namespace boost::algorithm;

			short linenb(0);
			// need to check for "Date" first of (pair Date,No)
			for (const string& w_checkStr : aCmdVec)
			{
				// looking for "Date" then break
				if (contains(w_checkStr, "Date"))
				{
					linenb++; // line below which contains the command number
					break;
				}
				linenb++;
			}

			// string that should be pass to this method
			string w_fieldValue = aCmdVec[linenb];
			trim_right_if( w_fieldValue, is_any_of(","));
			// splitting string to retrieve COxxxxx cmd no
			vector<string> strs;
			split( strs, w_fieldValue, is_any_of(","));

			// not sure about this one, after this we should have a vector of 2 elements
			// because our search is based on a line that contains a pair (Date,No)
			strs.erase( remove_if(strs.begin(), strs.end(), // delete empty element
				[](const string& s)
			{ return s.empty(); }), strs.cend());

			// boost::starts_with algorithm
			if( starts_with(strs.back(), string("CO")))
			{
				aBoducF.m_noCmd = strs.back(); //assume that the last field is the right one
			}
			else
			{
				// check next line (Fuchs case where line before contains a Date keyword also)
				linenb++;
				string w_fieldValue = aCmdVec[linenb];
				readNoCmd( w_fieldValue, aBoducF);
				if( aBoducF.m_noCmd.empty())
				{
					// we didn't read cmd no correctly, must do some action 
					aBoducF.m_noCmd = "xxxxxxxx";
				}
			}
		}

		void VictoReader::readNoCmd( const std::string& aFieldValue, BoDucFields & aBoducF)
		{
			using namespace std;
			using namespace boost;
			using namespace boost::algorithm;

			std::vector<std::string> w_dateSplitted =
				BoDucUtility::TrimRightCommaAndErase(aFieldValue);

			// boost::starts_with algorithm
			if( starts_with(w_dateSplitted.back(), string("CO")))
			{
				aBoducF.m_noCmd = w_dateSplitted.back(); //assume that the last field is the right one
			}
		}

		void VictoReader::readShippedTo(const vecofstr& aFieldValue, BoDucFields& aBoducF)
		{
			using namespace std;
			using namespace boost;
			using namespace boost::algorithm;
#if 0
			std::string w_completeAddress;     // store part of the complete address
			if( aFieldValue.size() > 2)
			{
				// we need to take the last 2 in most cases
				using riter = std::reverse_iterator<std::vector<std::string>::const_iterator>;
				riter w_lastPart = aFieldValue.crbegin() + 1;
				w_completeAddress += *w_lastPart + std::string(" ");
				w_lastPart = aFieldValue.rbegin();
				w_completeAddress += *w_lastPart;
				aBoducF.m_deliverTo = w_completeAddress; // assume that the last field is the right one 
			}
			else if (aFieldValue.size() == 2)
			{
				// rebuild the whole address
				w_completeAddress += aFieldValue[0] + std::string(" ");
				w_completeAddress += aFieldValue[1];
				aBoducF.m_deliverTo = w_completeAddress; // assume that the last field is the right one 
			}
			else if (aFieldValue.size() == 1)
			{
				w_completeAddress += aFieldValue[0];
				aBoducF.m_deliverTo = w_completeAddress; // assume that the last field is the right one
			}
			else
			{
				std::cout << "There is a problem with Customer address\n";
			}
#endif
			// fill complete address
			aBoducF.m_deliverTo = rebuildShippedTo(aFieldValue);
		}

		std::string VictoReader::rebuildShippedTo( const vecofstr& aAddressPart)
		{
			std::string w_rebuildStr;

			// NOTE case ==4 and >4, this situation arise for the first 2 element
			// of the billing file (probably because of the conversion from Excell to csv).
			// The former, we fix it by taking the last 2 lines of the complete address.
			// We consider that since we have a kind of pairing 2-2, its jsut a duplicate
			// Then we take the last 2 since its probably the "Shipped to" address. 
			// The latter need more attention, mangling addresses between "Sold To" and "Shipped To" 
			if( aAddressPart.size() > 4) // case 5 (to be completed)
			{
				if( aAddressPart.size() == 6)
				{
					// shall check for symmetry part in the address
					w_rebuildStr += aAddressPart[3] + std::string(" ");
					w_rebuildStr += aAddressPart[4] + std::string(" ");
					w_rebuildStr += aAddressPart[5];
					return w_rebuildStr;
				}

				// we need to take the last 2 in most cases
				using criter = std::reverse_iterator<std::vector<std::string>::const_iterator>;
				criter w_lastPart = aAddressPart.crbegin() + 1; // start one before last
				auto w_start = aAddressPart.cbegin();
				w_rebuildStr += *w_start; // should be the client name, but not sure
				w_rebuildStr += *w_lastPart + std::string(" ");
				w_lastPart = aAddressPart.crbegin();
				w_rebuildStr += *w_lastPart;
			}
			else if( aAddressPart.size() == 4)
			{
				// we need to take the last 2 in most cases
				// symetry, 
				using criter = std::reverse_iterator<std::vector<std::string>::const_iterator>;
				criter w_lastPart = aAddressPart.crbegin() + 1;
				w_rebuildStr += *w_lastPart + std::string(" ");
				w_lastPart = aAddressPart.crbegin();
				w_rebuildStr += *w_lastPart;
				return w_rebuildStr;
			}
			else if( aAddressPart.size() == 3) // need to add a test
			{
				// rebuild the whole address humm!! not sure
	//			w_rebuildStr += aAddressPart[0] + std::string(" ");
				w_rebuildStr += aAddressPart[1] + std::string(" ");
				w_rebuildStr += aAddressPart[2];
			}
			else if( aAddressPart.size() == 2) // 2-lines address
			{
				// rebuild the whole address
				// NOTE we have seen some cases (most of the times first address mixed up)
				// some stuff from "Sold To" inserted in "Shipped To" when converting pdf to csv
				// I think we must do some pre-treatment for the last line and check fields
				// city, province and postal code no more than that
				// split the last line with token set to comma, we should get these fields
				// if we have size > 3, some action must be taken
				std::vector<std::string> w_addressFields;
				boost::split(w_addressFields, aAddressPart[1], boost::is_any_of(","));
				if (w_addressFields.size()>3)
				{
					// do something at leat check if size==6 
					if( w_addressFields.size()==5) // number of elements in the last lines, duplicate city, prov and postal code
					{                              // some coming from "Sold To" that have been inserted here
						std::vector<std::string> w_shippedCity;
						boost::split(w_shippedCity, w_addressFields[2], boost::is_any_of(" "));
						w_shippedCity.erase(std::unique(w_shippedCity.begin(), w_shippedCity.end()), w_shippedCity.end());
						auto w_remEmpty = [](const std::string& aChar2Check) -> bool {if (aChar2Check == "") { return true; } else { return false; }};
						w_shippedCity.erase(std::remove_if( w_shippedCity.begin(), w_shippedCity.end(), w_remEmpty), w_shippedCity.end());
						// check if the first 2 element is the postal code
						auto checkDigit = []( const char aChar2Check) -> bool {if( std::isdigit(aChar2Check,std::locale())) { return true; } else { return false; }};
						if( std::find_if(w_shippedCity[1].cbegin(), w_shippedCity[1].cend(),checkDigit) != w_shippedCity[1].cend())
						{
							std::cout << "Found a digit it must be the postal code\n";
							w_rebuildStr += aAddressPart[0] + std::string(" ");
							w_rebuildStr += w_shippedCity[2] + std::string(" ");
						}
						// last element shall be the shipped city
						//w_rebuildStr += w_addressFields[2] + std::string(" "); debugging purpose put in comment
						w_rebuildStr += w_addressFields[3] + std::string(" ");
						w_rebuildStr += w_addressFields[4];
					}
					else
					{
						w_rebuildStr += aAddressPart[0] + std::string(" ");
						w_rebuildStr += aAddressPart[1];
						// just don't know what to do
						std::cerr << "Shipped address case don't know what to do\n";
					}
				}
				else
				{
					w_rebuildStr += aAddressPart[0] + std::string(" ");
					w_rebuildStr += aAddressPart[1];
				}
			}
			else if( aAddressPart.size() == 1)
			{
				// no special treatment, take it as it is
				w_rebuildStr += aAddressPart[0];
			}
			return w_rebuildStr;
		}

		size_t VictoReader::nbOfCmd(const std::string& aCmdFile) const
		{
			size_t w_nbCmd = 0;
			std::ifstream w_readCSV(aCmdFile.c_str());
			if (w_readCSV) // check if its open, then read line-by-line 
			{
				for (std::string line; std::getline(w_readCSV, line);)
				{
					// check if contains "Signature"
					if (boost::algorithm::contains(line, std::string("Signature")))
					{
						++w_nbCmd;
					}
				}//for-loop
			}

			return w_nbCmd;
		}

		void VictoReader::readDeliveryDate(const std::string& aFieldValue, BoDucFields& aBoducF)
		{
			using namespace std;
			using namespace boost;
			using namespace boost::algorithm;

			// splitting string to retrieve ... to be completed
			vector<string> strs;
			split(strs, aFieldValue, is_any_of(","));

			// shall use the auto=lambda to define the lambda function
			strs.erase(remove_if(strs.begin(), strs.end(), // delete empty element
				[](const string& s)
			{ return s.empty(); }));

			strs.erase(unique(strs.begin(), strs.end()), strs.end());
			//		vector<string> w_datepromise;
			//boost::split(w_datepromise, strs[1], boost::is_any_of(" ")); // need to be split

			// new stuff we look for string of this type: 2017/10/23     00:00 or 2017/10/23 
			for (std::string w_str2LookAt : strs)
			{
				//w_str2LookAt.find_first_of("/");
				if (w_str2LookAt.find_first_of("/") != std::string::npos) // found it?
				{
					vector<string> w_datepromise;
					// we found something, its probably the date format xxxx/xx/xx
					split(w_datepromise, w_str2LookAt, is_any_of(" ")); // need to be split
					aBoducF.m_datePromise = w_datepromise[0];
					break; //get out since we found it 
				}
			}

			// sometimes date format is different, not separated by back slash
			// but by dash symbol. We need to check for this if string empty
			// if the field is empty, check for dash separated to check date format 
			if( aBoducF.m_datePromise.empty())
			{
				// new stuff we look for string of this type: 2017/10/23     00:00 or 2017/10/23 
				for( std::string w_str2LookAt : strs)
				{
					//w_str2LookAt.find_first_of("/");
					if( w_str2LookAt.find_first_of("-") != std::string::npos) // found it?
					{
						vector<string> w_datepromise;
						// we found something, its probably the date format xxxx/xx/xx
						split(w_datepromise, w_str2LookAt, is_any_of(" ")); // need to be split
						aBoducF.m_datePromise = w_datepromise[0];
						break; //get out since we found it 
					}
				}
			}

			// Most of the cases date and time is represented by the same string.
			// In some rare case, there are split a part but in both case date is
			// the second element. The case which date and time same string, the 
			// split a vector with many empty elements, otherwise only one element.
			// After the split end up with date as first element of split result vector.  
			// We decide to assume the relevant information  
			//		split(w_datepromise, strs[1/*strs.size() - 3*/], is_any_of(" ")); // need to be split
			//		aBoducF.m_datePromise = w_datepromise[0];
		}

		void VictoReader::readProdDescr( const std::string& aFieldValue, BoDucFields& aBoducF)
		{
			using namespace std;
			using namespace boost;

			// Algorithm Steps 
			// Based on search of "Silo" which is the last element of the string 
			// 1. first position to the string that is under "Silo"
			// 2. look for first element that is empty (maybe use boost find algorithm)
			// which use the iterator range as return value (range between start of the 
			// string and the first non empty string), then split about ",", code product
			// the last element of vector
			// 3. From that position (first empty to the end of the string), look for first
			// non empty character, probably the beginning of the description. NOTE sometimes
			// description and quantity/silo are on the same line because there is no notes.
			// Split about "," but if there is no other stuff than description, it shouldn't
			// have any (vector == 0)
			// descr should be the first element

			// last part is the product description
			size_t w_charPos = aFieldValue.find_first_of(" ");
			string w_lastPart(aFieldValue.cbegin() + w_charPos, aFieldValue.cend());
			boost::trim_left(w_lastPart);
			vector<string> w_descrStr;
			split(w_descrStr, w_lastPart, boost::is_any_of(","));
			// check size
			//	std::vector<std::string>::size_type w_lastSize = w_descrStr.size();
			aBoducF.m_produit = w_descrStr[0];
		}

		void VictoReader::readQtySilo(const std::string & aFieldValue, BoDucFields & aBoducF)
		{
			using namespace std;
			using namespace boost;
			using namespace boost::algorithm;

				if( contains(aFieldValue, std::string("TM")) || // keyword to look for
					  contains(aFieldValue, std::string("TON")))
				{
					std::vector<std::string> w_splitStr;
					boost::split( w_splitStr, aFieldValue, boost::is_any_of(","));

					// remove all empty string, then we have as first element the "TM" string
					// just after we have the qty and silo 
					w_splitStr.erase(std::remove_if(w_splitStr.begin(), w_splitStr.end(), // delete empty element
						[](const std::string& s)
					{ return s.empty(); }), w_splitStr.cend());
					
// 					assert(4 == w_splitStr.size());
// 					w_splitStr.pop_front(); // remove first element not relevant
// 					assert(w_splitStr.size == 3); // "TM" or "TON" is the first element

					auto w_look4TM = std::find(w_splitStr.cbegin(), w_splitStr.cend(), std::string("TM"));
					if( w_look4TM!=w_splitStr.cend())
					{
						++w_look4TM;
					}
					else // switch to another keyword
					{
						w_look4TM = std::find(w_splitStr.cbegin(), w_splitStr.cend(), std::string("TON"));
						if( w_look4TM==w_splitStr.cend())
						{
							return; // can't find anything at this point something wrong!!
						}
						else
						{
							++w_look4TM;
						}
					}
					//++w_look4TM;

					aBoducF.m_qty = std::stof(*w_look4TM++); // ready for next value
					//aBoducF.m_qty = std::stof(w_splitStr[1]);  second element is quantity 

					// NOTE: some of the silo number has the following format "03"
					// want to remove the first character since it is not relevant
					// IMPORTANT: we have also the following case: 10 and higher
					// need to consider this case
					// Line below search for a zero, but we need to first check 
					// for the format xx which can be 03 or 10.
					// First check if the string has a size of 2
					// if so first element is zero, otherwise we have a silo number 
					// greater than 0.
					
					//std::string::size_type w_doubleNum = (*w_look4TM).size();
					//std::string::size_type w_doubleNum = w_splitStr[2].size();

					// check for empty string (Kalyan)
					if( w_look4TM == w_splitStr.cend())
					{
						// means that we have a empty field
						aBoducF.m_silo = "";
					}
					else
					{
						std::locale loc; // count number of digit
						auto w_doubleNum = std::count_if(w_look4TM->cbegin(), w_look4TM->cend(),
							[loc](unsigned char c) { return std::isdigit(c, loc); }
						);

						if (w_doubleNum > 1) // more than one digit
						{
							// first char is 0?
							char w_firstDigit = (*w_look4TM)[0];
							if (w_firstDigit == '0') // silo number in the range [0,...,9] but with double format
							{
								// then remove it (not consider it)
								aBoducF.m_silo = (*w_look4TM)[1];
							}
							else
							{
								// we have a silo number >=10
								aBoducF.m_silo = (*w_look4TM);
							}
						}
						else
						{
							aBoducF.m_silo = *w_look4TM; // silo number in the range [1,...,9]
						}
					}
	//				break; // found it, get out
//				}
#if 0
				if ((*w_look4TM).find("0") != std::string::npos)
				{
					aBoducF.m_silo = (*w_look4TM)[1];
				}
				else
				{
					aBoducF.m_silo = *w_look4TM;
				}
				//break; no point to go further
#endif
			}
		}

		void VictoReader::readProdCode(const std::string & aFieldValue, BoDucFields& aBoducF)
		{
			using namespace std;
			using namespace boost;

			size_t w_charPos = aFieldValue.find_first_of(" ");
			string w_subStr(aFieldValue.cbegin(), aFieldValue.cbegin() + w_charPos);
			vector<std::string> w_codeSplit;
			split(w_codeSplit, w_subStr, is_any_of(","));

			// search for first digit
			std::string w_prodCode;
			if (!std::all_of(w_codeSplit[1].begin(), w_codeSplit[1].end(), ::isdigit)) //!is_digits(w_codeSplit[1])
			{
				// we have to remove non-digit character from string 
				string::iterator w_foundDigit = find_if(w_codeSplit[1].begin(), w_codeSplit[1].end(),
					[](char c)
				{
					locale loc;
					return isdigit(static_cast<unsigned char>(c), loc);
				});

				size_t w_next = distance(w_codeSplit[1].begin(), w_foundDigit);
				// product code
				string::iterator w_chck = w_codeSplit[1].begin();
				advance(w_chck, w_next);
				string w_codeProd(w_chck, w_codeSplit[1].end());
				aBoducF.m_prodCode = atoi(w_codeProd.c_str());
			}
			else
			{
				w_prodCode = w_codeSplit[1];
				aBoducF.m_prodCode = atoi(w_prodCode.c_str());
			}
		}
} // End of namespace

