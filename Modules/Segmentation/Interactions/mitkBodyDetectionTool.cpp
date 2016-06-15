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

#include "mitkBodyDetectionTool.h"

#include "mitkToolManager.h"
#include "mitkBoundingObjectToSegmentationFilter.h"

#include <mitkCoreObjectFactory.h>
#include "mitkLevelWindowProperty.h"
#include "mitkColorProperty.h"
#include "mitkProperties.h"
#include "mitkOrganTypeProperty.h"
#include "mitkVtkResliceInterpolationProperty.h"
#include "mitkDataStorage.h"
#include "mitkRenderingManager.h"

#include "mitkImageCast.h"
#include "mitkImageAccessByItk.h"
#include "mitkImageTimeSelector.h"
#include "mitkImageStatisticsHolder.h"
#include <itkImageRegionIterator.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkBinaryFillholeImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkBinaryErodeImageFilter.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkSliceBySliceImageFilter.h>
#include "mitkPadImageFilter.h"
#include "mitkMaskAndCutRoiImageFilter.h"
#include "mitkLabelSetImage.h"

// us
#include "usModule.h"
#include "usModuleResource.h"
#include "usGetModuleContext.h"

namespace mitk {
  MITK_TOOL_MACRO(MITKSEGMENTATION_EXPORT, BodyDetectionTool, "Body detection tool");
}


mitk::BodyDetectionTool::BodyDetectionTool() :
  m_SensibleMinimumThresholdValue(-600),
  m_SensibleMaximumThresholdValue(+0),
  m_CurrentThresholdValue(-300),
  m_IsFloatImage(false)
{
  m_ThresholdFeedbackNode = DataNode::New();
  m_ThresholdFeedbackNode->SetProperty("name", StringProperty::New("Thresholding feedback"));
  m_ThresholdFeedbackNode->SetProperty("helper object", BoolProperty::New(true));
  m_ThresholdFeedbackNode->SetFloatProperty("labelset.contour.width", 0.0);
}

mitk::BodyDetectionTool::~BodyDetectionTool()
{
}

const char** mitk::BodyDetectionTool::GetXPM() const
{
  return nullptr;
}

us::ModuleResource mitk::BodyDetectionTool::GetIconResource() const
{
  us::Module* module = us::GetModuleContext()->GetModule();
  us::ModuleResource resource = module->GetResource("Body_48x48.png");
  return resource;
}

const char* mitk::BodyDetectionTool::GetName() const
{
  return "Body Detection";
}

void mitk::BodyDetectionTool::Activated()
{
  Superclass::Activated();

  m_ToolManager->RoiDataChanged += mitk::MessageDelegate<mitk::BodyDetectionTool>(this, &mitk::BodyDetectionTool::OnRoiDataChanged);

  m_OriginalImageNode = m_ToolManager->GetReferenceData(0);
  m_NodeForThresholding = m_OriginalImageNode;

  if (m_NodeForThresholding.IsNotNull())
  {
    SetupPreviewNode();
  }
  else
  {
    m_ToolManager->ActivateTool(-1);
  }
}

void mitk::BodyDetectionTool::Deactivated()
{
  m_ToolManager->RoiDataChanged -= mitk::MessageDelegate<mitk::BodyDetectionTool>(this, &mitk::BodyDetectionTool::OnRoiDataChanged);
  m_NodeForThresholding = NULL;
  m_OriginalImageNode = NULL;
  try
  {
    if (DataStorage* storage = m_ToolManager->GetDataStorage())
    {
      storage->Remove(m_ThresholdFeedbackNode);
      RenderingManager::GetInstance()->RequestUpdateAll();
    }
  }
  catch (...)
  {
    // don't care
  }
  m_ThresholdFeedbackNode->SetData(NULL);

  Superclass::Deactivated();
}

void mitk::BodyDetectionTool::SetThresholdValue(double value)
{
  if (m_ThresholdFeedbackNode.IsNotNull())
  {
    m_CurrentThresholdValue = value;
    // Bug 19250: The range of 0.01 is rather random. It was 0.001 before and probably due to rounding error propagation in VTK code
    // it leads to strange banding effects on floating point images with a huge range (like -40000 - 40000). 0.01 lowers this effect
    // enough to work with our images. Might not work on images with really huge ranges, though. Anyways, still seems to be low enough
    // to work for floating point images with a range between 0 and 1. A better solution might be to dynamically calculate the value
    // based on the value range of the current image (as big as possible, as small as necessary).
    m_ThresholdFeedbackNode->SetProperty( "levelwindow", LevelWindowProperty::New( LevelWindow(m_CurrentThresholdValue, 0.01) ) );
    UpdatePreview();
  }
}

