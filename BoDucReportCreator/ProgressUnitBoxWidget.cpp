// Qt includes
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QMessageBox>
// Boost include
#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string.hpp>
// application include
#include "ProgressUnitBoxWidget.h"

ProgressUnitBoxWidget::ProgressUnitBoxWidget(const QStringList& aListOfUnit, QWidget* parent /*= Q_NULLPTR*/)
: QWidget(parent),
  m_unitIndex(0),
  m_capacityLoad(eCptyMode::normal), // default
  m_listUniteAvailable(aListOfUnit)
{
  // some initialization
  unitLoadConfig();

  // do some initialization (createVerticalProgressBarBox() vertical progress bar)
  QGroupBox* w_progressBarUnitBox = createVerticalProgressBarBox();
  QHBoxLayout* w_layout = new QHBoxLayout;
  w_layout->addWidget( w_progressBarUnitBox);
  setLayout(w_layout); // takes the ownership
}

void ProgressUnitBoxWidget::setUnitProgressBar( const QString& aUnitOn, double aVal2Update)
{
 // bool ret = true;
  auto i = 0;
  for( const QString& w_unitNo : m_listUniteAvailable)
  {
    if( aUnitOn.compare(m_listUniteAvailable.at(i)) == 0) // identical
    {
      double w_val2Show = m_progressBar[i]->value() + aVal2Update;
      if (w_val2Show > m_progressBar[i]->maximum())
      {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Exceed Load");
        msgBox.setText("Maximum load is " + QString(std::to_string(m_progressBar[i]->maximum()).c_str()));
        msgBox.exec();
   //     ret = false;
      }
      m_progressBar[i]->setValue(w_val2Show);
      break;
    }
    ++i;
  }
 // return ret;


   // updating unit progress bar
//   if(aUnitOn.compare(m_listUniteAvailable.at(0)) == 0) // identical
//   {
//     double w_val2Show = m_progressBar[0]->value() + aQty;
//     m_progressBar[0]->setValue(w_val2Show);
//   }
//   else if (aUnitOn.compare(m_listUniteAvailable.at(1)) == 0)
//   {
//     double w_val2Show = m_progressBar[1]->value() + aQty;
//     m_progressBar[1]->setValue(w_val2Show);
//   }
//   else if (aUnitOn.compare(m_listUniteAvailable.at(2)) == 0)
//   {
//     double w_val2Show = m_progressBar[2]->value() + aQty;
//     m_progressBar[2]->setValue(w_val2Show);
//   }
//   else if (aUnitOn.compare(m_listUniteAvailable.at(3)) == 0)
//   {
//     double w_val2Show = m_progressBar[3]->value() + aQty;
//     m_progressBar[3]->setValue(w_val2Show);
//   }
//   else if (aUnitOn.compare(m_listUniteAvailable.at(4)) == 0)
//   {
//     double w_val2Show = m_progressBar[4]->value() + aQty;
//     m_progressBar[4]->setValue(w_val2Show);
//   }
}

void ProgressUnitBoxWidget::unitLoadConfig()
{
  // convention (tuple 3-element)
  // unit no, normal load, degel load -> std::make_tuple( 30,  17.,  14.)
  m_listLoadValuesPerUnit.push_back( std::make_tuple( 30,  17.,  14.)); //unit 30
  m_listLoadValuesPerUnit.push_back( std::make_tuple( 33,  31.,  25.)); //unit 33
  m_listLoadValuesPerUnit.push_back( std::make_tuple( 103, 32.,  25.)); //unit 103
  m_listLoadValuesPerUnit.push_back( std::make_tuple( 110, 32.,  25.)); //unit 110
  m_listLoadValuesPerUnit.push_back( std::make_tuple( 111, 26.,  21.)); //unit 111
  m_listLoadValuesPerUnit.push_back( std::make_tuple( 112, 38.5, 32.)); //unit 112
  m_listLoadValuesPerUnit.push_back( std::make_tuple( 114, 38.5, 31.)); //unit 114
  m_listLoadValuesPerUnit.push_back( std::make_tuple( 115, 32.,  25.)); //unit 115
  m_listLoadValuesPerUnit.push_back( std::make_tuple( 116, 14.,  11.)); //unit 116
  m_listLoadValuesPerUnit.push_back( std::make_tuple( 117, 38.5, 32.)); //unit 117
}

void ProgressUnitBoxWidget::setUnitCapacityLoad()
{
    auto w_pbarUnitMapBeg = m_unitPbar.cbegin();
    auto w_pbarUnitMapEnd = m_unitPbar.cend();
    while (w_pbarUnitMapBeg != w_pbarUnitMapEnd)
    {
      if (!w_pbarUnitMapBeg->second->isEnabled())
      {
        w_pbarUnitMapBeg->second->setEnabled(true);
      }
      if (w_pbarUnitMapBeg->second->value() != 0)
      {
        w_pbarUnitMapBeg->second->setValue(0);
      }
      if (w_pbarUnitMapBeg->second->minimum() != 0)
      {
        w_pbarUnitMapBeg->second->setMinimum(0);
      }
      if (m_capacityLoad == eCptyMode::normal)
      {
        w_pbarUnitMapBeg->second->setMaximum(m_unitLoadCapacity[w_pbarUnitMapBeg->first].first); // normal load: pair(normal,degel) which is first 
      }
      else // degel
      {
        w_pbarUnitMapBeg->second->setMaximum(m_unitLoadCapacity[w_pbarUnitMapBeg->first].second); // degel load: pair(normal,degel) which is second
      }
      ++w_pbarUnitMapBeg;
    }//while-loop
}

