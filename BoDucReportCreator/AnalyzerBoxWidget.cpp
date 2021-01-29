// C includes
#include <string.h>
#include <stdlib.h>

// Qt includes
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
// Widget includes
#include "BdAPI/BoDucCmdText.h"
#include "bdAPI/BoDucUtility.h"
#include "bdAPI/BoDucCmdFileReader.h"
#include "AnalyzerBoxWidget.h"

// internal linkage
namespace { // this is a temporary fix, will replaced by an environment variable
  const QString g_pythonScript = R"(F:\EllignoContract\BoDuc\pdfminerTxt\pdfminer-20140328\build\scripts-2.7\pdf2txt.py)";
} // End of namespace

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
#if 0

  QHBoxLayout* w_rowLayout = new QHBoxLayout;

  // replace all those lines above by these below, can i do that?
  QPushButton* w_loadCsvButton = new QPushButton(QString("Load .csv files"));
  w_rowLayout->addWidget(w_loadCsvButton); // stretch =0 and Qt::Alignment=alignment
                                           // connect signal/slot message 
  connect( w_loadCsvButton, SIGNAL(clicked()), this, SLOT(loadCsvFiles()));

  QPushButton* w_loadPdfButton = new QPushButton(QString("Load .pdf files"));
  w_rowLayout->addWidget(w_loadPdfButton); // see comment above
                                           // connect signal/slot message
  connect( w_loadPdfButton, SIGNAL(clicked()), this, SLOT(loadPdfFiles()));

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
  connect( w_settingsButton, SIGNAL(clicked()), this, SLOT(settingPath()));

  // set layout of this box (does Group box taking ownership of the layout??, yes it is)
  QGroupBox* w_analyzerBox = new QGroupBox;
  w_analyzerBox->setLayout(w_rowLayout); // take the owner ship of the HBoxLayout

  return w_analyzerBox;
#endif

  // an alternative of calling "grBox->setLayout(aLayout)"
  // pass the widget to the layout ctor, in both case widget 
  // takes the ownership of layout
  //QGroupBox* w_analyzerBox = new QGroupBox(tr("AnalyzerBox"));
  //QHBoxLayout* w_rowLayout = new QHBoxLayout(w_analyzerBox);
  QHBoxLayout* w_rowLayout = new QHBoxLayout;

  // replace all those lines above by these below, can i do that?
  QPushButton* w_loadCsvButton = new QPushButton(QString("Load .csv files"));
  QFont w_font = w_loadCsvButton->font();
  w_font.setPixelSize(17);
  w_loadCsvButton->setFont(w_font);
  w_rowLayout->addWidget(w_loadCsvButton); // stretch =0 and Qt::Alignment=alignment
  w_rowLayout->addSpacing(70);
  // connect signal/slot message 
  connect(w_loadCsvButton, SIGNAL(clicked()), this, SLOT(loadCsvFiles()));

  QPushButton* w_loadPdfButton = new QPushButton(QString("Load .pdf files"));
  w_font = w_loadPdfButton->font();
  w_font.setPixelSize(17);
  w_loadPdfButton->setFont(w_font);
  w_rowLayout->addWidget(w_loadPdfButton); // see comment above
  w_rowLayout->addSpacing(70);
  // connect signal/slot message
  connect(w_loadPdfButton, SIGNAL(clicked()), this, SLOT(loadPdfFiles()));

  // load type(normal/degel) 
  QPushButton* w_capacitySelectButton = new QPushButton("Normal Load");
  w_font = w_capacitySelectButton->font();
  w_font.setPixelSize(17);
  w_capacitySelectButton->setFont(w_font);
  if (m_capacityLoad != eCptyMode::normal) // default mode
  {
    w_capacitySelectButton->setText(QString("Degel Load"));
  }

  w_rowLayout->addWidget(w_capacitySelectButton); // see comment above
  w_rowLayout->addSpacing(70);

  // connect with a lambda 
  connect( w_capacitySelectButton, &QPushButton::clicked, [this, w_capacitySelectButton]
  {
    if( m_capacityLoad == eCptyMode::normal)
    {
      w_capacitySelectButton->setText(QString("Degel Load"));
      m_capacityLoad = eCptyMode::degel;
      emit capacityLoadChanged(/*m_capacityLoad*/);  // progress bar need this info to compute value
    }
    else
    {
      w_capacitySelectButton->setText(QString("Normal Load"));
      m_capacityLoad = eCptyMode::normal;
      emit capacityLoadChanged(/*m_capacityLoad*/);  // progress bar need this info to compute value
    }
  });

  // settings (combo box to set some of the path needed to make the application work)
  // need pdfminer path (installation of python 2.9 is required and set the path of 
  // the exec to make it work and convert pdf to text)
  QPushButton* w_settingsButton = new QPushButton(QString("Settings"));
  w_font = w_settingsButton->font();
  w_font.setPixelSize(17);
  w_settingsButton->setFont(w_font);
  w_rowLayout->addWidget(w_settingsButton);
  w_rowLayout->addSpacing(70);
  connect(w_settingsButton, SIGNAL(clicked()), this, SLOT(settingPath()));

  // set layout of this box (does Group box taking ownership of the layout??)
  // according to Qt documentation Widget::setLayout(), widget will take the ownership.  
  QGroupBox* w_analyzerBox = new QGroupBox(tr("AnalyzerBox"));
  QFont w_grbFont = w_analyzerBox->font();
  w_grbFont.setPixelSize(20);
  w_analyzerBox->setFont(w_grbFont);
  w_analyzerBox->setLayout(w_rowLayout); // see at the beginning of the method for detail about ownership 

  return w_analyzerBox;
}

