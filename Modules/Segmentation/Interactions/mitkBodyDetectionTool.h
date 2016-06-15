/*===================================================================


===================================================================*/

#ifndef mitkBodyDetectionTool_h_Included
#define mitkBodyDetectionTool_h_Included

#include "mitkCommon.h"
#include <MitkSegmentationExports.h>
#include "mitkAutoSegmentationTool.h"
#include <itkImage.h>
#include <mitkImage.h>

namespace us {
class ModuleResource;
}

namespace mitk
{


/**
  \brief Simple tool to automatically detect external body contours.

  \ingroup Interaction
  \ingroup ToolManagerEtAl

  Automates the steps needed to detect body

  \warning Only to be instantiated by mitk::ToolManager.

  $Darth Vader$
*/
class MITKSEGMENTATION_EXPORT BodyDetectionTool : public AutoSegmentationTool
{
  public:
    Message3<double,double, bool> IntervalBordersChanged;
    Message1<double>     ThresholdingValueChanged;

    mitkClassMacro(BodyDetectionTool, AutoSegmentationTool);
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

    const char** GetXPM() const override;
    const char* GetName() const override;
    us::ModuleResource GetIconResource() const override;

    virtual void Activated() override;
    virtual void Deactivated() override;

    virtual void SetThresholdValue(double value);
    virtual void AcceptCurrentThresholdValue();
    virtual void CancelThresholding();
    virtual void DetectBody();

  protected:

    BodyDetectionTool(); // purposely hidden
    virtual ~BodyDetectionTool();

    void SetupPreviewNode();

    void CreateNewSegmentationFromThreshold(DataNode* node);

    void OnRoiDataChanged();
    void UpdatePreview();

    template <typename TPixel, unsigned int VImageDimension>
    void ITKThresholding(itk::Image<TPixel, VImageDimension>* originalImage, 
                         mitk::Image* segmentation, double thresholdValue, 
                         unsigned int timeStep );
    
    template <typename TPixel, unsigned int VImageDimension>
    void ITKDetectBody(itk::Image<TPixel, VImageDimension>* original, 
                       mitk::Image* resultImage,
                       unsigned int timestep);

    DataNode::Pointer m_ThresholdFeedbackNode;
    DataNode::Pointer m_OriginalImageNode;
    DataNode::Pointer m_NodeForThresholding;

    double m_SensibleMinimumThresholdValue;
    double m_SensibleMaximumThresholdValue;
    double m_CurrentThresholdValue;
    bool m_IsFloatImage;
};

} // namespace

#endif
