#pragma once

#include <tuple>
#include <string>
#include <vector>
// Boost includes
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

namespace bdAPI 
{
	// Build address with correct format.
	// Helper that concatenate the address fields
	// according to format (3-lines name-street-city)
	// Usage:
	//  AddressBuilder w_builder;
	//  w_builder += w_name;
	//  ...
	class AddressBuilder 
	{
		//create alias
	public:
		using tplofstr = std::tuple<std::string/*name*/, std::string/*??*/, std::string/*city*/>;
	public:
		// default ctor
		AddressBuilder();
		AddressBuilder( const std::vector<std::string>& aVecofAddrs);
		AddressBuilder( std::vector<std::string>&& aVecofAddrs);//move semantic

		// services (not implemented yet)
		std::string name() const   { return m_name;   }
		std::string street() const { return m_street; }
		std::string city() const   { return m_city;   }
		tplofstr asTuple() const
		{
			return std::make_tuple(m_name, m_street, m_city);
		}
		// add address components 
		AddressBuilder& operator+= ( const std::string& aStr);
		std::string buildAddess() const 
		{
			return boost::algorithm::join(m_addressPart, ",");
		}
		void clearAddressBuf()
		{
			if( !m_addressPart.empty())
			{
			   m_addressPart.clear();
			}
		}
		void reserveMem2Store(size_t aNumberOfelem)
		{
			m_addressPart.reserve(aNumberOfelem);
		}
	private:
		std::string m_name;
		std::string m_street;
		std::string m_city;
		std::string m_wholeAddress;
		std::vector<std::string> m_addressPart;
		tplofstr m_addressFields;
		void extractAddressFields();// name, street and city code
		void setName() {} // default implementation
		void setCity() {}
		void setStreet() {}
	};
} // End of namespace
