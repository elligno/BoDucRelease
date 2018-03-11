#pragma once

#include "BoDucBonLivraisonAlgorithm.h"

namespace bdAPI
{
	class VictoBonLivraison : public BoDucBonLivraisonAlgorithm
	{
		enum class eKeywordType
		{
			CommandNo = 1,
			ShippedTo = 2
		};
	public:
		VictoBonLivraison();
		~VictoBonLivraison();
		void fillBoDucFields( const std::vector<std::string>& aCmdVec, BoDucFields& aBoDucField) override;
		void addBoDucField( const BoDucFields& aField2Add) override;
		bool containsBoDucField( const BoDucFields& aField2Look) override;
		bool removeBoDucField( const BoDucFields& aField2) override;
		size_t getnbOfCmd() override { return m_vecOfStruct.size(); }
		void getBoDucStruct( std::vector<BoDucFields>& aVec2Fill) override;
	private:
		std::initializer_list<std::string> m_fieldKey;
		std::vector<BoDucFields> m_vecOfStruct;
	};
} // End of namespace
