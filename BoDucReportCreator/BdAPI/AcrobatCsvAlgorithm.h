#pragma once

#include "ExtractDataAlgorithm.h"

namespace bdAPI
{
  // Design Note:
  //  Not sure if we keep this class, it will be removed in the next re-factoring
  //  Concept is not clear, mainly algorithm to parse the BoDucCmdTxt
	class AcrobatCsvAlgorithm : public ExtractDataAlgorithm
	{
// 		enum class eKeywordType ???
// 		{
// 			CommandNo = 1,
// 			ShippedTo = 2
// 		};
	public:
		AcrobatCsvAlgorithm()=default;
		~AcrobatCsvAlgorithm()=default;
    AcrobatCsvAlgorithm( const AcrobatCsvAlgorithm&) = default;
    AcrobatCsvAlgorithm& operator= (const AcrobatCsvAlgorithm&) = default;

    /** */
		void fillBoDucFields( const std::vector<std::string>& aCmdVec, BoDucFields& aBoDucField) override;
   
    /** */
    QBoDucFields fillBoDucFields(const BoDucCmdText& aCmdText) override { return QBoDucFields{}; }

// 		void getBoDucStruct( std::vector<BoDucFields>& aVec2Fill) override;
// 	private:
// 		std::initializer_list<std::string> m_fieldKey;
	};
} // End of namespace