void mitk::BodyDetectionTool::AcceptCurrentThresholdValue()
{
  CreateNewSegmentationFromThreshold(m_NodeForThresholding);

  RenderingManager::GetInstance()->RequestUpdateAll();
  m_ToolManager->ActivateTool(-1);
}

void mitk::BodyDetectionTool::CancelThresholding()
{
  m_ToolManager->ActivateTool(-1);
}

void mitk::BodyDetectionTool::SetupPreviewNode()
{
  if (m_NodeForThresholding.IsNotNull())
  {
    Image::Pointer image = dynamic_cast<Image*>(m_NodeForThresholding->GetData());
    Image::Pointer originalImage = dynamic_cast<Image*> (m_OriginalImageNode->GetData());

    if (image.IsNotNull())
    {
      mitk::Image* workingimage = dynamic_cast<mitk::Image*>(m_ToolManager->GetWorkingData(0)->GetData());

      if (workingimage)
      {
        m_ThresholdFeedbackNode->SetData(workingimage->Clone());

        //Let's paint the feedback node green...
        mitk::LabelSetImage::Pointer previewImage = dynamic_cast<mitk::LabelSetImage*> (m_ThresholdFeedbackNode->GetData());

        itk::RGBPixel<float> pixel;
        pixel[0] = 0.0f;
        pixel[1] = 1.0f;
        pixel[2] = 0.0f;
        previewImage->GetActiveLabel()->SetColor(pixel);
        previewImage->GetActiveLabelSet()->UpdateLookupTable(previewImage->GetActiveLabel()->GetValue());
      }
      else
        m_ThresholdFeedbackNode->SetData(mitk::Image::New());

      int layer(50);
      m_NodeForThresholding->GetIntProperty("layer", layer);
      m_ThresholdFeedbackNode->SetIntProperty("layer", layer + 1);

      if (DataStorage* ds = m_ToolManager->GetDataStorage())
      {
        if (!ds->Exists(m_ThresholdFeedbackNode))
          ds->Add(m_ThresholdFeedbackNode, m_OriginalImageNode);
      }

      if (image.GetPointer() == originalImage.GetPointer())
      {
        Image::StatisticsHolderPointer statistics = originalImage->GetStatistics();
        m_SensibleMinimumThresholdValue = static_cast<double>(statistics->GetScalarValueMin());
        m_SensibleMaximumThresholdValue = static_cast<double>(statistics->GetScalarValueMax());
      }

      if ((originalImage->GetPixelType().GetPixelType() == itk::ImageIOBase::SCALAR)
        && (originalImage->GetPixelType().GetComponentType() == itk::ImageIOBase::FLOAT || originalImage->GetPixelType().GetComponentType() == itk::ImageIOBase::DOUBLE))
        m_IsFloatImage = true;
      else
        m_IsFloatImage = false;

      if (m_SensibleMinimumThresholdValue < -300) {
        m_CurrentThresholdValue = -300;
      } else {
        m_CurrentThresholdValue = (m_SensibleMaximumThresholdValue + m_SensibleMinimumThresholdValue) / 2.0;
      }
 
      IntervalBordersChanged.Send(m_SensibleMinimumThresholdValue, m_SensibleMaximumThresholdValue, m_IsFloatImage);
      ThresholdingValueChanged.Send(m_CurrentThresholdValue);
    }
  }
}

template <typename TPixel, unsigned int VImageDimension>
static void ITKSetVolume(itk::Image<TPixel, VImageDimension>* originalImage, mitk::Image* segmentation, unsigned int timeStep)
{
  segmentation->SetVolume((void*)originalImage->GetPixelContainer()->GetBufferPointer(), timeStep);
}