#if 0 //deprecated
// Design Note
//  In this version progress layout horizontally, but in the next
//  version it will be vertically, only one row instead of two.
QGroupBox* ProgressUnitBoxWidget::createProgressUnitBox()
{
  QVBoxLayout* w_vlayoutPbar = new QVBoxLayout;
  
  // create a layout for progress bar in a vertical layout
  QHBoxLayout* w_hProgressBar1 = new QHBoxLayout;
  for (auto i = 0; i < 5; ++i)
  {
    addProgressBar(w_hProgressBar1, m_listUniteAvailable.at(i).toStdString().c_str());
  }
  w_vlayoutPbar->addLayout(w_hProgressBar1);

  QHBoxLayout* w_hProgressBar2 = new QHBoxLayout;
  for( auto i = 5; i < 10; ++i)
  {
    addProgressBar(w_hProgressBar1, m_listUniteAvailable.at(i).toStdString().c_str());
  }
  w_vlayoutPbar->addLayout(w_hProgressBar2);

  // maybe create a group box and return it
  QGroupBox* w_unitBarBox = new QGroupBox;
  w_unitBarBox->setLayout(w_vlayoutPbar);

  return w_unitBarBox;
}
#endif

QGroupBox* ProgressUnitBoxWidget::createVerticalProgressBarBox()
{
    // create a layout for progress bar in a vertical layout
    QHBoxLayout* w_hProgressBar = new QHBoxLayout;

    auto i = 0;
    for( const auto& key : m_unitLoadCapacity | boost::adaptors::map_keys)
    {
      auto w_pbarNo = m_listUniteAvailable.at(i).split(" ")[1].toInt();
      if (key == w_pbarNo) // check if we have same unit
      {
        QLabel* w_pbarLabel = new QLabel(tr(m_listUniteAvailable.at(i++).toStdString().c_str()));
        QFont w_pbarFont = w_pbarLabel->font();
        w_pbarFont.setPixelSize(17);
        w_pbarFont.setBold(true);
        w_pbarLabel->setFont(w_pbarFont);
        m_unitPbar[key] = new QProgressBar; // insert in the map
                                            //        Qt::Orientation w_testOrientation = m_unitPbar[key]->orientation();
        m_unitPbar[key]->setOrientation(Qt::Vertical);
        m_unitPbar[key]->setTextVisible(true);
        QVBoxLayout* w_vlayoutPbar = new QVBoxLayout;
        w_vlayoutPbar->addWidget(w_pbarLabel);
        w_vlayoutPbar->addWidget(m_unitPbar[key]);
        w_hProgressBar->addLayout(w_vlayoutPbar); // take the ownership of vlayout
      }
    }

    // maybe create a group box and return it
    QGroupBox* w_unitBarBox = new QGroupBox(tr("Unit Capacity"));
    QFont w_font = w_unitBarBox->font();
    w_font.setPixelSize(20);
    w_unitBarBox->setFont(w_font);
    w_unitBarBox->setFixedHeight(150); // capacity box height fixed, should use a variable constexpr
    w_unitBarBox->setLayout(w_hProgressBar); //take the ownership of hlayout

    return w_unitBarBox;
}

// deprecated
void ProgressUnitBoxWidget::addProgressBar( QBoxLayout* aProgressBar, const std::string& aUniteNb)
{
  // i am not sure i understand the code below, i do not think it's used?
  // need to check
  if (QVBoxLayout* w_lablandLayout1 = dynamic_cast<QVBoxLayout*>(aProgressBar))
  {
    // ...
    QLabel* w_barU1 = new QLabel(tr(aUniteNb.c_str()));
    m_progressBar[0] = new QProgressBar;

    // Both label and button together
    //			QVBoxLayout* w_lablandLayout1 = new QVBoxLayout;
    w_lablandLayout1->addWidget(w_barU1);
    w_lablandLayout1->addWidget(m_progressBar[0]);
    aProgressBar->addStretch(2);  // visual effect?
    aProgressBar->addLayout(w_lablandLayout1);
  }
  else if( QHBoxLayout* w_lablandLayout1 = dynamic_cast<QHBoxLayout*>(aProgressBar))
  {
    QLabel* w_barlbl = new QLabel( tr(aUniteNb.c_str()));
    //			short i = 0;
    //	static_assert(i < 10 && "");

    // 			for (const QString& w_unit : m_listUniteAvailable)
    // 			{
    if (m_unitIndex >= 0 && m_unitIndex < m_nbUnit)
    {
      m_progressBar[m_unitIndex] = new QProgressBar;
      // just a test (may want to put vertical)
      Qt::Orientation w_testOrientation = m_progressBar[m_unitIndex]->orientation();
      if (m_progressBar[m_unitIndex]->orientation() == Qt::Horizontal)
      {
        w_lablandLayout1->addWidget(w_barlbl);
        w_lablandLayout1->addWidget(m_progressBar[m_unitIndex++]);
        aProgressBar->addStretch(1);
        aProgressBar->addLayout(w_lablandLayout1);
      }
    }
  }
}