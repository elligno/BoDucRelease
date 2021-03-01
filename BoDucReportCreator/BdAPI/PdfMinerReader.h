#pragma once

#include "BoDucCsvReader.h"
#include "BoDucProDescrAnalyzer.hpp"

namespace bdAPI 
{
	class PdfMinerReader : public BoDucReader
	{
	public:
		// alias
		using vecofstr = std::vector<std::string>;
	public:
    PdfMinerReader() = default;
    ~PdfMinerReader() = default;
		
		// Base class methods oerride
		virtual void readNoCmd( const vecofstr& aCmdVec, BoDucFields& aBoducF) override {/*no implemenetation*/}
    virtual void readNoCmd( const std::string& COline, BoDucFields& aBoDucField) override;
		// ... to be completed
		virtual void readShippedTo( const vecofstr& aFieldValue, BoDucFields& aBoducF) override;
		virtual void readDeliveryDate(const std::string& aFiedValue, BoDucFields& aBoducF) override;
		virtual void readProdDescr( const std::string& aCmdVec, BoDucFields& aBoducF) override 
    {
      throw "Not implemented yet";
    }
    void readProdDescr( const BoDucProDescrArea& aProdescrArea, BoDucFields& aBoducF);
		virtual void readQtySilo( const std::string & aFieldValue, BoDucFields& aBoDucField) override
    {
      throw "Not implemented yet";
    }
		virtual void readProdCode( const std::string& aFieldValue, BoDucFields& aBoducF) override {/*no implemenetation*/ }

    // class method
    void readProdDescr( const std::vector<std::string>& aCmdVec, BoDucFields& aBoDucField, std::string aStopWordCriteria="Notes");
	};
} // End of namespace