void AnalyzerBoxWidget::loadPdfFiles()
{
  // setting default directory to start with 
  // i need to check, which is the root directory of the VS15 project
  //   QString w_defaultDir = QDir::currentPath();

  // now opening file to be processed (name of the file returned)
  QStringList w_filesName = QFileDialog::getOpenFileNames(this, tr("Open File"),
    QDir::currentPath(),
    tr("Text (*.txt *.csv *.pdf)"));

  // parse file to extract command
  bdAPI::BoDucParser w_fileParser(bdAPI::BoDucParser::eFileType::pdf);

  //     QMessageBox msgError;
  //     msgError.setText("CRITICAL ERROR!\nThe McRib is no longer available!");
  //     msgError.setIcon(QMessageBox::Critical);
  //     msgError.setWindowTitle("Danger");
  //     msgError.exec();

  // check for supported file extension
  if (!w_fileParser.isFileExtOk(w_filesName)) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "Wrong file extension selected", "Need to select pdf file?",
      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
      w_filesName = QFileDialog::getOpenFileNames(this, tr("Open File"),
        QDir::currentPath(),
        tr("Text (*.txt *.csv *.pdf)"));
    }
    else { // user doesn't want to continue
           //	qDebug() << "Yes was *not* clicked";
      QApplication::quit();
    }
  }

  // future development (next version) 
  // Use environment variable to set QPenv "BDPY27_ROOT" BoDuc python path
  // then extract info with "getenv_s(...)" 
  // at installation user have to set this variable
  char* w_py27Var = nullptr;
  size_t requiredSize;

  // check if it exist, return number of elements if true
  ::getenv_s( &requiredSize, nullptr, 0, "BDPY27_ROOT");
  if (requiredSize == 0)
  {
    printf("BDPY27_ROOT doesn't exist!\n");
    QMessageBox msgBox;
    msgBox.setText("PYTHON environment (BDPY27_ROOT) variable not set.");
    msgBox.exec();
   // exit(1);
  }

  // since it exist, allocate resource to retrieve it
  w_py27Var = (char*)malloc(requiredSize * sizeof(char));
  if( !w_py27Var)
  {
    // Should be put in a log file
    printf("Failed to allocate memory!\n");
    exit(1);
  }

  // Get the value of the "BDPY27_ROOT" environment variable
  getenv_s(&requiredSize, w_py27Var, requiredSize, "BDPY27_ROOT");
  // shall be put in a log file 
  printf("Original BDPY27_ROOT variable is: %s\n", w_py27Var);

  // BoDuc environment variable for pdf support
  // library path is also required
  char w_py27Lib[100];  //concatenated string
  errno_t ret = ::strcpy_s(w_py27Lib, 100, w_py27Var);
  if( ret!=0)
  {
    // could proceed anymore
    printf("Couldn't copy in memory!\n");
  }
  ret = ::strcat_s( w_py27Lib, 100, "\\Lib");
  if( ret != 0)
  {
    printf("Couldn't set lib path: %s\n", w_py27Var);
  }

  QProcessEnvironment w_procEnv = QProcessEnvironment::systemEnvironment();
  w_procEnv.insert("PYTHONHOME", w_py27Var);
  w_procEnv.insert("PYTHONPATH", w_py27Lib);

  // convert pdf to txt format (swap pdf to txt)
  // Design Note: add a test to check if conversion succeed?
  // return a bool ? true : false or the list of converted files (QStringList)
  // need to call setQProcessEnv()
