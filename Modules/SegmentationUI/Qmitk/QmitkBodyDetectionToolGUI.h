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

#ifndef QmitkBodyDetectionToolGUI_h_Included
#define QmitkBodyDetectionToolGUI_h_Included

#include "QmitkToolGUI.h"
#include <MitkSegmentationUIExports.h>
#include "mitkBodyDetectionTool.h"

#include <QDoubleSpinBox>

class QSlider;

/**
  \ingroup org_mitk_gui_qt_interactivesegmentation_internal
  \brief GUI for mitk::BodyDetectionTool.
  \sa mitk::BodyDetectionTool

  This GUI shows two sliders to change the watershed parameters. It executes the watershed algorithm by clicking on the button.

*/
class MITKSEGMENTATIONUI_EXPORT QmitkBodyDetectionToolGUI : public QmitkToolGUI
{
  Q_OBJECT

  public:
    mitkClassMacro(QmitkBodyDetectionToolGUI, QmitkToolGUI);
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

    void OnThresholdingIntervalBordersChanged(double lower, double upper, bool isFloat);
    void OnThresholdingValueChanged(double current);

  signals:

    /// \brief Emitted when threshold Accepted
    void thresholdAccepted();

    /// \brief Emitted when threshold Canceled
    void thresholdCanceled();

  public slots:

  protected slots:

    void OnNewToolAssociated(mitk::Tool*);
    void OnAcceptThresholdPreview();
    void OnDetectBody();

    /// \brief Called when Spinner value has changed. Consider: Spinner contains DOUBLE values
    void OnSpinnerValueChanged();

    /// \brief Called when Slider value has changed. Consider: Slider contains INT values
    void OnSliderValueChanged(int value);

  protected:
    QmitkBodyDetectionToolGUI();
    virtual ~QmitkBodyDetectionToolGUI();

    /// \brief When Slider (int value) has changed, we need to convert it to a respective double value for the spinner
    double SliderIntToDouble(int val);

    /// \brief When Spinner (double value) has changed, we need to convert it to a respective int value for the slider
    int DoubleToSliderInt(double val);

    QSlider* m_Slider;
    QDoubleSpinBox* m_Spinner;

    /// \brief is image float or int?
    bool m_isFloat;

    double m_RangeMin;
    double m_RangeMax;
    double m_Range;

    /// \brief helper bool values to find out, which of the GUI elements has been touched by the user.
    bool m_ChangingSlider, m_ChangingSpinner;

    mitk::BodyDetectionTool::Pointer m_BodyDetectionTool;
};

#endif
