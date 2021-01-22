#pragma once

// API package include
#include "BoDucCmdText.h"

namespace bdAPI 
{
  /** Parse file that contains command*/
  class BoDucFileParser 
  {
  public:
    /** */
    void parse( const BoDucFileListCmdTxt& aFileListCmd);
  };
} // End of namespace
