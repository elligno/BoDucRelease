#pragma once

#include "BoDucBonLivraisonAlgorithm.h"

namespace bdAPI
{
  // Design Note:
  //  Not sure if we keep this class, it will be removed in the next re-factoring
  //  Concept is not clear, mainly algorithm to parse the BoDucCmdTxt
	class VictoBonLivraison : public BoDucBonLivraisonAlgorithm
	{
// 		enum class eKeywordType
// 		{
// 			CommandNo = 1,
// 			ShippedTo = 2
// 		};
	public:
		VictoBonLivraison()=default;
		~VictoBonLivraison()=default;
    VictoBonLivraison( const VictoBonLivraison&) = default;
    VictoBonLivraison& operator= (const VictoBonLivraison&) = default;

		void fillBoDucFields( const std::vector<std::string>& aCmdVec, BoDucFields& aBoDucField) override;
// 		void addBoDucField( const BoDucFields& aField2Add) override;
// 		bool containsBoDucField( const BoDucFields& aField2Look) override;
// 		bool removeBoDucField( const BoDucFields& aField2) override;
// 		size_t getnbOfCmd() override { return m_vecOfStruct.size(); }
// 		void getBoDucStruct( std::vector<BoDucFields>& aVec2Fill) override;
// 	private:
// 		std::initializer_list<std::string> m_fieldKey;
// 		std::vector<BoDucFields> m_vecOfStruct;
	};
} // End of namespace
