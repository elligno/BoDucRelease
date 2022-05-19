#pragma once
// C++ include
#include <sstream>
// App include
#include "RCsvAlgorithm.h"

namespace bdAPI
{
  void bdAPI::RCsvAlgorithm::fillBoDucFields( const std::vector<std::string>& aCmdVec, BoDucFields & aBoDucField)
  {
    throw "No implementation";
  }

  // TODO: need implementation of the R csv algorithm 
  QBoDucFields RCsvAlgorithm::fillBoDucFields( const BoDucCmdText& aCmdVec)
  {
    using namespace std;

    // this is a temporary fix because we have some problem 
    // with the list initializer
    // Actually shall be set by some methods and not ficx in the code
    // if those keywords change 
//     vector<string> w_keyWords = { string("No Command"),string("Shipped to"),
//       string("Date promise"),string("produit et description") };

    // R csv format is different from Acrobat csv format, it convert the pdf
    // in the same appearance of the pdf, text same at the place. Csv rendering
    // is exactly the same as the pdf rendering. Algorithm for parsing the command
    // goes sequentially (take one keyword after one)

    QBoDucFields w_bdFields;

    // loop over each line of the command and check if the line contains the given keyword. 
    std::cout << "This cmd need to be treated\n";
    QStringList w_keywords{ QString{ "Ordered on" } , QString{ "Shipped to" }, 
                            QString{ "Date promise" }, QString{ "Qty ordered" } };
    
    auto w_strListIdx{ 0 }; // keyword index
    auto w_checkSiz = aCmdVec.size(); // number of lines don't work!!
    
    QStringListIterator w_cmdIter{ aCmdVec.asQStringList() };
    while( w_cmdIter.hasNext()) // iterate over each cmd line
    {
      auto w_currLine = w_cmdIter.next();
      if (w_currLine.contains( QString{ w_keywords[0] }))
      {
        auto w_cmdNo = w_currLine.split(" ").back();
        //  auto w_cmdNo = w_strList.back();
        if (w_cmdNo.startsWith(QString{ "CO" }))
        {
          std::cout << "Cmd number is: " << w_cmdNo.data() << "\n";
        }
        //++w_strListIdx;  next in the list
        w_bdFields.m_noCmd = w_cmdNo;
      }
      if (w_currLine.contains( QString{ w_keywords[1] }))
      {
        QStringList w_addressLines;
        QString w_Line2look4;
        do
        {
          w_Line2look4 = w_cmdIter.next();
          if( w_Line2look4.isEmpty())
          {
            continue;
          }
          auto res = w_Line2look4.split( QString{ "   " }); //3 white spaces as default
          auto lastWord = res.back(); // shall be the address of 
          w_addressLines.push_back(lastWord);
        } while (!w_Line2look4.contains(QString{ "Contact" }));
        w_addressLines.pop_back();
        auto sizeQStrList = w_addressLines.size();
        std::cout << "We have something\n";

        w_bdFields.m_deliverTo = w_addressLines.join(QString{" "});
      }
      if( w_currLine.contains( QString{ w_keywords[2] }))
      {
        auto w_Line2look4 = w_cmdIter.next();
        std::istringstream iss{ w_Line2look4.toStdString() };
        std::vector<std::string> w_splitStr{ std::istream_iterator<std::string>{iss}, // begin range
                                             std::istream_iterator<std::string>{} };  // end range
        for (auto&& w_str2LookAt : w_splitStr)
        {
          if (w_str2LookAt.find_first_of("/") != std::string::npos) // found it?
          {
            bdAPI::BoDucFields w_bdField;
            w_bdField.m_datePromise = w_str2LookAt;
            w_bdFields.m_datePromise = QString{ w_str2LookAt.data() };
            std::cout << "This is the delivery date: " << w_str2LookAt << "\n";
            break;
          }
          continue;
        }
      }
      if( w_currLine.contains( QString{ w_keywords[3] })) // qty ordered
      {
        auto w_Line2look4 = w_cmdIter.next();
        // may be split line
        auto w_strList = w_Line2look4.split(" ");
        if( w_strList.first().toInt() == 1) // cmd number usually 1
        {
          if( w_strList[1].toLong() !=0 ) // succeed
          {
            w_bdFields.m_prodCode = w_strList[1].toLong();
          }
        }

        // Need  to do before duplicate, because product no is 1 (most time)
        // and sometimes silo number is 1, if we duplicate we lose silo no
        auto w_siloNo = w_strList.back(); // silo number
        w_bdFields.m_silo = w_siloNo;

        // remove white space
        w_strList.removeDuplicates();
        w_strList.pop_front(); // product code
        w_strList.pop_front(); // product number
        // check empty 
        if( w_strList.first().isEmpty())
        {
          w_strList.pop_front();
        }
        
        // check 'TM' pos in the whole
        auto w_idxTM = w_strList.indexOf(QRegExp{ QString{"TM"} });
        QStringList w_descrProd; w_descrProd.reserve(w_strList.size());
        std::copy( w_strList.cbegin(), w_strList.cbegin()+w_idxTM, std::back_inserter(w_descrProd));
        // Next line of prod descr
        auto w_nextLineDescr = w_cmdIter.next();
        w_descrProd.push_back(w_nextLineDescr);
        auto w_prodDescrCmplt = w_descrProd.join( QString{ " " }); // whole descr.
        w_bdFields.m_produit = w_prodDescrCmplt;
        
   //     w_strList.pop_back();
        auto w_qtyOrdered = w_strList.back();
        w_bdFields.m_qty = w_qtyOrdered.toFloat();

        // 1 1234567687 ....
//         if (std::isdigit(w_asd[0], std::locale()))
//         {
//           aBoDucField.m_qty = ::atof((*std::next(begVec, 1)).c_str());
//           aBoDucField.m_silo = *std::next(begVec, 4);
//           std::advance(begVec, 1); // next command line
//           std::advance(w_keyWordIter, 1); // next token to look for
//           continue;
//         }
//         else // take that the next 
//         {
          // case where qty ordered and silo inside "prod et descr" area (blais facture)
          // we add 4 lines qty,fab,silo
//          std::vector<std::string> w_prdoDescr(std::next(begVec, 4), aCmdVec.cend()); // cmd number usually 1

          // call prodDecription algo or parser/reader
//           w_boducReader->readProdDescr(w_prdoDescr, aBoDucField); // as it use to be in csv format
//           break;
      }
    }//while-loop

    return w_bdFields;
  }

} // End of namespace

