#pragma once

// C++ include
#include <string>
// STL includes
#include <vector>
#include <map>
//Qt includes
#include <QStringList>
// Boost include
#include <boost/algorithm/string.hpp> // string algorithm
// App include
#include "BoDucFields.h"

// forward declaration
namespace bdAPI { class BoDucBonLivraisonAlgorithm; }

namespace bdAPI 
{
	// abstract base class (not sure about that)
  // Design Note:
  //  Need a serious re-factoring, many of the methods are deprecated
  //  New architecture (new types are under construction) which will 
  //  take care of those deprecated methods 
  // void parse(BoDucFileCmd) parse a command file
  //  new type BoDucFileParser
  //  
	class BoDucParser
	{
  public:
    // type of file (command)  
    enum class eFileType
    {
      csv = 0,
      pdf = 1
    };

	public:
		// create alias
		using vecofstr = std::vector<std::string>;
		using mapIntVecstr = std::map<int, vecofstr>; // deprecated

	public:
    //BoDucParser(); not need it for now
    BoDucParser( const eFileType aFilext = eFileType::csv);
		// part of refactoring
		BoDucParser( BoDucBonLivraisonAlgorithm* aReader);
		// copy and assign ctor not allowed
		BoDucParser( const BoDucParser& aOther) = delete;
		BoDucParser& operator= ( const BoDucParser& aOther) = delete;

		// main algorithm to parse the csv file format of BoDuc (deprecated)
		virtual void extractData( const mapIntVecstr& aListOfCmd, BoDucBonLivraisonAlgorithm* aReader);

		// part of refactoring (vector of string reprsent a command)
		virtual BoDucFields extractData( const std::vector<std::string>& aListOfCmd);
		
    // return vector of all cmd (deprecated)
		std::vector<BoDucFields> getBdFields() { return m_bdStruct; }

    // return current file extension
    eFileType getFileExt() const { return m_fileExt; }
 
    // deprecated
    // before extracting let's check so
    bool isTransporteurNameValid( const std::vector<std::string>& aCmdText,
      const std::initializer_list<std::string>& aListTransporteur) const
    {
      // check for carrier name some are different and don't need to be treated
      // Francois mentioned that field can be blank, need to be careful
      if( std::any_of( aCmdText.cbegin(), aCmdText.cend(), m_checkTransportName))
      {
        return true;
      }
      return false;
    }

    // deprecated
    // let's in the meantime if we have the right file extension
    bool isFileExtOk( const QStringList& aListOfiles) const;

    // deprecated
    //  to be completed
    bool hasAnyTM_TON( const std::vector<std::string>& aCmdText);
 
	private:
    // deprecated
		//shall be in the BdApp class
		bool useTM( const std::vector<std::string>& aVecOfCmdLines);

    eFileType m_fileExt; /**< file extension*/

		// part of the refactoring (deprecated)
		BoDucBonLivraisonAlgorithm* m_fieldParserAlgo;  // deprecated
		std::vector<BoDucFields> m_bdStruct; // not sure about this one??

    // deprecated
    // lambda (anonymous) function declaration
    std::function<bool( const std::string &aStr2Look) > m_checkTransportName = []( const std::string& aStr2Look) -> bool
    {
      using namespace boost;

      // Transporteur name (BoDuc)
      return (contains( aStr2Look, "NIR R-117971-3 TRSP CPB INC")
        || contains( aStr2Look, "NIR R-117971-3 TRANSPORT CPB")
        || contains( aStr2Look, "NIR R-117971-3 C.P.B.")
        || contains( aStr2Look, "BODUC- ST-DAMASE")
        || contains( aStr2Look, "NIR R-004489-2 TR. BO-DUC"));     //sometime we have empty (blank field) string
    };
	};
} // End of namespace