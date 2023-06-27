#pragma once

// C++ includes
#include <string>
#include <vector>
#include <utility> // std::move
#include <forward_list>
// boost includes
//#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp> // string algorithm
//#include <boost/algorithm/string/split.hpp> // splitting algo
// Qt includes
#include <QFile>
#include <QFileInfo>
#include <QVector>
#include <QStringList>
#include <QStringListIterator>

//#include <QDir>
// #include <QTextStream>
// #include <QMessageBox>
// boost include
//#include <boost/operators.hpp>
// API package include
#include "VictoReader.h"

namespace bdAPI 
{
  /** Responsible to keep a string representation of the command (lines).
  // check for number of command to proceed
  //std::map<std::string, size_t> m_nbOfCmdInFile;
//   size_t w_numCmd = nbOfCmd();
//   // check if it exist
//   if (m_nbOfCmdInFile.find(w_fileName) != m_nbOfCmdInFile.cend())
//   {
//     continue; // not sure about this one!!
//   }
//   m_nbOfCmdInFile.insert(std::make_pair(*w_begListIter, w_numCmd));
  */
  class BoDucCmdText : public boost::equality_comparable<BoDucCmdText> // strickly comparable
  {                                                                    // define != operator
  public:

    using cmdLineIterator = boost::iterator_range<std::vector<std::string>::const_iterator>;

    auto begin() { return m_qListStrCmd.begin(); }
    auto end()   { return m_qListStrCmd.end();   }

    auto cbegin() const { m_qListStrCmd.cbegin(); }
    auto cend()   const { m_qListStrCmd.cend(); }

  public:
    // what about default, copy and assignment ctor (=default)
    BoDucCmdText() 
    { 
      m_qListStrCmd.reserve(50); 
    }

    // default ctor will be ok
    BoDucCmdText( const BoDucCmdText& aOther) = default;
    BoDucCmdText& operator= ( const class BoDucCmdText& aOther) = default;
    //~BoDucCmdText() = default;  by doing this you disable move semantic

    // move semantic shall be supported
    BoDucCmdText( BoDucCmdText&& aOther)
    : m_qListStrCmd(std::move(aOther.m_qListStrCmd))
    {}

    void push_line( QString&& aLine2Mv)
    {
      m_qListStrCmd.push_back( std::move(aLine2Mv));
    }

    void pop_line()
    {
      m_qListStrCmd.pop_back();
    }

    // 
    //void find_line_with_token(boost::find_token) { throw "Not implemented yet"; }

    // not sure about this one!!!
    QStringListIterator getQListIterator() const { return QStringListIterator(m_qListStrCmd);     }
   // cmdLineIterator getCmdLineIterator()   const { return boost::make_iterator_range(m_veCmdStr); }

    // return a range based on line index (boost range)
//     QStringList rangeOfLines( size_t begLine, size_t endLine)
//     {
//       // implement with the boost range library
//       //boost::iterator_range<std::vector<std::string>::const_iterator> cmd_range(m_veCmdStr.cbegin() + begLine, m_veCmdStr.cbegin() + endLine);
// 
//       return QStringList{ m_qListStrCmd.cbegin() + begLine, m_qListStrCmd.cbegin() + endLine };
//     }

    std::vector<std::string>::size_type size() const
    {
      // number of lines for this command
      return m_qListStrCmd.size();
    }

    /** clear whole command */
    void clearCommand()
    {
      if( !m_qListStrCmd.isEmpty())
      {
        m_qListStrCmd.clear();
      }
    }

    /** Implicit conversion operator to be use with our algorithm
    * Our algorithm have been designed as taking std::vectorof string
    * but in our new architecture 
     According to Scott Myers be carefull when using such tricky stuff;*/
    operator std::vector<QString>() const { return m_qListStrCmd.toVector().toStdVector(); }

    // not sure about those?? be carefull!! "TON" can be part of word like 'BROMPTON'
    // when checking for that given token, special care must be taken
    bool hasTM_TON() const
    {
//      using namespace boost::algorithm;

      // first search for QString{ "Qty ordered" }
      // when found it, go to next line and look for TN || TON
      QStringListIterator w_lineIter{ m_qListStrCmd };
      QString foundIt{};
      do 
      {
        foundIt = w_lineIter.next();
      } while( !foundIt.contains( QString{ "Qty ordered" }));
      
//       return std::any_of(m_qListStrCmd.cbegin(), m_qListStrCmd.cend(),
//         [](const QString& aStr) // check for the "TM" or "TON"
//       {
        // check for a valid
        // "BROMPTON" contains "TON", we need some kind of mechanism
        // to check if this a valid code or a string contained 
        return w_lineIter.next().contains(QString{ "TM" }) || w_lineIter.next().contains(QString{ "TON" });
//      });
    }

