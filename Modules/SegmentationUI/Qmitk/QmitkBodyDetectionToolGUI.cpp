/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include "QmitkBodyDetectionToolGUI.h"

#include "QmitkNewSegmentationDialog.h"
#include "QmitkConfirmSegmentationDialog.h"

#include <qlabel.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <qlayout.h>

MITK_TOOL_GUI_MACRO(MITKSEGMENTATIONUI_EXPORT, QmitkBodyDetectionToolGUI, "")

QmitkBodyDetectionToolGUI::QmitkBodyDetectionToolGUI()
:QmitkToolGUI(),
 m_Slider(NULL),
 m_Spinner(NULL),
 m_isFloat(false),
 m_RangeMin(0),
 m_RangeMax(0),
 m_ChangingSlider(false),
 m_ChangingSpinner(false)
{
  // create the visible widgets
  QBoxLayout* mainLayout = new QVBoxLayout(this);

  QLabel* label = new QLabel( "Threshold :", this );
  QFont f = label->font();
  f.setBold(false);
  label->setFont( f );
  mainLayout->addWidget(label);


  QBoxLayout* layout = new QHBoxLayout();

  m_Spinner = new QDoubleSpinBox();
  m_Spinner->setMaximum(20);
  m_Spinner->setMinimum(5);
  m_Spinner->setValue(1);

  connect(m_Spinner, SIGNAL(valueChanged(double)), this, SLOT(OnSpinnerValueChanged()) );
  layout->addWidget(m_Spinner);

  //m_Slider = new QSlider( 5, 20, 1, 1, Qt::Horizontal, this );
  m_Slider = new QSlider( Qt::Horizontal, this );
  m_Slider->setMinimum(5);
  m_Slider->setMaximum(20);
  m_Slider->setPageStep(1);
  m_Slider->setValue(1);
  connect( m_Slider, SIGNAL(valueChanged(int)), this, SLOT(OnSliderValueChanged(int)));
  layout->addWidget( m_Slider );

  mainLayout->addLayout(layout);

  QBoxLayout *button_layout = new QHBoxLayout();

  QPushButton* doitButton = new QPushButton("Detect body", this);
  connect( doitButton, SIGNAL(clicked()), this, SLOT(OnDetectBody()));
  doitButton->setFont(f);
  button_layout->addWidget(doitButton);

  QPushButton* okButton = new QPushButton("Confirm Segmentation", this);
  connect( okButton, SIGNAL(clicked()), this, SLOT(OnAcceptThresholdPreview()));
  okButton->setFont( f );
  button_layout->addWidget(okButton);

  mainLayout->addLayout( button_layout );

 connect( this, SIGNAL(NewToolAssociated(mitk::Tool*)), this, SLOT(OnNewToolAssociated(mitk::Tool*)) );
}

QmitkBodyDetectionToolGUI::~QmitkBodyDetectionToolGUI()
{
  // !!!
  if (m_BodyDetectionTool.IsNotNull())
  {
    m_BodyDetectionTool->IntervalBordersChanged -= mitk::MessageDelegate3<QmitkBodyDetectionToolGUI, double, double, bool>( this, &QmitkBodyDetectionToolGUI::OnThresholdingIntervalBordersChanged );
    m_BodyDetectionTool->ThresholdingValueChanged -= mitk::MessageDelegate1<QmitkBodyDetectionToolGUI, double>( this, &QmitkBodyDetectionToolGUI::OnThresholdingValueChanged );
  }

}

