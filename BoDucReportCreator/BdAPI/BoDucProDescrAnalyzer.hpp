#pragma once

// C++ includes
#include <vector>
#include <string>
// Boost includes
#include <boost/range/iterator_range.hpp>
//#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string.hpp> // string algorithm
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/adaptor/filtered.hpp>

namespace bdAPI 
{
  template <typename T>
  T lexical_cast( const std::string& str)
  {
    T var;
    std::istringstream iss;
    iss.str(str);
    iss >> var;
    // deal with any error bits that may have been set on the stream
    return var;
  }

  class BoDucProDescrArea 
  {
    // alias for code clarity
    using vecstr = std::vector<std::string>;
    using vecstr_iter = vecstr::iterator;
    using vecstr_citer = vecstr::const_iterator;
    using vecstr_riter = vecstr::reverse_iterator;
    using vecstr_criter = vecstr::const_reverse_iterator;

  public:
    // STL-like interface
    vecstr_iter begin() { return m_proDescrArea.begin();  }
    vecstr_iter end()   { return m_proDescrArea.end();    }
    vecstr_citer begin() const   { return m_proDescrArea.cbegin(); }
    vecstr_citer end()   const   { return m_proDescrArea.cend();   }
    vecstr_riter rbegin()        { return std::make_reverse_iterator( m_proDescrArea.end());     }
    vecstr_riter rend()          { return std::make_reverse_iterator( m_proDescrArea.begin()+1); }
    vecstr_criter rbegin() const { return std::make_reverse_iterator( m_proDescrArea.cend());    }
    vecstr_criter rend() const   { return std::make_reverse_iterator( m_proDescrArea.cbegin() + 1); }

  public:
    // do we need a default ctor (vector initialization)
    // move semantic is appropriate here because the area range is not useful anymore  
    BoDucProDescrArea( std::vector<std::string>&& aProddescrArea) : m_proDescrArea( std::move(aProddescrArea)) {}
    std::string lastLine()  const  { return m_proDescrArea.back();  }
    std::string firstLine() const { return m_proDescrArea.front(); }
    
    // Token: "Notes :", "Vivaco", "???" or any other tokens
    // in some file, prod descr is mess up with other fields
    // we need to know before processing 
    bool hasTokens( const std::string& aToken) const  
    { 
      auto begSearch = begin();
      do 
      {
        ++begSearch;
      } while( !boost::contains( *begSearch,aToken) && (begSearch != end()-1));
      if (begSearch != end()-1)
      {
        return true;
      }
      return false;
    }

    bool isTokensEndWith() const
    {
      auto begSearch = begin(); // begin of the range
      do // check each line and stop string end "SR" 
      {
        begSearch++; // incr iterator
      } while( boost::ends_with(*begSearch, "SR"));

      // check if the beginning of the string is digit (look for "123456SR")
      if( begSearch != end())
      {
        // check if it start with digit
        auto foundS = std::find_if( begSearch->cbegin(), begSearch->cend(), [] (char c)
        {
          return (c == 'S');
        });

        auto w_isDigit = std::all_of( begSearch->cbegin(), foundS, [] ( unsigned char c)
        {
          return std::isdigit( c, std::locale());
        });
        if( w_isDigit)
        {
          return true;
        }
      }
      return false;
    }

    // area size in terms of lines
    std::vector<std::string>::size_type size() const { return m_proDescrArea.size(); }

    // count lines before we hit a specified token
    std::size_t countLineBeforeToken( const std::string& aToken) 
    {
      // set to "Notes :" for debug purpose
      auto foundOne = std::find_if( begin(), end(), []( std::string aStr)
      {
        return boost::equals( aStr,"Notes :");
      });

      if( foundOne != end())
      {
        return std::distance( begin(), foundOne);
      }
      return 0; // something wrong??
    }

    // reverse iterator
    std::size_t rcountLineBeforeToken( const std::string& aToken)
    {
      return 0; // debugging purpose
    }

