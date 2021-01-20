#pragma once

// C++ includes
#include <string>
#include <vector>
#include <forward_list>
// boost includes
//#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp> // string algorithm
//#include <boost/algorithm/string/split.hpp> // splitting algo
// Qt includes
#include <QFile>
#include <QStringList>
// boost include
#include <boost/operators.hpp>

// // forward declaration
namespace bdAPI {
class BoDucCmdIterator;
}

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
  public:
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
    // what about default, copy and assignment ctor (=default)
    BoDucCmdText( eSrcType aSrctext = eSrcType::pdf2csv) { m_veCmdStr.reserve(50); m_qListStrCmd.reserve(50); }
//     BoDucCmdText( const QFile& aFileName);
    // default ctor will be ok

    void push_line( std::string&& aLine2Mv)
    {
      m_veCmdStr.push_back( std::move(aLine2Mv));
      m_qListStrCmd.push_back( std::move(QString(aLine2Mv.data())));
    }

    void push_line( const std::string& aLine)
    {
      m_veCmdStr.push_back(aLine);
      m_qListStrCmd.push_back(QString(aLine.data()));
    }

    void pop_line()
    {
      m_veCmdStr.pop_back();
      m_qListStrCmd.pop_back();
    }

    // 
    //void find_line_with_token(boost::find_token) { throw "Not implemented yet"; }

    // not sure about this one!!!
    QStringListIterator getQListIterator() const { return QStringListIterator(m_qListStrCmd);     }
    cmdLineIterator getCmdLineIterator()   const { return boost::make_iterator_range(m_veCmdStr); }

    // return a range based on line index (boost range)
    std::vector<std::string> rangeOfLines( size_t begLine, size_t endLine)
    {
      // implement with the boost range library
      //boost::iterator_range<std::vector<std::string>::const_iterator> cmd_range(m_veCmdStr.cbegin() + begLine, m_veCmdStr.cbegin() + endLine);

      return std::vector<std::string>( m_veCmdStr.cbegin()+begLine, m_veCmdStr.cbegin()+endLine);
    }

    std::vector<std::string>::size_type size() const
    {
      // number of lines for this command
      return m_veCmdStr.size();
    }

    /** Implicit conversion operator to be use with our algorithm
    * Our algorithm have been designed as taking std::vectorof string
    * but in our new architecture 
     According to Scott Myers be carefull when using such tricky stuff;*/
    operator std::vector<std::string>() const { return m_veCmdStr; }

    // not sure about those?? be carefull!! "TON" can be part of word like 'BROMPTON'
    // when checking for that given token, special care must be taken
    bool hasTM_TON() const
    {
      using namespace boost::algorithm;

      return !std::any_of( m_veCmdStr.cbegin(), m_veCmdStr.cend(),
        [](const QString& aStr) // check for the "TM" or "TON"
      {
        // check for a valid
        // "BROMPTON" contains "TON", we need some kind of mechanism
        // to check if this a valid code or a string contained 
        return boost::contains(aStr, std::string("TM")) || boost::contains(aStr, std::string("TON"));
      });
    }

    // before extracting let's check so
    bool hasTransporteurNameValid( const std::initializer_list<std::string>& aListTransporteur) const
    {
      auto begList = aListTransporteur.begin();
      while( begList != aListTransporteur.end())
      {
        std::string w_transporteurName = *begList;
        auto testLambda = [w_transporteurName]( const std::string& aStr2Check)
        {
           return boost::contains( aStr2Check, w_transporteurName);
        };
        if( std::any_of(m_veCmdStr.cbegin(), m_veCmdStr.cend(), testLambda))
        {
          return true;
        }
        return false;
      }
    }

    // compatibility with Qt 
    QStringList asQStringList() 
    { 
//       auto beg = m_veCmdStr.cbegin();
//       auto end = m_veCmdStr.cend();
//       while (beg!=end)
//       {
//         m_qListStrCmd.push_back( QString(*beg->data()));
//         ++beg;
//       }

      return m_qListStrCmd;
    }

    bool isEmpty() const { return m_veCmdStr.empty(); }
    bool hasTokens( const std::initializer_list<std::string>& aListofTokens = { "Vivaco", "Notes" });
    bool hasBlankLines() const
    {
      return std::any_of( m_veCmdStr.cbegin(), m_veCmdStr.cend(),
        [] (const std::string& aStr) -> bool
      { return aStr.empty();
      });
    }

    void removeBlankLine() // pdfMiner conversion insert blank line
    {
      m_veCmdStr.erase( std::remove_if( m_veCmdStr.begin(), m_veCmdStr.end(), // delete empty element
        []( const std::string& s) // check if an empty string
      {
        return s.empty();
      }), m_veCmdStr.cend());
    }
    
    void shrinkIt() 
    {
      if( m_veCmdStr.capacity() > m_veCmdStr.size())
      {
        m_veCmdStr.shrink_to_fit();
      }

      // what about QStringList
    }

          std::string& operator[] ( int Idx)       { return m_veCmdStr[Idx]; }
    const std::string& operator[] ( int Idx) const { return m_veCmdStr[Idx]; }

    // comparison operator (boost operator equality_comparable)
    // operator != is defined
    friend bool operator== ( const BoDucCmdText& aCmd1, const BoDucCmdText& aCmd2)
    {
      // Placeholder
      BoDucFields w_bdField1;
      BoDucFields w_bdField2;
      
      // equal if they the same Command tag CO123456
      for( const std::string& aLine : aCmd1)
      {
        if( boost::contains(aLine,"Date"))
        {
          VictoReader w_prodCode;
          w_prodCode.readProdCode(aLine, w_bdField1);
        }
      }
      for ( const std::string& aLine : aCmd2)
      {
        if (boost::contains(aLine, "Date"))
        {
          VictoReader w_prodCode;
          w_prodCode.readProdCode(aLine, w_bdField2);
        }
      }
      return (w_bdField1.m_prodCode == w_bdField2.m_prodCode);
    }

    // friend class to iterate through the text command
    friend class BoDucCmdIterator;

  private:
    QStringList m_qListStrCmd;
    std::vector<std::string> m_veCmdStr;

    // shall be in the BdApp class
    bool useTM( const std::vector<std::string>& aVecOfCmdLines)  { return true;  } //default value
    bool useTON( const std::vector<std::string>& aVecOfCmdLines) { return false; } // default value
  };

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

  /** contains a list of command to file.
   * Usage
   *  std::map<filename,BoDucFileListCmdTxt> key is the file name
   *  or better an unordered_map<filename,BoDucFileListCmdTxt> more efficient
  */  
  class BoDucFileListCmdTxt 
  {
  public:
    // what about ctor??
    void add(  const BoDucCmdText& aCmdTxt);
    void add( BoDucCmdText&& aCmdTxt);
    void remove(); // based on command number

    size_t nbOfCmd() const;

    const std::string& fileName() const
    {
      return m_fileName;
    }

    const QString& filename() const { return m_fileCmd.fileName(); }
    void getFileExtension() const { return; }

    // let's in the meantime if we have the right file extension
    // support .csv and .txt
    bool isFileExtOk( const QStringList& aListOfiles) const;

    // Iterator on BoDucCmdText must be supported (boost range iterator)
  private:
    QFile m_fileCmd;
    std::string m_fileName;
    // maybe a forward list?? singly linked list
    std::forward_list<BoDucCmdText> m_listOfCmd2File;
  };
} // End of namespace