void mitk::BodyDetectionTool::CreateNewSegmentationFromThreshold(DataNode* node)
{
  if (node)
  {
    Image::Pointer feedBackImage = dynamic_cast<Image*>(m_ThresholdFeedbackNode->GetData());
    if (feedBackImage.IsNotNull())
    {
      DataNode::Pointer emptySegmentation = GetTargetSegmentationNode();

      if (emptySegmentation)
      {
        // actually perform a thresholding and ask for an organ type
        for (unsigned int timeStep = 0; timeStep < feedBackImage->GetTimeSteps(); ++timeStep)
        {
          try
          {
            ImageTimeSelector::Pointer timeSelector = ImageTimeSelector::New();
            timeSelector->SetInput(feedBackImage);
            timeSelector->SetTimeNr(timeStep);
            timeSelector->UpdateLargestPossibleRegion();
            Image::Pointer image3D = timeSelector->GetOutput();

            if (image3D->GetDimension() == 2)
            {
              AccessFixedDimensionByItk_2(image3D, ITKSetVolume, 2, dynamic_cast<Image*>(emptySegmentation->GetData()), timeStep);
            }
            else
            {
              AccessFixedDimensionByItk_2(image3D, ITKSetVolume, 3, dynamic_cast<Image*>(emptySegmentation->GetData()), timeStep);
            }
          }
          catch (...)
          {
            Tool::ErrorMessage("Error accessing single time steps of the original image. Cannot create segmentation.");
          }
        }

        if (m_OriginalImageNode.GetPointer() != m_NodeForThresholding.GetPointer())
        {
          mitk::PadImageFilter::Pointer padFilter = mitk::PadImageFilter::New();

          padFilter->SetInput(0, dynamic_cast<mitk::Image*> (emptySegmentation->GetData()));
          padFilter->SetInput(1, dynamic_cast<mitk::Image*> (m_OriginalImageNode->GetData()));
          padFilter->SetBinaryFilter(true);
          padFilter->SetUpperThreshold(1);
          padFilter->SetLowerThreshold(1);
          padFilter->Update();

          emptySegmentation->SetData(padFilter->GetOutput());
        }

        m_ToolManager->SetWorkingData(emptySegmentation);
        m_ToolManager->GetWorkingData(0)->Modified();
      }
    }
  }
}

void mitk::BodyDetectionTool::OnRoiDataChanged()
{
  mitk::DataNode::Pointer node = m_ToolManager->GetRoiData(0);

  if (node.IsNotNull())
  {
    mitk::MaskAndCutRoiImageFilter::Pointer roiFilter = mitk::MaskAndCutRoiImageFilter::New();
    mitk::Image::Pointer image = dynamic_cast<mitk::Image*> (m_NodeForThresholding->GetData());

    if (image.IsNull())
      return;

    roiFilter->SetInput(image);
    roiFilter->SetRegionOfInterest(node->GetData());
    roiFilter->Update();

    mitk::DataNode::Pointer tmpNode = mitk::DataNode::New();
    tmpNode->SetData(roiFilter->GetOutput());

    m_SensibleMaximumThresholdValue = static_cast<double> (roiFilter->GetMaxValue());
    m_SensibleMinimumThresholdValue = static_cast<double> (roiFilter->GetMinValue());

    m_NodeForThresholding = tmpNode;
  }
  else
  {
    m_NodeForThresholding = m_OriginalImageNode;
  }

  this->SetupPreviewNode();
  this->UpdatePreview();
}

template <typename TPixel, unsigned int VImageDimension>
void mitk::BodyDetectionTool::ITKThresholding(itk::Image<TPixel, VImageDimension>* originalImage, Image* segmentation, double thresholdValue, unsigned int timeStep)
{
  typedef itk::Image<TPixel, VImageDimension> ImageType;
  typedef itk::Image<mitk::Tool::DefaultSegmentationDataType, VImageDimension> SegmentationType;
  typedef itk::BinaryThresholdImageFilter<ImageType, SegmentationType> ThresholdFilterType;

  typename ThresholdFilterType::Pointer filter = ThresholdFilterType::New();
  filter->SetInput(originalImage);
  filter->SetLowerThreshold(thresholdValue);
  filter->SetUpperThreshold(m_SensibleMaximumThresholdValue);
  filter->SetInsideValue(1);
  filter->SetOutsideValue(0);
  filter->Update();

  segmentation->SetVolume((void*)(filter->GetOutput()->GetPixelContainer()->GetBufferPointer()), timeStep);
}



