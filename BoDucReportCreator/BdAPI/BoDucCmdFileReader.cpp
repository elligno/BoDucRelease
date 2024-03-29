
// C++ includes
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <stdlib.h>
// boost includes
#include <boost/algorithm/string.hpp> // string algorithm
#include <boost/algorithm/string/split.hpp> // splitting algo
// Qt includes
// #include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>

// lib includes
#include "BoDucUtility.h"
#include "BoDucCmdFileReader.h"

// static declaration (internal linkage)
namespace { // check if it is a valid character

  // sometime we have empty (blank field) string
  // not sure if its a good thing to declare it in a translation unit? 
  std::initializer_list<std::string> m_lisTransporteur = { "NIR R-117971-3 TRSP CPB INC",
                                                           "NIR R-117971-3 TRANSPORT CPB", 
                                                           "NIR R-117971-3 C.P.B.", 
                                                           "BODUC- ST-DAMASE", 
                                                           "NIR R-004489-2 TR. BO-DUC"};
  bool charPredicate(char c)
  {
    return !(isalpha(c) || c == '_');
  }

  // lambda (anonymous) function declaration
  auto checkTransportName = [](const std::string& aStr2Look) -> bool
  {
    // Transporteur name (BoDuc)
    return ( boost::contains(aStr2Look, "NIR R-117971-3 TRSP CPB INC")
          || boost::contains(aStr2Look, "NIR R-117971-3 TRANSPORT CPB")
          || boost::contains(aStr2Look, "NIR R-117971-3 C.P.B.")
          || boost::contains(aStr2Look, "BODUC- ST-DAMASE")
          || boost::contains(aStr2Look, "NIR R-004489-2 TR. BO-DUC"));     //sometime we have empty (blank field) string
  };
}

namespace bdAPI {

  BoDucCmdFileReader::vecmapstr BoDucCmdFileReader::readFiles( const QStringList& aFilesNameWithPath,
    const std::string& aSplitCmdToken /*= std::string("Signature")*/)
  {
    // multiple files selection supports
    std::vector<mapIntVecstr> w_vecOfMap; 
    QStringListIterator w_filesIter(aFilesNameWithPath);
    while( w_filesIter.hasNext())
    {
      const auto& w_file2Proceed = w_filesIter.next();
      BoDucCmdFileReader::mapIntVecstr w_mapintVec =
        readFile( w_file2Proceed.toStdString(), aSplitCmdToken /*std::string("Signature")*/);

      // cannot bind a lvalue to rvalue (w_mapintVec)???
      // NOTE we use the push_back rvalue version by casting 
      // lvalue to rvalue reference 
      w_vecOfMap.push_back( std::move(w_mapintVec)); 

      // sanity check (according to the move semantic)
      assert( 0 == w_mapintVec.size()); 
    }

#if 0

    auto w_begListIter = aFilesNameWithPath.cbegin();
    while (w_begListIter != aFilesNameWithPath.cend())
    {
   //   const std::string& w_fileName = *w_begListIter;

      // call readFile
      BoDucCmdFileReader::mapIntVecstr w_mapintVec = 
          readFile(w_fileName, std::string("Signature"));
      
      // check if it exist
//       if (m_nbOfCmdInFile.find(w_fileName) != m_nbOfCmdInFile.cend())
//       {
//         continue; // not sure about this one!!
//       }
// 
//       m_nbOfCmdInFile.insert(std::make_pair(*w_begListIter, w_numCmd));

      // push m_mapIntVec into vector
      // vector provide a push_back() that support the move semantic  
      // since we don't need the content of the map for next iteration
      // might as well to move its content, is that make sense?
      // don't copy something that i am not going to use 
      w_vecOfMap.push_back( std::move(w_mapintVec)); // can i do that? why not

      // sanity check
      assert(0 == w_mapintVec.size()); // according to the move semantic
      ++w_begListIter; // next in the list
    }
#endif

    return w_vecOfMap;
  }

