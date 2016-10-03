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

#include "mitkAdaptiveTumorSegTool.h"
#include "mitkImage.h"
#include "mitkToolManager.h"
#include "mitkProperties.h"
// us
#include <usModule.h>
#include <usModuleResource.h>
#include <usGetModuleContext.h>
#include <usModuleContext.h>


namespace mitk {
  MITK_TOOL_MACRO(MITKSEGMENTATION_EXPORT, AdaptiveTumorSegTool, "AdaptiveTumorSegTool");
}

mitk::AdaptiveTumorSegTool::AdaptiveTumorSegTool()
{
  m_PointSetNode = mitk::DataNode::New();
  m_PointSetNode->GetPropertyList()->SetProperty("name", mitk::StringProperty::New("3D_TumorSeg_Seedpoint"));
  m_PointSetNode->GetPropertyList()->SetProperty("helper object", mitk::BoolProperty::New(true));
  m_PointSet = mitk::PointSet::New();
  m_PointSetNode->SetData(m_PointSet);
}

mitk::AdaptiveTumorSegTool::~AdaptiveTumorSegTool()
{
}

bool mitk::AdaptiveTumorSegTool::CanHandle(BaseData* referenceData) const
{
  if (referenceData == NULL)
    return false;

  Image* image = dynamic_cast<Image*>(referenceData);

  if (image == NULL)
    return false;

  if (image->GetDimension() < 3)
    return false;

  return true;
}

const char** mitk::AdaptiveTumorSegTool::GetXPM() const
{
  return NULL;
}

const char* mitk::AdaptiveTumorSegTool::GetName() const
{
  return "Tumor Segmentation 3D";
}

us::ModuleResource mitk::AdaptiveTumorSegTool::GetIconResource() const
{
  us::Module* module = us::GetModuleContext()->GetModule();
  us::ModuleResource resource = module->GetResource("RegionGrowing_48x48.png");
  return resource;
}

void mitk::AdaptiveTumorSegTool::Activated()
{
  Superclass::Activated();

  if (!GetDataStorage()->Exists(m_PointSetNode))
    GetDataStorage()->Add(m_PointSetNode, GetWorkingData());
  m_SeedPointInteractor = mitk::SinglePointDataInteractor::New();
  m_SeedPointInteractor->LoadStateMachine("PointSet.xml");
  m_SeedPointInteractor->SetEventConfig("PointSetConfig.xml");
  m_SeedPointInteractor->SetDataNode(m_PointSetNode);
}

void mitk::AdaptiveTumorSegTool::Deactivated()
{
  m_PointSet->Clear();
  GetDataStorage()->Remove(m_PointSetNode);

  Superclass::Deactivated();
}

mitk::DataNode* mitk::AdaptiveTumorSegTool::GetReferenceData(){
  return this->m_ToolManager->GetReferenceData(0);
}

mitk::DataStorage* mitk::AdaptiveTumorSegTool::GetDataStorage(){
  return this->m_ToolManager->GetDataStorage();
}

mitk::DataNode* mitk::AdaptiveTumorSegTool::GetWorkingData(){
  return this->m_ToolManager->GetWorkingData(0);
}

mitk::DataNode::Pointer mitk::AdaptiveTumorSegTool::GetPointSetNode()
{
  return m_PointSetNode;
}