    // before extracting let's check so
    bool hasTransporteurNameValid( const std::initializer_list<std::string>& aListTransporteur) const
    {
      QStringListIterator w_qstrListIter{ m_qListStrCmd };
      while( w_qstrListIter.hasNext())
      {
        if( w_qstrListIter.next().contains( QString{ "Transporteur" },Qt::CaseInsensitive))
        {
          if( w_qstrListIter.next().contains(QString{"NIR R-117971-3 C.P.B."}))
          {
            return true;
          }
        }
      }//while-loop
      // 
      return false;

#if 0
      auto begList = aListTransporteur.begin();
      while( begList != aListTransporteur.end())
      {
        std::string w_transporteurName = *begList;
        auto testLambda = [w_transporteurName]( const std::string& aStr2Check)
        {
           return boost::contains( aStr2Check, w_transporteurName);
        };
        if( std::any_of( m_veCmdStr.cbegin(), m_veCmdStr.cend(), testLambda))
        {
          return true;
        }
      }//while-loop

      return false;
#endif
    }

    // compatibility with Qt 
    QStringList getCmdList() const
    { 
      return m_qListStrCmd;
    }

    std::vector<QString> asStdVector() const { return m_qListStrCmd.toVector().toStdVector(); }

    bool isEmpty() const { return m_qListStrCmd.empty(); /*m_veCmdStr.empty();*/ }

    bool hasTokens( const std::initializer_list<std::string>& aListofTokens = { "Vivaco", "Notes", "Date" });
    
    // check any blank lines
    bool hasBlankLines() const
    {
      QStringList w_list2Check{ m_qListStrCmd };
      // returns the number of lines removed
      return (w_list2Check.removeAll(QString{ "" }) != 0);
    }

    void removeBlankLine() // pdfMiner conversion insert blank line ???
    {
      // number of lines removed
      auto w_nblinesRem = m_qListStrCmd.removeAll(QString{ "" });
     }

    // comparison operator (boost operator equality_comparable)
    // operator != is defined
    friend bool operator== ( const BoDucCmdText& aCmd1, const BoDucCmdText& aCmd2)
    {
      // Placeholder to check product code
      BoDucFields w_bdField1 {}; // default all set to empty
      BoDucFields w_bdField2 {}; // default all set to empty

      // equal if they have the same Command tag CO123456
      auto w_cmd1TxtIter = aCmd1.getQListIterator();
      while( w_cmd1TxtIter.hasNext())
      {
        auto w_cmdLine = w_cmd1TxtIter.next();
        if( w_cmdLine.contains(QRegExp{ "Date" })) // line token = "Date", line below contains COxxxxx command order
        {
          VictoReader w_prodCode;
          w_prodCode.readProdCode( w_cmdLine.toStdString(), w_bdField1);
          break; // don't need to go further, usually "Date" is at the beginning of the file
        }
      }//while-loop

      auto w_cmd2txtIter = aCmd2.getQListIterator();
      while( w_cmd2txtIter.hasNext())
      {
        auto w_cmdLine = w_cmd2txtIter.next();
        if( w_cmdLine.contains(QRegExp{ "Date" })) // line token = "Date", line below contains COxxxxx command order
        {
            VictoReader w_prodCode;
            w_prodCode.readProdCode(w_cmdLine.toStdString(), w_bdField2);
            break;
        }
      }//while-loop

      return (w_bdField1.m_prodCode == w_bdField2.m_prodCode);

      // NOTE compile error because in the for-loop we pass aCmd1 which is aBoDucCmdText
      //      doesn't support interface STL such as begin
      //      range for-loop use iterator behind the hood, begin() and end() are required
      //      since BoDucCmdtext is not a range, we a compile time error such as begin() not found
#if 0

      // equal if they the same Command tag CO123456
      for( const std::string& aLine : aCmd1)
      {
        if( boost::contains(aLine,"Date")) // line token = "Date", line below contains COxxxxx command order
        {
          VictoReader w_prodCode;
          w_prodCode.readProdCode(aLine, w_bdField1);
        }
      }
      for ( const std::string& aLine : aCmd2)
      {
        if (boost::contains(aLine, "Date")) // line token = "Date", line below contains COxxxxx command order
        {
          VictoReader w_prodCode;
          w_prodCode.readProdCode(aLine, w_bdField2);
        }
      }
      return (w_bdField1.m_prodCode == w_bdField2.m_prodCode);
#endif
      //return false;  debugging purpose
    }

    // friend class to iterate through the text command
   // friend class BoDucCmdIterator;

  private:
    QStringList m_qListStrCmd;

