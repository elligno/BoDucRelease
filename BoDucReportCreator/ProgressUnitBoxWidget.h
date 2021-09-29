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

  /** unit no is the key and value is a pair for normal/degel capacity */
  using unitloadCapact = std::map<int/*unit no*/, std::pair<double/*Normal*/, double/*Degel*/>>;

  //  Should be put in file and loaded at app startup 
  //  Convention: (unit, normal, degel)
  unitloadCapact m_unitLoadCapacity{ { 30,{ 17.,14. } },{ 33,{ 31.,25. } },
  { 103,{ 32.,25. } },{ 110,{ 32.,25. } },{ 111,{ 26.,21. } },{ 112,{ 38.5,32. } },
  { 114,{ 38.5,31. } },{ 115,{ 32.,25. } },{ 116,{ 14.,11. } },{ 117,{ 38.5,32. } } };

  /** store all progress bar in a map with corresponding unit no*/
  using unitpbar = std::map<int/*unit no*/, QProgressBar*>;
  //unitpbar w_unitPbar;
  unitpbar m_unitPbar;
};