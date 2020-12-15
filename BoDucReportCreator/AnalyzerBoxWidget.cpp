
// Qt includes
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
// Widget include
#include "AnalyzerBoxWidget.h"
#include "bdAPI/BoDucCmdFileReader.h"

AnalyzerBoxWidget::AnalyzerBoxWidget( QWidget* parent /*= Q_NULLPTR*/) 
: QWidget(parent)
{
  QGroupBox* w_rowButtonsBoxLayout = createAnalyzerBox();
  QHBoxLayout* w_layout = new QHBoxLayout;
  w_layout->addWidget(w_rowButtonsBoxLayout);
  setLayout(w_layout); // takes the ownership  
}

QGroupBox* AnalyzerBoxWidget::createAnalyzerBox()
{
  QHBoxLayout* w_rowLayout = new QHBoxLayout;

  // replace all those lines above by these below, can i do that?
  QPushButton* w_loadCsvButton = new QPushButton(QString("Load .csv files"));
  w_rowLayout->addWidget(w_loadCsvButton); // stretch =0 and Qt::Alignment=alignment
                                           // connect signal/slot message 
  connect(w_loadCsvButton, SIGNAL(clicked()), this, SLOT(loadCsvFiles()));

  QPushButton* w_loadPdfButton = new QPushButton(QString("Load .pdf files"));
  w_rowLayout->addWidget(w_loadPdfButton); // see comment above
                                           // connect signal/slot message
  connect(w_loadPdfButton, SIGNAL(clicked()), this, SLOT(loadPdfFiles()));

  // load type(normal/degel) 
  QPushButton* w_capacitySelectButton = new QPushButton("Normal Load");
  if( m_capacityLoad != eCptyMode::normal) // default mode
  {
    w_capacitySelectButton->setText(QString("Degel Load"));
  }

  w_rowLayout->addWidget(w_capacitySelectButton); // see comment above
  connect(w_capacitySelectButton, &QPushButton::clicked, [this, w_capacitySelectButton]
  {
    if (m_capacityLoad == eCptyMode::normal)
    {
      w_capacitySelectButton->setText(QString("Degel Load"));
      m_capacityLoad = eCptyMode::degel;
      emit capacityLoadChanged(m_capacityLoad);  // should i pass as argument capacity enum?
                                                 // progress bar need this info to compute value
    }
    else
    {
      w_capacitySelectButton->setText(QString("Normal Load"));
      m_capacityLoad = eCptyMode::normal;
      emit capacityLoadChanged(m_capacityLoad);  // should i pass as argument capacity enum?
                                                 // progress bar need this info to compute value
    }
  });

  // settings (combo box to set some of the path needed to make the application work)
  // need pdfminer path (installation of python 2.9 is required and set the path of 
  // the exec to make it work and convert pdf to text)
  QPushButton* w_settingsButton = new QPushButton(QString("Settings"));
  w_rowLayout->addWidget(w_settingsButton);
  connect(w_settingsButton, SIGNAL(clicked()), this, SLOT(settingPath()));

  // set layout of this box (does Group box taking ownership of the layout??)
  QGroupBox* w_analyzerBox = new QGroupBox;
  w_analyzerBox->setLayout(w_rowLayout);

  return w_analyzerBox;
}

void AnalyzerBoxWidget::loadPdfFiles()
{
  throw "Not implemented yet";
}

void AnalyzerBoxWidget::loadCsvFiles()
{
  // emit a signal about comamnd report has been updated
  using namespace boost;

  // no need to do that, it's done when reading file,
  // transporteur name is checked and command discarded 
  /// if does not appear.
  //     std::initializer_list<std::string> w_lisTransporteur =
  //     {
  //       "NIR R-117971-3 TRSP CPB INC",
  //       "NIR R-117971-3 TRANSPORT CPB",
  //       "NIR R-117971-3 C.P.B.",
  //       "BODUC- ST-DAMASE",
  //       "NIR R-004489-2 TR. BO-DUC"     //sometime we have empty (blank field) string
  //     };

  // now opening files to be processed (user selection)
  QStringList m_filesName = QFileDialog::getOpenFileNames(this, tr("Open File"),
    QDir::currentPath(),
    tr("Text (*.txt *.csv *.pdf)"));

  // parse file to extract command
  bdAPI::BoDucParser w_fileParser;

  // check for file extension valid
  if( !w_fileParser.isFileExtOk(m_filesName)) {
    QMessageBox();
    return;
  }

  // parse each commands file 
  std::list<std::string> w_listFilesName;
  QStringList::const_iterator w_begList = m_filesName.constBegin();
  QStringList::const_iterator w_endList = m_filesName.constEnd();
  while (w_begList != w_endList)
  {
    //  std::cout << "Loading the following files: " << w_begList->toStdString() << std::endl;
    w_listFilesName.push_back(w_begList->toStdString());
    ++w_begList;
  }

  // loop on each files in the list (fill vector map for processing multiple files in one step)
  // each map contains all command readed from a file (reminder: a can contains 1 or more command)
  auto w_vecofCmd = bdAPI::BoDucCmdFileReader::readFiles(w_listFilesName, std::string("Signature"));

  // number of files selected by user
  //m_fileLoaded = w_vecofCmd.size();
  // 		m_bdApp.setNbSelectedFiles(m_fileLoaded);

  // debugging purpose
  std::vector<bdAPI::BoDucFields> w_reportCmd;

  // This represent command extracted from files selected by user.
  // Each file is a map of vector of string (text rep of a command)
  // ready to parse all command and save it to data store
  // list of all files selected by user, check for each command in a file
  for( auto begVecMap = w_vecofCmd.cbegin(); begVecMap != w_vecofCmd.cend(); ++begVecMap)
  {
    // Initialize first command for this file 
    std::map<int, std::vector<std::string>>::const_iterator w_cmdFileBeg = begVecMap->cbegin();

    // go through all command in this file (map(int,vecstr))
    while (w_cmdFileBeg != (*begVecMap).cend())
    {
      std::vector<std::string> w_checkCmd = (*w_cmdFileBeg).second;

      // check for some blank lines when converting from pdf to text
      if (std::any_of(w_checkCmd.cbegin(), w_checkCmd.cend(), [](const std::string& aStr) { return aStr.empty(); }))
      {
        w_checkCmd.erase(std::remove_if(w_checkCmd.begin(), w_checkCmd.end(), // delete empty element
          [](const std::string& s) // check if an empty string
        {
          return s.empty();
        }), w_checkCmd.cend());
      }

      // maybe merge those 2 if
      if (!w_fileParser.hasAnyTM_TON(w_checkCmd))
      {
        ++w_cmdFileBeg; // ready for next command
        continue;
      }

      // Need to check for tonnage tags: "TM" or "TON"
      //         if( !w_fileParser.isTransporteurNameValid(w_checkCmd, w_lisTransporteur))
      //         {
      //           ++w_cmdFileBeg; // ready for next command
      //           continue;
      //         }

      // use the std any algorithm (with a lambda as the predicate to find
      //if any of those are present in files) 
      auto bdField = w_fileParser.extractData(w_checkCmd);
      w_reportCmd.push_back(bdField);
      ++w_cmdFileBeg; // ready for next command 
    }//while-loop
  }//for-loop

  emit commandReportUpdated( w_reportCmd);
}

void AnalyzerBoxWidget::settingPath()
{
  throw "Not implemented yet";
}
