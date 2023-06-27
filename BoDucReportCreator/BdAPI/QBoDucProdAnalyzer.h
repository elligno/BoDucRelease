#pragma once

// C++ include
#include <cctype>

// Qt includes
#include <QObject>
#include <QString>
#include <QStringList>

namespace bdAPI {

  class BoDucProDescrArea : public QObject
  {
    Q_OBJECT

  public:
        
    // 2-lines description in RCsvAlgorithm
    BoDucProDescrArea( QString&& aFirstLine, QString&& aLinesArea)
    : m_firstLine{ std::move(aFirstLine) }, m_secondLine{ std::move(aLinesArea) }
    {
      m_splittedLines = m_firstLine.split(QString{ " " });
    }
  
    // Token: "Notes :", "Vivaco", "???" or any other tokens
    bool isSecondLineHasTokens() const
    {
      return (m_secondLine.contains(QString{ "Vivaco" }) ||
        m_secondLine.contains(QString{ "Notes" }) ||
        m_secondLine.contains(QString{ "Info" }) ||
        m_secondLine.contains(QString{ "Victoriaville" }));
    }

    // format is usually "123456SR" digit + "SR"
    bool isSecondLineEndsWithSR() const
    {
      return m_secondLine.endsWith(QString{ "SR" });
    }

    bool isSecondLineStartWithDigit() const
    {
      return std::isdigit(m_secondLine.toStdString()[0]);
    }

    bool isSecondLineAllDigits() const
    {
      auto w_stdStr = m_secondLine.toStdString();
      return std::all_of(w_stdStr.cbegin(), w_stdStr.cend(), [](unsigned char aChar)
      {
        return std::isdigit(aChar);
      });
    }

//     // RCsv second line check
    bool hasVivacoTokens() const        { return m_secondLine.contains(QString{ "Vivaco" }); }
    bool hasVictoriavilleTokens() const { return m_secondLine.contains(QString{ "Victoriaville" }); }
    bool hasInfoTokens() const          { return m_secondLine.contains(QString{ "Info" }); }
    bool hasNotesTokens() const         { return m_secondLine.contains(QString{ "Notes" }); }

    QString getProdCode()
    {
      // cmd number usually 1, only 1 command. I've seen
      if (m_splittedLines.first().toInt() == 1 && m_splittedLines[1].toLong() != 0)
      {
        return m_splittedLines.at(1);
      }
    }

    QStringList getProdDescription()
    {
      QStringList w_prodDescr{ m_splittedLines };
      // remove white space
      w_prodDescr.removeDuplicates();
      w_prodDescr.pop_front(); // product code
      w_prodDescr.pop_front(); // product number
                               // check empty 
      if (w_prodDescr.first().isEmpty())
      {
        w_prodDescr.pop_front();
      }

      // need to check for negative (assume not equal to -1)
      auto w_idxTON_TM = indexOfTM_TON(w_prodDescr);

      QStringList w_retProdDescr;
      w_retProdDescr.reserve(5);

      // check
      //aStrList.reserve(aStrList.size());
      std::copy(w_prodDescr.cbegin(), w_prodDescr.cbegin() + w_idxTON_TM, std::back_inserter(w_retProdDescr));

      // check for second line
      if (!isSecondLineHasTokens() && !isSecondLineEndsWithSR())
      {
        // add it to the description
        w_retProdDescr.push_back(m_secondLine);
      }
    }

  private:
    // RCsvAlgorithm we have 2 lines (working on those 2-lines)
    QString m_firstLine;
    QString m_secondLine;
    QStringList m_splittedLines;

    int indexOfTM_TON(const QStringList& aStrList)
    {
      auto w_idxTON_TM = -1;
      // check 'TM' pos in the whole
      if (aStrList.indexOf(QRegExp{ QString{ "TM" } }) != -1)
      {
        w_idxTON_TM = aStrList.indexOf(QRegExp{ QString{ "TM" } });
      }
      else if (aStrList.indexOf(QRegExp{ QString{ "TON" } }) != -1)
      {
        w_idxTON_TM = aStrList.indexOf(QRegExp{ QString{ "TON" } });
      }

      return w_idxTON_TM;
    }
  };
}// End of namespace
