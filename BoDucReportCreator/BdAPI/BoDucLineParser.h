#pragma once

#include <string>

namespace bdAPI 
{
  /** Parse line of command text.*/
  class BoDucLineParser 
  {
  public:
    /** Set of word used to retrieve data when parsing the command line-by-line*/
    void setLineTokens() {}
    /** Shall return something if found anything*/
    void parse( const std::string& aLine) {}
  };
} // End of namespace
