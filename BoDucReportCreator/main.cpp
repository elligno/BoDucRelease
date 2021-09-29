// test our string algo
#include <fstream>

// temporary for debug (to be removed)
#include <deque>
#include <boost/algorithm/string/split.hpp> // spitting algo
#include <boost/algorithm/string.hpp> // string algorithm

// app inlcudes
#include "BoDucReportCreator.h"
#include <QtWidgets/QApplication>


void findTwoSeqWhiteSpace( const std::string& aString2Check, std::size_t& aEndFirstStr)
{
  if (aString2Check.empty())
  {
    return;
  }

  if( std::isalpha( aString2Check[0], std::locale{}) == 0) // first character is white space
  {
    return;
  }

  auto strPos = aString2Check.find_first_of(' ');
  if( strPos == std::string::npos) // end of string
  {
    return;
  }

  std::string w_subStr;
  if( std::isalpha( aString2Check[strPos + 1], std::locale{}) != 0)
  {
    aEndFirstStr += strPos+1;
    std::string w_restOfStr( aString2Check.cbegin() + strPos + 1, aString2Check.cend());
    w_subStr = aString2Check.substr(strPos + 1);
    findTwoSeqWhiteSpace( w_subStr, aEndFirstStr);
  }
  else
  {
    aEndFirstStr += strPos;
  }

  return;
}

// ===================================================================
//
//                        Main Entry Point
//
// ===================================================================

int main( int argc, char *argv[])
{
#if 0
  // did some test reading the R csv format (pdf to csv)
  // it needs some clean-up (removing unwanted stuff such as double quotation)
  // also each line start with a string for line number, we need to remove this
   std::deque<std::string> w_vecOfLines;
   //w_vecOfLines.reserve(50);
   const std::string w_fileName = "MartinoTest.csv";
   std::ifstream w_readVSV(w_fileName.c_str());
   if( w_readVSV) // check if its open, then read line-by-line 
   {
     for( std::string line; std::getline( w_readVSV, line);)
     {
       //line.erase(std::remove(line.begin(), line.end(), '\"'), line.end());
       w_vecOfLines.push_back(line);
     }
   }// for loop

  w_vecOfLines.pop_front();
//    std::deque<std::string> w_splittedVec;
     std::vector<std::string> w_vecCheck;
     w_vecCheck.reserve(75);
  // w_splittedVec.reserve(75);
    for( const auto& w_str : w_vecOfLines)
    {
      const char* w_serach_str = ",";
      auto siz = w_str.find_first_of(w_serach_str);
      if( siz != std::string::npos)
      {
         // found it
         siz += 1;
         std::string w_subStr( w_str.cbegin()+siz, w_str.cend());
         // remove Double Quotation "\" beginning of the string
         w_subStr.erase(std::remove(w_subStr.begin(), w_subStr.end(), '\"'), w_subStr.end());
         boost::trim_left(w_subStr); // remove white space at beginning
         w_subStr.erase(std::remove(w_subStr.begin(), w_subStr.end(), '\r'), w_subStr.end());
         w_vecCheck.push_back(w_subStr);
      }
      // remove end of line cartridge return character
      //w_vecCheck.erase( std::remove(w_vecCheck.begin(), w_vecCheck.end(), '\r'), w_vecCheck.end());

  //    boost::split( w_splittedVec, w_str, boost::is_any_of(","));
  //    w_splittedVec.pop_front();
//   //   w_splittedVec.erase( w_splittedVec.begin(), w_splittedVec.begin() + 1);
 //   w_splittedVec[0].erase(std::remove(w_splittedVec[0].begin(), w_splittedVec[0].end(), '\"'), w_splittedVec[0].end());
//   //   boost::split( w_splittedVec[0], boost::is_any_of(" "));
//       boost::trim_left(w_splittedVec[0]);
//       w_vecCheck.push_back(w_splittedVec[0]);
    }

    // check address
    std::string w_addrstr = w_vecCheck[9]; // shall be customer name
    auto w_len = w_addrstr.size();
    std::size_t w_begLastStr {};
    findTwoSeqWhiteSpace( w_addrstr, w_begLastStr); // find 2 white space sequence
    std::string w_shippedAddrs{ w_addrstr.cbegin() + w_begLastStr, w_addrstr.cend() };
    auto w_foundFirstCharPos = w_shippedAddrs.find_first_not_of(' '); // find first character
    if( w_foundFirstCharPos != std::string::npos)
    {
      // remove white space from beginning to 
      w_shippedAddrs.erase( w_shippedAddrs.cbegin(), w_shippedAddrs.cbegin() + w_foundFirstCharPos); // the first non white space
    }

//     const auto strPos = w_addrstr.find_first_of(' ');
//     if( std::isalpha(w_addrstr[strPos+1], std::locale{})!=0)
//     {
//       std::cout << "We have a character\n";
//     }

    //auto dist = std::distance(w_addrstr.begin(), w_addrstr.size()/2);
    //auto halfRng  = w_addrstr.begin() / 2;
    auto w_shipAddrs = w_addrstr.substr(w_addrstr.size() / 2); // return substring from half to end
    boost::trim_left(w_shipAddrs); // remove blank if any
    // some end of line character that want to strip
    w_shipAddrs.erase(std::remove(w_shipAddrs.begin(), w_shipAddrs.end(), '\r'), w_shipAddrs.end());

    std::cout << "Half string address" << w_shipAddrs.c_str() << "\n";
#endif

	QApplication a(argc, argv);
	bdApp::BoDucReportCreator w;
	w.show();
	return a.exec();
}