#include <iostream>
#include "AddressBuilder.h"

namespace bdAPI 
{
	AddressBuilder::AddressBuilder()
	: m_name(""),
		m_street(""),
		m_city(""),
		m_addressPart()
	{
	}

	AddressBuilder::AddressBuilder( const std::vector<std::string>& aVecofAddrs)
	: m_name(""),
		m_street(""),
		m_city(""),
		m_addressPart()
	{
		if( !aVecofAddrs.empty()) // sanity check
		{
		   m_addressPart.assign( aVecofAddrs.cbegin(), aVecofAddrs.cend());
		}
	}

	AddressBuilder::AddressBuilder( std::vector<std::string>&& aVecofAddrs)
	: m_name(""),
		m_street(""),
		m_city(""),
		m_addressPart(aVecofAddrs)
	{
		std::cout << "we are in AddressBuilder move ctor\n";
	}

	AddressBuilder & AddressBuilder::operator+= ( const std::string & aStr)
	{
		// check for emptiness
		if( !m_addressPart.empty())
		{
			m_addressPart.clear();
		}
		m_addressPart.push_back(aStr);

		// TODO: insert return statement here
		return *this;
	}

	void AddressBuilder::extractAddressFields()
	{
		// field tuple field
		// first line is the client name
		// check address part, usually last line is the address
		if( m_addressPart.size()==2)
		{
			// this case is easy to handle
			std::get<0>(m_addressFields) = m_addressPart[0];
		}
	}

} // End of namespace
