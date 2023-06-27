#include "BoDucCmdText.h"
#include <QTextStream>

namespace bdAPI 
{

  void BoDucFileListCmdTxt::add(BoDucCmdText&& aCmdTxt)
  {
    // m_listOfCmd2File.push_front(aCmdTxt);
     // temporary, not sure if a forward list is appropriate for this task!
     //m_listOfCmd2File.emplace_after( m_listOfCmd2File.before_begin(), aCmdTxt);
    m_vecOfBDCmd.push_back(std::move(aCmdTxt));
    ++m_nbCmd;
  }


  void BoDucFileListCmdTxt::readCmdFile()
  {
    BoDucCmdText w_bdCmdTxt;
    QTextStream w_readText(&m_fileCmd);
    while (!w_readText.atEnd())
    {
      auto w_line = w_readText.readLine();
      auto w_idx = w_line.indexOf(QString(","));
      if (w_idx != 0)
      {
        w_line = w_line.remove(0, w_idx + 1); // +1 to remove the whole string "," 
      }
      if (w_line.startsWith('\"'))
      {
        w_line.remove(QChar('\"'), Qt::CaseInsensitive);
      }
      //auto isWhiteSpace = w_line.startsWith('\"'); // remove " is character at beginning
      //      QString w_test( w_line);
      auto w_trimStr = w_line.trimmed();
      w_bdCmdTxt.push_line(std::move(w_trimStr));

      if (w_line.contains("Signature"))
      {
        // move semantic since we don't need this command anymore
        add(std::move(w_bdCmdTxt));
      }
    }//while-loop
  }


}