    // bool hasVivaco() const { return false; }
  private:
    std::vector<std::string> m_proDescrArea;
  };

  // Should i pass a range (Not sure yet of the whole concept)
  // but something that check validity of the area format
  // Below  a representation of the data to be extracted
  // sometimes data is messed up and we need to do some clean-up
  // 1 123243 prod descr 
  //
  class BoDucProDescrAnalyzer 
  {
  public:
    enum class eAreaPattern 
    {
      None=0,
      Notes=1,       // Notes :
      Vivaco=2,      // Vivaco
      xxxxxSR=3,     // 123456SR
      VivacoNotes=4, // Vivaco/Notes :
      NotesSR=5,     // 123456SR/Notes :
      areaSrewup=6   // ..
    };

  public:
    // ... to be completed
    BoDucProDescrAnalyzer(): m_stopCriteria() {}

    // check in the book The C++ template: complete guide
    // about passing arg as reference or value 
    // Remember about template type deduction
    template<typename Range> 
    void analazeProDescrArea( Range& aRng)
    {
      // command numbering (1)
      auto w_checkCmdNo = std::begin(aRng)+1;
      std::string w_one = *w_checkCmdNo;
      boost::trim_left(w_one);
      auto w_cmd1 = bdAPI::lexical_cast(w_one);
//       if( w_cmd1 != 1)
//       {
//         // do something
//     //    m_errCode = errType::wrongCmdNo;
//       }

      // an alternative
      int cmdno = std::stoi(w_one);

      // position at product number which is second line after command number 
      // which always 1 (1 command by billing), i saw one case there was more than one
      auto w_prodCodeStr = *std::next( std::begin(aRng) + 2); // 1 xxxxxxx 
      
      // all digits??
      auto w_isDigit = std::all_of( w_prodCodeStr.cbegin(), w_prodCodeStr.end(), [] ( unsigned char c)
      {
        return std::isdigit( c, std::locale());
      });

//       if( false == w_isDigit)
//       {
//         // do something whatever it is
//       }

      // now next line can be screw up, 
      // check 2 lines if they are digits string 1 123321
//       while( std::begin(aRng) != std::end(aRng))
//       {
//       }
      // check next lines for Notes
      // check 
    }

    eAreaPattern getAreaProdescrPattern() const { return m_areaPattern; }