  // copy on return
  BoDucCmdFileReader::mapIntVecstr BoDucCmdFileReader::readFile( const std::string& aFileAnPath, 
    const std::string& aSplitBill /*= "Ordered on"*/)
  {
    using namespace std;
    using namespace boost;
    using namespace boost::algorithm;

    // some of the file has french accent, when this
    // happens we need to remove it
    const std::string w_fileName = check4Accent(aFileAnPath);

    // store our command as string
    // shall be a vector of pair <string,vecstr> with string is the file name
    // corresponding to command vector of string (lines)
    mapIntVecstr w_mapintVec;

    // declare vector to store our string from csv document 
    vecofstr w_vecStr;
    w_vecStr.reserve(50); // reserve enough space for storing each line ??

    // Testing purpose (next version, we must use the QString)
    // we don't have any problem utf-8 character
    QStringList w_testStr;
    w_testStr.reserve(50);

    // Command in one file, reading the command by splitting with the "Ordered on"
    short i(0); // set to zero as default, otherwise set to whatever comes up
    ifstream w_readVSV( w_fileName.c_str());
    if( w_readVSV) // check if its open, then read line-by-line 
    {
      for( string line; std::getline(w_readVSV, line);)
      {
        if (i == 0) // first line is obsolete
        {
          // check if contains "Ordered on" as first line
          // NOTE "request-2.csv" we split the bill with this token, 
          // and format was that first line contains this string
          if( contains(line, "Ordered on"))
          {
            ++i;  // next line
            continue; // i don't see why we should increment it
          }
        }//if(i==0)
        w_vecStr.push_back(line);

        // just a test
        // The given const char pointer is converted
        // to unicode using the fromUtf8() function
//         QString w_strFmt( line.data()); 
//         w_testStr.push_back( QString(line.data()));   by using this, we don't have the scrapt of the std string

        // NOTE we assume that we are at the last line of the command
        // then we check if the carrier string name is part of the whole 
        // command (if so, add it to the map otherwise skip it)
        // IMPORTANT this algorithm assume that we are at the end or the 
        // last line (split into separate command is based on this assumption)
        // if not the case then it won't work!!
        if( contains( line, aSplitBill))
        {
          // check for carrier name some are different and don't need to be treated
          // Francois mentioned that field can be blank, need to be careful
          if( any_of( w_vecStr.cbegin(), w_vecStr.cend(), checkTransportName))
          {
            w_mapintVec.insert(make_pair(i++, w_vecStr));
            w_vecStr.clear();
          }
          else
          {
            QMessageBox w_msgBname;
            QFileInfo w_filExtract(aFileAnPath.c_str());
            const QString w_fileName = w_filExtract.fileName();
            QString w_strMsg = QString("Not a valid transporteur name in ") + w_fileName;
            w_msgBname.setText(w_strMsg);
            w_msgBname.setIcon(QMessageBox::Warning);
            w_msgBname.exec();
            // ready for the next iteration
            w_vecStr.clear();
          }
        }
      }//for-loop
    }//if
    else // file could not be opened because name invalid or something else
    {
      QMessageBox msgBox;
      QFileInfo w_filExtract(aFileAnPath.c_str());
      const QString w_fileName = w_filExtract.fileName();
      QString w_strMsg = QString("The document ") + w_fileName + QString(" could not be opened.");
      msgBox.setText(w_strMsg);
      msgBox.setInformativeText("We try to fix it, do you want to proceed?");
      msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
      msgBox.setIcon(QMessageBox::Warning);
      int ret = msgBox.exec();
    }

    // temp fix for debugging purpose
    return w_mapintVec;
  }

  // not in use at the  moment, just a test 
  BoDucFileListCmdTxt BoDucCmdFileReader::readFile( QFile& aFileAndPath)
  {
    // debugging stuff
    QFileInfo w_fileInfo(aFileAndPath);

    BoDucFileListCmdTxt w_bdFileListCmdTxt( aFileAndPath);

    BoDucCmdText w_bdCmdTxt;
    // some character at the beginning of the line (crapt)
 //   QString w_remChar = ","; 
    if( aFileAndPath.open( QFile::ReadOnly | QFile::Text))
    {
      QTextStream w_readText(&aFileAndPath);
      while( !w_readText.atEnd())
      {
        auto w_line = w_readText.readLine();
        auto w_idx = w_line.indexOf( QString(",")); 
        if( w_idx != 0)
        {
          w_line = w_line.remove( 0, w_idx + 1); // +1 to remove the whole string "," 
        }
        if( w_line.startsWith('\"'))
        {
          w_line.remove( QChar('\"'), Qt::CaseInsensitive);
        }
        //auto isWhiteSpace = w_line.startsWith('\"'); // remove " is character at beginning
  //      QString w_test( w_line);
        auto w_trimStr = w_line.trimmed();
        w_bdCmdTxt.push_line( std::move(w_trimStr));

        if( w_line.contains( "Signature"))
        { 
          // move semantic since we don't need this command
          w_bdFileListCmdTxt.add( std::move(w_bdCmdTxt));
        }
      }//while-loop
    }

    // return list of cmd as text format
    return w_bdFileListCmdTxt;
  }