//   QProcessEnvironment w_procEnv = QProcessEnvironment::systemEnvironment();
//   w_procEnv.insert("PYTHONHOME", "C:\\Python27");
//   w_procEnv.insert("PYTHONPATH", "C:\\Python27\\Lib");

  // convert pdf to txt format
  QStringList w_filesConverted =
    bdAPI::BoDucUtility::convertPdf2Txt(w_filesName, g_pythonScript, w_procEnv, this);

  //QFile::remove();
  // move file to different folder
  //     file.rename( g_pdfilesPath+"/"+ QString("DataValidation")+ "/" +fileInfo.fileName());
  //     auto complPath = fileInfo.absoluteFilePath();
  //     QDir w_wrkDir(g_pdfilesPath);
  //     QFileInfo w_finfo(*w_begList);
  //     auto fileName = w_finfo.fileName();
  //     QFile w_file( w_wrkDir.absolutePath() + "/" + fileName);
  //     QFileInfo w_checkWholePath(w_file);
  //     auto fileCheck = w_checkWholePath.absoluteFilePath();

  //     while (w_begList != w_endList)
  //     {
  //       //  std::cout << "Loading the following files: " << w_begList->toStdString() << std::endl;
  //       w_listFilesName.push_back( w_begList->toStdString());
  //       ++w_begList;
  //     }

  // read files (...) and return list of files (text format) 
  auto w_vecofCmd = bdAPI::BoDucCmdFileReader::readFiles(w_filesConverted);

#if 1
  // deleting txt file (these are temporaries, so might as well to delete it)
  QStringListIterator w_listConvertFilesIter(w_filesConverted);
  while (w_listConvertFilesIter.hasNext())
  {
    QFile file(w_listConvertFilesIter.next());
    QFileInfo fileInfo(file);
    if( !QFile::remove(fileInfo.absoluteFilePath()))
    {
      // we definitively need a log file
      std::cout << "Could not remove file from location\n";
    }
  }

  // debugging purpose
 // std::vector<bdAPI::BoDucFields> w_reportCmd;

  // This represent command extracted from files selected by user.
  // Each file is a map of vector of string (text rep of a command)
  // ready to parse all command and save it to data store
  // list of all files selected by user, check for each command in a file
  for (auto begVecMap = w_vecofCmd.cbegin(); begVecMap != w_vecofCmd.cend(); ++begVecMap)
  {
    // Initialize first command for this file
    // this map represent a file with all the commands  
    std::map<int, std::vector<std::string>>::const_iterator w_cmdFileBeg = begVecMap->cbegin();

    // go through all command in this file (map(int,vecstr))
    while (w_cmdFileBeg != (*begVecMap).cend())
    {
      // list of commands for a given file 
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
      m_reportCmd.push_back(bdField);
      ++w_cmdFileBeg; // ready for next command 
    }//while-loop
  }//for-loop
  if( m_reportCmd.size() < m_reportCmd.capacity())
  {
    m_reportCmd.shrink_to_fit();
  }

#endif //

  if( w_py27Var)
  {
    delete w_py27Var;
    w_py27Var = nullptr;
  }

  // notify that new commands has been loaded 
  emit commandLoaded();
}