    eAreaPattern startCriteriaPattern( const BoDucProDescrArea& aAreaDescr)
    {
      // now ready to do basic check
      if( aAreaDescr.hasTokens( std::string("Notes :")))
      {
        if (aAreaDescr.hasTokens( std::string("Vivaco")))
        {
          m_areaPattern = eAreaPattern::VivacoNotes;
          return m_areaPattern;
        }
        else if( aAreaDescr.isTokensEndWith())
        {
          m_areaPattern = eAreaPattern::NotesSR;
          return m_areaPattern;
        }
        else
        {
          m_areaPattern = eAreaPattern::Notes;
          return m_areaPattern;
        }
      }
      return eAreaPattern::None;
    }

#if 0
    // check in this range "Vivaco" and "Notes"
    // where they are located in the range, in  the middle or at the end?
    // boost::iterator_range<std::string::iterator>
    //template<typename Range>
    void areaProDescr( Range aStartIter)
    {
      // i do not remember what i want to do? define an iterator
#if 0
      using boost_iter_rng = boost::iterator_range<range_iterator<Range::iterator>::type>;
     
      // iterator 
      boost_iter_rng w_rngIter = boost::make_iterator_range( aRange2Look);
      boost_iter_rng w_setPair( boost::begin(aRange2Look), boost::ends(aRange2Look));
      auto rngSiz = w_rngIter.size();
      w_rngIter.advance_begin(1); // advance 
      w_rngIter.advance_end(-1);
      rngSiz = w_rngIter.size();  // smaller range i believe
      auto myIter = *w_rngIter;   // 
#endif

      // range and looking for "Ce chargement" substring in the range
      // find_if returns iterator position in the rng
      // initially rng has a size rng-1 contains Notes or Vivaco??
      // sometimes we have both, check rng-2
      // if so we have the stepping criteria
      // start from the beginning to stopping criteria and gather data
      // this means check each line for the "Ce chargement"
      // stopping criteria is "Ce chargement" as initial value
      // by stepping back from this we check for another criteria

//       auto rbegRng = std::make_reverse_iterator( std::end(aRange2Look));
//       auto rendRng = std::make_reverse_iterator( std::begin(aRange2Look)+1);
//       do {
//         rbegRng++;
//       } while( !boost::contains( *rbegRng, "Ce chargement") && rbegRng != rendRng);

//       if( rbegRng != rendRng)
//       {
        // set the stop criteria (initialize)
        //auto w_stopCriteria = rbegRng;

//         if( boost::iequals( *std::next(rbegRng, 1), "Vivaco") 
//          || boost::iequals( *std::next(rbegRng, 1), "Notes :"))
//         {
//           // go to the next level
//           //w_stopCriteria = rbegRng+1;
//           if( boost::iequals( *std::next(rbegRng, 2), "Vivaco")
//            || boost::iequals( *std::next(rbegRng, 2), "Notes :"))
//           {
            // at this point it means that both are present, we must
            // start from beg + 3 (prod descr start)
            auto w_startProdDescr = rbegRng + 3;
         //   auto distFromBeg = std::distance( w_startProdDescr, rendRng);
            if (distFromBeg == 3) // one-line description
            {
              std::vector<std::string> w_retProdescr = { *w_startProdDescr };
       //       return w_retProdescr;
            }
            else //two-lines description
            {
              // std::advance(w_stopCriteria,1);
              std::deque<std::string> w_retProdescr;
              w_retProdescr.push_front(*w_startProdDescr++);
              w_retProdescr.push_front(*w_startProdDescr);

              // Reminder: they are in reverse order (last to first line)
//               std::deque<std::string> w_revOrder(w_retProdescr.size());
//               std::copy( w_retProdescr.cbegin(), w_retProdescr.cend(), std::front_inserter(w_revOrder));
              
     //         return std::vector<std::string>(w_retProdescr.cbegin(), w_retProdescr.cbegin()+w_retProdescr.size());
            }
//           }
//           else // "Notes :" or "Vivaco" is present but it may also contains "xxxxxSR" pattern 
//           {
//             // Notes : or Vivaco alone we are at the end of our search
//             // check number of lines from start
//             auto distFromBeg = std::distance( rendRng, ++rbegRng);
//             // 2 case: 3 or 4  (descr has 1 line or 2 lines), more than that 
//             // prod and descr has 2 line anyway
//           }
//         }
//         else // didn't found "Notes :" or "Vivaco"
//         {
//           // check for digit string with the following format: "xxxxxSR" or "xxxxxxx"
//           if( boost::ends_with( *std::next(rbegRng,1), "SR"))
//           {
//             auto w_startProdDescr = rbegRng + 1; // first line of descr
//             // evaluate distance
//             auto distFrom = std::distance(w_startProdDescr, rendRng);
//           }
//           // none of those tokens are present: "Notes :", "Vivaco", "xxxxxxSR"
//           auto w_startProdDescr = rbegRng + 1; // first line of descr
//           std::deque<std::string> w_retProdescr;
//           w_retProdescr.push_front(*w_startProdDescr++);
//           w_retProdescr.push_front(*w_startProdDescr);
//         }
   //   }

      // set stopping criteria
//       std::string::iterator w_stopcriteria = begRng;
//       std::prev( w_stopcriteria, 1);
//       boost::contains( *std::prev( w_stopcriteria, 1), "Vivaco");
//       boost::contains( *std::prev( w_stopcriteria, 2), "Notes");

      // Could use the boost "find_backward" algorithm
      // like a reverse iterator but easier to use
      // need to make some test (boost::find_backward_if)
      // pass the boost range as argument  

      // IMPORTANT 
      //These overloads of find_backward return an iterator to the last element that is equal to x in [first, last) or r, respectively

      // could make an iterator_range from [begin,find_backward]
      // cover the prod description range

//       auto w_first = std::make_reverse_iterator( std::end(aRange2Look));
//       auto w_last  = std::make_reverse_iterator( std::begin(aRange2Look));
//       while( w_first != w_last)
//       {
//       }
//         if( boost::iequals(*w_begPos, "Vivaco") || !boost::iequals(*w_begPos, "Notes :"))
//         {
//           break;
//         }
// 
//       do
//       {
//         w_begPos++;
//       } while( !boost::iequals( *w_begPos, "Vivaco") || !boost::iequals( *w_begPos, "Notes :"));


      // now we have set the stopping criteria  
      // start from the beginning to the end
    //  return std::vector<std::string>(); 
    }
#endif