    // shall be in the BdApp class??? still use it?
    bool useTM( const std::vector<std::string>& aVecOfCmdLines)  { return true;  } //default value
    bool useTON( const std::vector<std::string>& aVecOfCmdLines) { return false; } // default value
  };

#if 0
  /** Utility to traverse the BoDuc command text representation*/
  class BoDucCmdIterator
  {
  public:
    BoDucCmdIterator( BoDucCmdText& aBoDucCmdTxt): m_index(0), m_bdCmdTxt(aBoDucCmdTxt) {}
    bool hasNext() const       { return m_index < m_bdCmdTxt.size(); }
    std::string& current()     { return m_bdCmdTxt[m_index++]; }
    std::string  top() const   { return m_bdCmdTxt.m_veCmdStr.front(); }
    std::string& operator++ () { ++m_index; return m_bdCmdTxt[m_index]; }
    // post-increment not need for now 
  private:
    int m_index;               /**< */
    BoDucCmdText& m_bdCmdTxt;  /**< */
  };
#endif

  /** contains a list of command to file.
   * Usage
   *  std::map<filename,BoDucFileListCmdTxt> key is the file name
   *  or better an unordered_map<filename,BoDucFileListCmdTxt> more efficient
  */  
  class BoDucFileListCmdTxt 
  {
    /** Command text format from source (tool that was used to convert into text).
    *  Algorithm are based on source format to locate specific character string.
    *  We need to know from which tool the conversion was done.
    */
    enum class eSrcType
    {
      pdf2csv = 0,       /**< Pdf tool from Acrobat reader*/
      pdfminer2txt = 1,  /**< Pdf miner engine to data extracter*/
      pdf2rcsv = 2       /**< R pdf tool library*/
    };

  public:
  //  BoDucFileListCmdTxt() = default;
    explicit BoDucFileListCmdTxt( QFile& aFile /*eSrcType aSrctext = eSrcType::pdf2csv*/) 
    : m_fileCmd{ aFile },
      //m_fileName( QFileInfo(m_fileCmd).completeBaseName().toStdString()),
      m_nbCmd{ 0 }
    {
      // not much to do for now
    }

    // what about ctor??
    bool add( const BoDucCmdText& aCmdTxt) 
    { 
      ++m_nbCmd; 
      return false; 
    } // debugging purpose

    // move semantic avoid copying large amount of data without creating temporary
    void add( BoDucCmdText&& aCmdTxt);

    //bool remove() { return false; }  based on command number

    size_t nbOfCmd() const 
    {
      return m_nbCmd;
    }
//     const std::string& fileName() const
//     {
//       return m_fileName;
//     }

    QString filename() const 
    {
      QFileInfo w_fInfo(m_fileCmd);
      return w_fInfo.completeBaseName(); 
    }

    const QString& getFileExtension() const
    { 
      QFileInfo w_fInfo(m_fileCmd);
      return w_fInfo.completeSuffix();
    }

    // let's in the meantime if we have the right file extension
    // support .csv and .txt
    bool isFileExtOk( const QStringList& aListOfiles) const;

    QString getFileExt() const
    {
      QFileInfo w_checkFileInfo(m_fileCmd);
      return w_checkFileInfo.completeSuffix();
    }

    // just a test on how we should return element of the list 
    BoDucCmdText first() const { return m_vecOfBDCmd.front(); /*m_listOfCmd2File.front();*/ }
    BoDucCmdText last() const  { return m_vecOfBDCmd.back();}

    QVectorIterator<BoDucCmdText> getIterator() const 
    { 
      return QVectorIterator<BoDucCmdText>( m_vecOfBDCmd); 
    }

//     QMessageBox msgBox1;
//     msgBox1.setText("Close CSV File.");
//     msgBox1.exec();
//     QMessageBox msgBox;
//     msgBox.setText("Open CSV File.");
//     msgBox.exec();

    bool open() const
    {
      if( !m_fileCmd.open(QFile::ReadOnly | QFile::Text))
      {
        return false;
      }
      return true;
    }

    void close() const 
    {
      if( m_fileCmd.isOpen())
        m_fileCmd.close();
    }

    void readCmdFile();

    // Iterator on BoDucCmdText must be supported (boost range iterator)
    // NO Qt iterator such as QVectorIterator
  private:
    QFile& m_fileCmd; /**< */
//    std::string m_fileName; /**< */
    // maybe a forward list?? singly linked list
    std::forward_list<BoDucCmdText> m_listOfCmd2File; /**< */
    // temporary just testing some concept
    QVector<BoDucCmdText> m_vecOfBDCmd;
    size_t m_nbCmd; /**< */
  };
} // End of namespace