void AnalyzerBoxWidget::loadCsvFiles()
{
  using namespace boost;

  // now opening files to be processed (user selection)
  QStringList w_filesName = QFileDialog::getOpenFileNames(this, tr("Open File"),
    QDir::currentPath(),
    tr("Text (*.txt *.csv *.pdf)"));

  // parse file to extract command
  bdAPI::BoDucParser w_fileParser;

  // check for file extension valid
  if (!w_fileParser.isFileExtOk(w_filesName)) {
    QMessageBox();
    return;
  }

#if 0 // want to pass as argument the list of file instead of a list of file name 
  // some tests container of QFile
  //    QFile w_fileTst(*w_begList);
  std::forward_list<std::unique_ptr<QFile>> w_fwdList;
  //    std::list<QFile> w_check;
  //    w_check.emplace_back( QDir::currentPath() + "aa.txt");
  // shall use the emplace_back version (move semantic)
  // build vector in-place directly (more efficient)
  // initialize our list staeting element
  auto iter = w_fwdList.before_begin();
  QStringListIterator w_filesNameIter(w_filesName);
  while( w_filesNameIter.hasNext())
  {
    w_fwdList.emplace_after( iter, std::make_unique<QFile>(w_filesNameIter.next()));
  }
//   auto dist1 = std::distance(w_fwdList.cbefore_begin(),w_fwdList.cend());
//   std::unique_ptr<QFile> w_checkUniq( std::move( w_fwdList.front()));
//   dist1 = std::distance(w_fwdList.cbefore_begin(), w_fwdList.cend());
//   auto begFwd = w_fwdList.begin();
//   
//   std::unique_ptr<QFile> w_checkUniq( std::move(*begFwd));
//   QFileInfo w_checkFile(*w_checkUniq);
//   auto fileName = w_checkFile.fileName();
//   ++begFwd;
//   std::unique_ptr<QFile> iterpp = std::move(*begFwd);
//   QFileInfo chcaa(*iterpp);
//   auto nammeF = chcaa.fileName();

  // now we are ready t read files of command
  std::vector<bdAPI::BoDucFileListCmdTxt> w_test123 = bdAPI::BoDucCmdFileReader::readFiles(w_fwdList);
  //++iter;
  bdAPI::BoDucFileListCmdTxt w_sanityCheck = w_test123.back();
  auto checkFileName = w_sanityCheck.fileName();
  auto w_checkNbCmd = w_sanityCheck.nbOfCmd();
#endif

  // store extracted data
//  std::vector<bdAPI::BoDucFields> w_reportCmd;

  // loop on each files in the list (fill vector map for processing multiple files in one step)
  // each map contains all command readed from a file (reminder: a can contains 1 or more command)
  auto w_vecofCmd = bdAPI::BoDucCmdFileReader::readFiles(w_filesName/*, std::string("Signature")*/);

  // This represent command extracted from files selected by user.
  // Each file is a map of vector of string (text rep of a command)
  // ready to parse all command and save it to data store
  // list of all files selected by user, check for each command in a file
  for (auto begVecMap = w_vecofCmd.cbegin(); begVecMap != w_vecofCmd.cend(); ++begVecMap)
  {
    // Initialize first command for this file
    // this map represent a file with all the commands  
    std::map<int, std::vector<std::string>>::const_iterator w_cmdFileBeg = begVecMap->cbegin();

    // go through all command in this file (map(int,vecstr))
    while (w_cmdFileBeg != (*begVecMap).cend())
    {
      // list of commands for a given file 
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

      // use the std any algorithm (with a lambda as the predicate to find
      //if any of those are present in files) 
      auto bdField = w_fileParser.extractData(w_checkCmd);
      m_reportCmd.push_back(bdField);
      ++w_cmdFileBeg; // ready for next command 
    }//while-loop
  }//for-loop
  if( m_reportCmd.size() < m_reportCmd.capacity())
  {
    m_reportCmd.shrink_to_fit();
  }

  // notify that new commands has been loaded 
  emit commandLoaded();
}

void AnalyzerBoxWidget::settingPath()
{
  throw "Not implemented yet";
}