    std::vector<std::string> productAndDescription( const BoDucProDescrArea& aAreaDescr, 
      const BoDucProDescrAnalyzer::eAreaPattern aPattern) 
    {
      // should 
      auto rbegRng = aAreaDescr.rbegin();
      auto rendRng = aAreaDescr.rend();
      do {
        rbegRng++;
      } while ( !boost::contains(*rbegRng, "Ce chargement") && rbegRng != rendRng);

      // switch case for pattern
      switch( m_areaPattern)
      {
      case eAreaPattern::None:
        break;
      case BoDucProDescrAnalyzer::eAreaPattern::VivacoNotes :
      {
        auto dist = std::distance( rbegRng, rendRng); // bottom to start "prod et descr"
  //      auto rngPair = std::make_pair( boost::rbegin( aAreaDescr.begin()), boost::rend( aAreaDescr.end()));
//         // not sure anymore, lost in my thinking 
//         auto someIter = boost::make_iterator_range( rngPair, dist + 3, 0);
        if( dist >= 5)
        {
          auto w_startProdDescr = rbegRng + 3;
          std::deque<std::string> w_retProdescr;
          w_retProdescr.push_front( *w_startProdDescr++);
          w_retProdescr.push_front( *w_startProdDescr);
          return std::vector<std::string>( w_retProdescr.cbegin(), w_retProdescr.cend());
        }
        else
        {
          std::vector<std::string> w_prodescr;  //(boost::make_iterator_range(boost::rbegin(), dist + 3, 0));
          w_prodescr.push_back( *(rbegRng+1));
          return w_prodescr;
        }
        //return getNotesVivacoVec( boost::make_iterator_range( boost::rbegin(), dist+3, 0)); // 
        //break;
      }
      case BoDucProDescrAnalyzer::eAreaPattern::Notes :
      {
        auto dist = std::distance( rbegRng, rendRng); // bottom to start "prod et descr"
        auto checkPos = boost::equals( *(rbegRng+1), "Notes :");
        if( dist > 5)
        {
          auto w_startProdDescr = rbegRng + 1;
          std::deque<std::string> w_retProdescr;
          w_retProdescr.push_front( *w_startProdDescr++);
          w_retProdescr.push_front( *w_startProdDescr);
          return std::vector<std::string>(w_retProdescr.cbegin(), w_retProdescr.cend());
        }
        else
        {
          std::vector<std::string> w_prodescr;  //(boost::make_iterator_range(boost::rbegin(), dist + 3, 0));
          w_prodescr.push_back(*(rbegRng + 1));
          return w_prodescr;
        }
      }
      default:
        return std::vector<std::string>();
        break;
      }//switch-case

      return std::vector<std::string>();
    }

    // not sure anymore
//     std::vector<std::string> NotesSRAlgo() {}
//     std::vector<std::string> NotesAlgo() {}
//     std::vector<std::string> VivacoAlgo() {}

  private:
    std::string m_stopCriteria;
    eAreaPattern m_areaPattern;
  };
} // End of namespace