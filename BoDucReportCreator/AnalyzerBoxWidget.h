#pragma once

// Qt includes
#include <QWidget>

// forward declaration
class QGroupBox;

// not suer if we gonna keep it
#include "BoDucReportCreator.h"
/** Widget for analyze buttons.
 *  This group box is made of push button
*/
class AnalyzerBoxWidget : public QWidget
{
  Q_OBJECT

  // make code look cleaner
  using eCptyMode = bdApp::BoDucReportCreator::eCapacityMode;

  // set the capacity load according to time period of the year
//   enum class eCapacityMode
//   {
//     normal = 0, // default mode (most of the year)
//     degel = 1   // usually spring time
//   };
public:

  /** */
  AnalyzerBoxWidget( QWidget* parent = Q_NULLPTR);

  const std::vector<bdAPI::BoDucFields>& getCommandLoaded() const { return m_reportCmd; }
private:
  /** Top group box, actually it's the original buttons layout
  Open, load, save, process (buttons)*/
  QGroupBox* createAnalyzerBox();

  signals:
    void capacityLoadChanged(/*eCptyMode aCptyMode*/);
    void commandLoaded();          // new commands has been added to file data store 

  public slots:
    //void highlightChecked(QListWidgetItem* item);
    /** */
    void loadPdfFiles();
    /** */
    void loadCsvFiles();
    /** */
    void settingPath();
private:
  // 
  std::vector<bdAPI::BoDucFields> m_reportCmd;
  eCptyMode m_capacityLoad = eCptyMode::normal; /**< set capacity load (normal/degel)*/
};