  // pass by reference no move semantic
  BoDucFileListCmdTxt BoDucCmdFileReader::readFile( const std::unique_ptr<QFile>& aFileAndPath, QString cmdSep/* = QString("Signature")*/)
  {
    // QFileInfo w_fileInfo(*aFileAndPath);
    BoDucFileListCmdTxt w_bdFileListCmdTxt(*aFileAndPath);
   // std::vector<std::string> w_vecStr;
    BoDucCmdText w_bdCmdTxt;
   // w_vecStr.reserve(50);

    // ready to read line
    if( aFileAndPath->open( QFile::ReadOnly | QFile::Text))
    {
      // start reading line to cmd text format
      // Shall be available to read file name with accent since 
      // QTextStream use UTF-8 encoding 
      QTextStream w_readText( &(*aFileAndPath));
      while( !w_readText.atEnd())
      {
        auto w_line = w_readText.readLine();
        w_bdCmdTxt.push_line( w_line.toStdString());
        if( w_line.contains(cmdSep))
        {
          // NOTE: Should also check for "tonnage" tags such as "TM" and "TON"
          // check transporteur name
          if( w_bdCmdTxt.hasTransporteurNameValid( m_lisTransporteur))
          {
            // something to ...
            // remove blank lines if any
            w_bdFileListCmdTxt.add( BoDucCmdText(w_bdCmdTxt)); // shal do that  .add(std::move(w_bdCmdTxt));
            w_bdCmdTxt.clearCommand(); // ready for next
          }
          else
          {
            QMessageBox w_msgBname;
            QFileInfo w_filExtract(*aFileAndPath);
            const QString w_fileName = w_filExtract.fileName();
            QString w_strMsg = QString("Not a valid transporteur name in ") + w_fileName;
            w_msgBname.setText(w_strMsg);
            w_msgBname.setIcon(QMessageBox::Warning);
            w_msgBname.exec();

            // ready for the next iteration
            //w_vecStr.clear();
          }
        }//if-contains
      }//while-loop
    }//if-open

    // closing before leaving
    aFileAndPath->close();

    return w_bdFileListCmdTxt;
  }

  // TODO: first pitch, need to review this code and complete implementation
  std::vector<BoDucFileListCmdTxt> BoDucCmdFileReader::readFiles( const std::forward_list<std::unique_ptr<QFile>>& aListFiles,
    QString cmdSep /*= QString("Signature")*/)
  {
    // each file with corresponding command
    std::vector<BoDucFileListCmdTxt> w_filesCmdTxt;

    // 
    auto w_fileListBegin = aListFiles.cbegin();
    auto checkEmpty = std::distance(aListFiles.cbegin(), aListFiles.cend());
    while( w_fileListBegin != aListFiles.cend())
    {
      // debugging purpose (will be removed)
      const std::unique_ptr<QFile>& w_checkUniq1 = *w_fileListBegin;
      QFileInfo w_finfo(*w_checkUniq1);
      if( w_finfo.isFile())
      {
        auto aa1 = w_finfo.fileName();
        auto bb1 = w_finfo.absoluteFilePath();
        QDir w_dir = w_finfo.dir();
        QString w_dNam = w_dir.dirName();
        bool w_checkExist = w_finfo.exists();
      }
//       const auto& w_fil = std::move(*w_fileListBegin);
//       const std::unique_ptr<QFile>& w_jj = std::move(*w_fileListBegin);
//       QFileInfo w_checkInfoDbg(**w_fileListBegin);
//       auto nameTest = w_checkInfoDbg.fileName();
//       auto pathTest = w_checkInfoDbg.absoluteFilePath();

 //     BoDucFileListCmdTxt w_test = readFile( *w_fileListBegin);
      w_filesCmdTxt.push_back( readFile( *w_fileListBegin));

      // increment iterator for next in the list
      ++w_fileListBegin;
    }//while-loop

    return w_filesCmdTxt;
  }

  std::string BoDucCmdFileReader::check4Accent( const std::string &aFileAnPath)
  {
    // just a test (accent in file name)
    QFile w_wholeFile(aFileAnPath.c_str());
    QFileInfo w_infFile(w_wholeFile);
    const auto& w_myFilename = w_infFile.fileName();

    // not sure if we are going to keep it (just want to make sure )
    std::string w_fileName;
    if (!w_fileName.empty())
    {
      w_fileName.clear();
    }

    // check for accent in file name, if so fix it
    if (std::find_if(aFileAnPath.cbegin(), aFileAnPath.cend(), charPredicate) != aFileAnPath.cend())
    {
      // we have special character, need to fix name without accent
      std::wstring w_utf8FileName = BoDucUtility::FromUtf8ToUtf16(aFileAnPath);
      w_fileName.assign(w_utf8FileName.cbegin(), w_utf8FileName.cend());
    }
    else
    {
      // csv file that we are processing
      w_fileName.assign(aFileAnPath.cbegin(), aFileAnPath.cend());
    }

    return w_fileName;
  }

} // End of namespace