void mitk::BodyDetectionTool::UpdatePreview()
{
  mitk::Image::Pointer thresholdImage = dynamic_cast<mitk::Image*> (m_NodeForThresholding->GetData());
  mitk::LabelSetImage::Pointer previewImage = dynamic_cast<mitk::LabelSetImage*> (m_ThresholdFeedbackNode->GetData());
  if (thresholdImage && previewImage)
  {
    for (unsigned int timeStep = 0; timeStep < thresholdImage->GetTimeSteps(); ++timeStep)
    {
      ImageTimeSelector::Pointer timeSelector = ImageTimeSelector::New();
      timeSelector->SetInput(thresholdImage);
      timeSelector->SetTimeNr(timeStep);
      timeSelector->UpdateLargestPossibleRegion();
      Image::Pointer feedBackImage3D = timeSelector->GetOutput();
      AccessByItk_n(feedBackImage3D, ITKThresholding, (previewImage, m_CurrentThresholdValue, timeStep));
    }

    RenderingManager::GetInstance()->RequestUpdateAll();
  }
}

template <typename TPixel, unsigned int VImageDimension>
void mitk::BodyDetectionTool::ITKDetectBody(itk::Image<TPixel, VImageDimension> *original, Image* segmentation, unsigned int timestep) {
  typedef itk::Image< TPixel, VImageDimension > ImageType;
  typedef itk::BinaryBallStructuringElement<TPixel, VImageDimension> BallType;
  typedef typename itk::BinaryFillholeImageFilter< itk::Image<TPixel, 2> > FillHoleFilterType;
  typedef typename itk::BinaryErodeImageFilter<ImageType, ImageType, BallType> BallErodeFilterType;
  typedef typename itk::BinaryDilateImageFilter<ImageType, ImageType, BallType> BallDilateFilterType;
  typedef typename itk::SliceBySliceImageFilter<ImageType, ImageType> SliceFilter;

  BallType strel;
  strel.SetRadius(3);
  strel.CreateStructuringElement();

  typename BallErodeFilterType::Pointer erodeFilter = BallErodeFilterType::New();
  erodeFilter->SetKernel(strel);
  erodeFilter->SetInput(original);
  erodeFilter->SetErodeValue(1);

  typename BallDilateFilterType::Pointer dilateFilter = BallDilateFilterType::New();
  dilateFilter->SetKernel(strel);
  dilateFilter->SetDilateValue(1);
  dilateFilter->SetInput(erodeFilter->GetOutput());

  typename FillHoleFilterType::Pointer fillHoleFilter = FillHoleFilterType::New();
  //fillHoleFilter->SetInput(dilateFilter->GetOutput());
  fillHoleFilter->SetForegroundValue(1);
  //fillHoleFilter->UpdateLargestPossibleRegion();

  typename SliceFilter::Pointer sliceFilter = SliceFilter::New();
  sliceFilter->SetFilter(fillHoleFilter);
  sliceFilter->SetInput(dilateFilter->GetOutput());
  sliceFilter->UpdateLargestPossibleRegion();

  segmentation->SetVolume((void*)(sliceFilter->GetOutput()->GetPixelContainer()->GetBufferPointer()), timestep);

  //mitk::CastToMitkImage(fillHoleFilter->GetOutput(), resultImage);
}

void mitk::BodyDetectionTool::DetectBody() {
  mitk::LabelSetImage::Pointer previewImage = dynamic_cast<mitk::LabelSetImage*> (m_ThresholdFeedbackNode->GetData());
  if (previewImage)
  {
    for (unsigned int timeStep = 0; timeStep < previewImage->GetTimeSteps(); ++timeStep)
    {
      ImageTimeSelector::Pointer timeSelector = ImageTimeSelector::New();
      timeSelector->SetInput(previewImage);
      timeSelector->SetTimeNr(timeStep);
      timeSelector->UpdateLargestPossibleRegion();
      Image::Pointer feedBackImage3D = timeSelector->GetOutput();
      AccessFixedDimensionByItk_2(feedBackImage3D, ITKDetectBody, 3, previewImage, timeStep);
    }

    RenderingManager::GetInstance()->RequestUpdateAll();
  }  
}