void QmitkBodyDetectionToolGUI::OnNewToolAssociated(mitk::Tool* tool)
{
  if (m_BodyDetectionTool.IsNotNull())
  {
    m_BodyDetectionTool->IntervalBordersChanged -= mitk::MessageDelegate3<QmitkBodyDetectionToolGUI, double, double, bool>( this, &QmitkBodyDetectionToolGUI::OnThresholdingIntervalBordersChanged );
    m_BodyDetectionTool->ThresholdingValueChanged -= mitk::MessageDelegate1<QmitkBodyDetectionToolGUI, double>( this, &QmitkBodyDetectionToolGUI::OnThresholdingValueChanged );
  }

  m_BodyDetectionTool = dynamic_cast<mitk::BodyDetectionTool*>( tool );

  if (m_BodyDetectionTool.IsNotNull())
  {
    m_BodyDetectionTool->IntervalBordersChanged += mitk::MessageDelegate3<QmitkBodyDetectionToolGUI, double, double, bool>( this, &QmitkBodyDetectionToolGUI::OnThresholdingIntervalBordersChanged );
    m_BodyDetectionTool->ThresholdingValueChanged += mitk::MessageDelegate1<QmitkBodyDetectionToolGUI, double>( this, &QmitkBodyDetectionToolGUI::OnThresholdingValueChanged );
  }
}

void QmitkBodyDetectionToolGUI::OnSpinnerValueChanged()
{
   if (m_BodyDetectionTool.IsNotNull())
   {
      m_ChangingSpinner = true;
      double doubleVal = m_Spinner->value();
      int intVal = this->DoubleToSliderInt(doubleVal);
      m_BodyDetectionTool->SetThresholdValue( doubleVal );
      if (m_ChangingSlider == false)
         m_Slider->setValue( intVal );
      m_ChangingSpinner = false;
   }
}

void QmitkBodyDetectionToolGUI::OnSliderValueChanged(int value)
{
  if (m_BodyDetectionTool.IsNotNull())
  {
     m_ChangingSlider = true;
    double doubleVal = SliderIntToDouble(value);
    if (m_ChangingSpinner == false)
      m_Spinner->setValue(doubleVal);
    m_ChangingSlider = false;
  }

}

void QmitkBodyDetectionToolGUI::OnAcceptThresholdPreview()
{
  QmitkConfirmSegmentationDialog dialog;
  QString segName = QString::fromStdString(m_BodyDetectionTool->GetCurrentSegmentationName());

  dialog.SetSegmentationName(segName);
  int result = dialog.exec();

  switch(result)
  {
  case QmitkConfirmSegmentationDialog::CREATE_NEW_SEGMENTATION:
    m_BodyDetectionTool->SetOverwriteExistingSegmentation(false);
    break;
  case QmitkConfirmSegmentationDialog::OVERWRITE_SEGMENTATION:
    m_BodyDetectionTool->SetOverwriteExistingSegmentation(true);
    break;
  case QmitkConfirmSegmentationDialog::CANCEL_SEGMENTATION:
    return;
  }

  if (m_BodyDetectionTool.IsNotNull())
  {
      this->thresholdAccepted();
      m_BodyDetectionTool->AcceptCurrentThresholdValue();
  }
}

void QmitkBodyDetectionToolGUI::OnDetectBody() {
  m_BodyDetectionTool->DetectBody();
}

void QmitkBodyDetectionToolGUI::OnThresholdingIntervalBordersChanged(double lower, double upper, bool isFloat)
{
  m_isFloat = isFloat;
  m_RangeMin = lower;
  m_RangeMax = upper;

  m_Spinner->setRange(lower, upper);
  if (!m_isFloat)
  {
     m_Slider->setRange(int(lower), int(upper));
     m_Spinner->setDecimals(0);
     m_Spinner->setSingleStep(1);
  }
  else
  {
     m_Slider->setRange(0, 99);
     m_Spinner->setDecimals(2);
     m_Range = upper-lower;
     m_Spinner->setSingleStep(m_Range/100);
  }

}

void QmitkBodyDetectionToolGUI::OnThresholdingValueChanged(double current)
{
  m_Slider->setValue(DoubleToSliderInt(current));
  m_Spinner->setValue(current);
}




double QmitkBodyDetectionToolGUI::SliderIntToDouble(int val)
{
   if (!m_isFloat)
   {
      return double(val);
   }
   else
   {
      return double(val*(m_Range)/100 + m_RangeMin);
   }
}

int QmitkBodyDetectionToolGUI::DoubleToSliderInt(double val)
{
   if (!m_isFloat)
   {
      return int(val);
   }
   else
   {
      int intVal = int( ((val-m_RangeMin) / m_Range)*100);
      return intVal;
   }
}
