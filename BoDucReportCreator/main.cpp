// test our string algo
#include <fstream>
#include <sstream>
// temporary for debug (to be removed)
#include <deque>
#include <boost/algorithm/string/split.hpp> // spitting algo
#include <boost/algorithm/string.hpp> // string algorithm
// app inlcudes
#include "BoDucReportCreator.h"
#include "BdAPI/BoDucCmdFileReader.h"
// Qt includes
#include <QtWidgets/QApplication>
#include <QStringBuilder>
#include <QTextStream>

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

std::initializer_list<std::string> glisTransporteur = { "NIR R-117971-3 TRSP CPB INC",
"NIR R-117971-3 TRANSPORT CPB",
"NIR R-117971-3 C.P.B.",
"BODUC- ST-DAMASE",
"NIR R-004489-2 TR. BO-DUC" };

int main( int argc, char *argv[])
{
#if 0
  QFile w_file2Read("MartinoTest.csv");
  w_file2Read.isReadable();
  //bdAPI::BoDucFileListCmdTxt w_listFileCmdText = bdAPI::BoDucCmdFileReader::readFile(w_file2Read);
  const auto w_listFileCmdText = bdAPI::BoDucCmdFileReader::readFile(w_file2Read);
  const auto w_nbCmdThisFile = w_listFileCmdText.nbOfCmd();
  const auto w_nameCmdFile = w_listFileCmdText.filename();
  const auto w_firstCmdTxt = w_listFileCmdText.first();
  if( !w_firstCmdTxt.isEmpty())
  {
    if( !w_firstCmdTxt.hasTransporteurNameValid(glisTransporteur))
    {
      return EXIT_FAILURE;
    }

    std::cout << "This cmd need to be treated\n";
    QStringList w_keywords{ QString{"Ordered on"} , QString{"Shipped to"}, QString{"Date promise"},
                            QString{"Transporteur"}, QString{ "Qty ordered" } };
    auto w_strListIdx{ 0 }; // keyword index
    auto w_checkSiz = w_firstCmdTxt.size(); // number of lines don't work!!
    QStringListIterator w_cmdIter{ w_firstCmdTxt.asQStringList()};
    while( w_cmdIter.hasNext()) // iterate over each cmd line
    {
      auto w_currLine = w_cmdIter.next();
      if( w_currLine.contains( QString{ w_keywords[0] }))
      {
        auto w_cmdNo = w_currLine.split(" ").back();
      //  auto w_cmdNo = w_strList.back();
        if( w_cmdNo.startsWith( QString{ "CO" }))
        {
          std::cout << "Cmd number is: " << w_cmdNo.data() << "\n";
        }
        //++w_strListIdx;  next in the list
      }
      if( w_currLine.contains( QString{ w_keywords[1] }))
      {
        QStringList w_addressLines;
        QString w_Line2look4;
        do 
        {
          w_Line2look4 = w_cmdIter.next();
          auto res = w_Line2look4.split( QString{ "' '' '' '" }); //3 white spaces as default
          auto lastWord = res.back(); // shall be the address of 
          w_addressLines.push_back(lastWord);
        } while( !w_Line2look4.contains( QString{"Contact"}));
        w_addressLines.pop_back();
        auto sizeQStrList = w_addressLines.size();
        std::cout << "We have something\n";
      }

      if( w_currLine.contains( QString{ w_keywords[2] }))
      {
        auto w_Line2look4 = w_cmdIter.next();
        std::istringstream iss{ w_Line2look4.toStdString() };
        std::vector<std::string> w_splitStr{ std::istream_iterator<std::string>{iss},
                                             std::istream_iterator<std::string>{} };
        for( auto&& w_str2LookAt : w_splitStr)
        {
          if( w_str2LookAt.find_first_of("/") != std::string::npos) // found it?
          {
            bdAPI::BoDucFields w_bdField;
            w_bdField.m_datePromise = w_str2LookAt;
            std::cout << "This is the delivery date: " << w_str2LookAt << "\n";
            break;
          }
          continue;
        }
      }
    }//while-loop
  }//if

  std::cout << "Finished reading cmd from file\n";

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

  //
  //    Qt API
  //

  QFile w_csvFile2Read( QString("MartinoTest.csv"));
  w_csvFile2Read.open( QFile::ReadOnly);
  QTextStream w_textFile2Clean(&w_csvFile2Read);
  QStringList w_listOfStr;
  while( !w_textFile2Clean.atEnd())
  {
    auto w_strReaded = w_textFile2Clean.readLine();
    auto w_testSplit = (w_strReaded.split(",")[1]);
    w_testSplit.replace('\"',' ');
    const auto& w_cleanedUp = w_testSplit.trimmed();
    w_listOfStr.push_back(w_cleanedUp);
  }

  // Write to file
  QFile w_write2File(QString("MartinoQtFmt.txt"));
  w_write2File.open( QFile::WriteOnly | QFile::Text);
  QTextStream w_testingNewFmt(&w_write2File);
  QStringListIterator w_listIter(w_listOfStr);
  while( w_listIter.hasNext())
  {
    w_testingNewFmt << w_listIter.next() << "\n";
  }
  w_write2File.close();

  QDir w_checkFle =  QDir::current();
  if( !w_checkFle.exists( QString("MartinoQtFmt.txt")))
  {
    return EXIT_FAILURE;
  }

#if 0 // testing our new implementation 
  // just reading a file to test our reading algo 
  QFile w_file2TestRead( QString("MartinoTest.csv"));
  // sanity check (shall be current working directory)
  QString w_checkPath = w_file2TestRead.fileName();
//  QDir w_dir(w_checkPath);
  // is it different?
  QFileInfo w_fileInfo( w_file2TestRead);
  auto w_absPath = w_fileInfo.absolutePath();
  auto w_filName = w_fileInfo.fileName();

  // BoDucFileListCmdTxt (list of all command in the file: w_file2TestRead)
  auto w_testRet = bdAPI::BoDucCmdFileReader::readFile(w_file2TestRead);
  auto w_bdCmdTxtIter = w_testRet.getIterator();
  while( w_bdCmdTxtIter.hasNext())
  {
    const auto& w_bdCmdTxt = w_bdCmdTxtIter.next();
    
    // Check info about file name 
    auto w_fileName = w_testRet.filename();

    // next want to iterate over each line 
    QStringList w_strListCmd = w_bdCmdTxt.asQStringList();
    bool isTM = w_strListCmd.contains( QString("TM"));
 //   if( isTM)
  //  {
  
     // remove multiple whitespaces from the name
    // name.replace(QRegularExpression("\\s+"), " ");
  //edit: i originally posted the following, which would delete the first character from every line.upon re - reading the question i realized that isn't quite what was wanted.

   // sed - i 's/^.\(.*\)/\1/' file
  // https://www.theunixschool.com/2014/08/sed-examples-remove-delete-chars-from-line-file.html
      
      // can proceed with this command 
      QStringListIterator w_cmdLinesIter( w_strListCmd);
      while( w_cmdLinesIter.hasNext())
      {
        if( w_cmdLinesIter.next().contains("Date"))
        {
          auto w_cmdNo = w_cmdLinesIter.next();
          // split according to one or more white space
          // regular expression \s is a predefined character class that indicates a single whitespace
          // the '+' one or more of regex
          auto w_listElem = w_cmdNo.split( QRegularExpression("\\s+"));
          auto w_lastElem = w_listElem.back();
          if( w_lastElem.startsWith("CO")) // and check for rest of the string to be digits
          {
            // can we check for digit with qt ^CO[0-9]$
            bool w_checkFormat = w_lastElem.contains( QRegularExpression("[0-9]$"));
            if (w_checkFormat)
            {
              auto dfg = w_lastElem;
            }
          }
        }//if
      }//while-loop
   // }if
  }
#endif // end testing

 // QMap<QString, bdAPI::BoDucFileListCmdTxt> w_justTest;
  //QMapIterator w_mapIter(w_testRet);
 // w_justTest.find()

  // could convert to std vector

//    std::deque<std::string> w_splittedVec;
     std::vector<std::string> w_vecCheck;
     w_vecCheck.reserve(75);
  // w_splittedVec.reserve(75);
    for( const auto& w_str : w_vecOfLines)
    {
      const char* w_search_str = ",";
      auto siz = w_str.find_first_of(w_search_str); // returns iterator pos of comma
      if( siz != std::string::npos)
      {
         // found it
         siz += 1; // pos of comma, we we want to remove the whole "," 
         std::string w_subStr{ w_str.cbegin() + siz, w_str.cend() };
         // remove Double Quotation "\" beginning of the string
         w_subStr.erase( std::remove( w_subStr.begin(), w_subStr.end(), '\"'), w_subStr.end());
         boost::trim_left( w_subStr); // remove white space at beginning
         w_subStr.erase(std::remove( w_subStr.begin(), w_subStr.end(), '\r'), w_subStr.end());
         w_vecCheck.push_back( w_subStr);
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

    std::ofstream w_stdFile( std::string("myFile.txt"));
    if( w_stdFile) // check if open
    {
      for( const auto& w_str : w_vecOfLines)
      {
        w_stdFile << w_str << "\n";
      }
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