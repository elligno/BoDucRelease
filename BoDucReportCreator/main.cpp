// test our string algo
#include <fstream>
#include <boost/algorithm/string/split.hpp> // spitting algo

 // app inlcude
#include "BoDucReportCreator.h"
#include <QtWidgets/QApplication>

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
         w_vecCheck.push_back(w_subStr);
       }
  //    boost::split( w_splittedVec, w_str, boost::is_any_of(","));
  //    w_splittedVec.pop_front();
//   //   w_splittedVec.erase( w_splittedVec.begin(), w_splittedVec.begin() + 1);
 //   w_splittedVec[0].erase(std::remove(w_splittedVec[0].begin(), w_splittedVec[0].end(), '\"'), w_splittedVec[0].end());
//   //   boost::split( w_splittedVec[0], boost::is_any_of(" "));
//       boost::trim_left(w_splittedVec[0]);
//       w_vecCheck.push_back(w_splittedVec[0]);
    }
#endif

	QApplication a(argc, argv);
	bdApp::BoDucReportCreator w;
	w.show();
	return a.exec();
}
