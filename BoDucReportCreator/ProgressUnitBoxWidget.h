#pragma once

// Qt includes
#include <QList>
#include <QWidget>
#include <QStringList>

// forward declaration
class QGroupBox;
class QProgressBar;

// not suer if we gonna keep it
#include "BoDucReportCreator.h"

/** Widget for unit progress bar.
*  This group box is made of progress bar
*  showing load capacity.
*/
class ProgressUnitBoxWidget : public QWidget
{
  Q_OBJECT

  // make code look cleaner
  using eCptyMode = bdApp::BoDucReportCreator::eCapacityMode;
  using tplunitAndLoad = std::tuple<unsigned/*unit no*/, double/*normal load*/, double/*degel load*/>;

public:

  /** */
  ProgressUnitBoxWidget( const QStringList& aListOfUnit,  QWidget* parent = Q_NULLPTR);

  public slots:
  void setUnitProgressBar( const QString& aUnitOn, double aVal2Update);

  /** to be completed*/
//  QGroupBox* createProgressUnitBox();
private:
  // need to be fix, this variable is set to many places, hard to keep tracking
  static constexpr auto m_nbUnit = 10;           /**< */
  int m_unitIndex;
  eCptyMode m_capacityLoad;
  QStringList m_listUniteAvailable;              /**< */
  QProgressBar* m_progressBar[m_nbUnit];         /**< */
  QList<tplunitAndLoad> m_listLoadValuesPerUnit; /**< */

  /** */
  void unitLoadConfig();
  QGroupBox* createProgressUnitBox();
  QGroupBox* createVerticalProgressBarBox();
  void addProgressBar( QBoxLayout* aProgressBar, const std::string& aUniteNb);
  void setUnitCapacityLoad();
};