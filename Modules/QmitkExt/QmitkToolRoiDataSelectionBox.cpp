#include "QmitkToolRoiDataSelectionBox.h"
#include <mitkProperties.h>
#include <QBoxLayout>
#include <QLabel>

QmitkToolRoiDataSelectionBox::QmitkToolRoiDataSelectionBox(QWidget* parent, mitk::DataStorage* storage)
:QWidget(parent),
m_SelfCall(false),
m_lastSelection(NULL),
m_lastSelectedName(tr("none"))
{

  QBoxLayout* mainLayout = new QVBoxLayout(this);
  m_segmentationComboBox = new QComboBox(this);
  QLabel* label = new QLabel("region of interest:", this);
  m_boundingObjectWidget = new QmitkBoundingObjectWidget();

  mainLayout->addWidget(label);
  mainLayout->addWidget(m_segmentationComboBox);
  mainLayout->addWidget(m_boundingObjectWidget);

  //connect signals
  connect(m_segmentationComboBox, SIGNAL(activated(const QString&)), this, SLOT(OnRoiDataSelectionChanged(const QString&)) );
  connect(m_boundingObjectWidget, SIGNAL(BoundingObjectsChanged(const QString&)), this, SLOT(OnRoiDataSelectionChanged(const QString&)) );

  //create ToolManager
  m_ToolManager = mitk::ToolManager::New(storage);

  //setup message delegates
  m_ToolManager->RoiDataChanged += mitk::MessageDelegate<QmitkToolRoiDataSelectionBox> (this, &QmitkToolRoiDataSelectionBox::OnToolManagerRoiDataModified);

}

QmitkToolRoiDataSelectionBox::~QmitkToolRoiDataSelectionBox()
{
}

void QmitkToolRoiDataSelectionBox::SetDataStorage(mitk::DataStorage &storage)
{
  m_ToolManager->SetDataStorage(storage);
  m_boundingObjectWidget->SetDataStorage(&storage);
  UpdateComboBoxData();
}

mitk::DataStorage* QmitkToolRoiDataSelectionBox::GetDataStorage()
{
  return m_ToolManager->GetDataStorage();
}

void QmitkToolRoiDataSelectionBox::SetToolManager(mitk::ToolManager& manager)
{
  //remove old messagedelegates
  m_ToolManager->RoiDataChanged -= mitk::MessageDelegate<QmitkToolRoiDataSelectionBox> (this, &QmitkToolRoiDataSelectionBox::OnToolManagerRoiDataModified);
  //set new toolmanager
  m_ToolManager = &manager;
  //add new message delegates
  m_ToolManager->RoiDataChanged += mitk::MessageDelegate<QmitkToolRoiDataSelectionBox> (this, &QmitkToolRoiDataSelectionBox::OnToolManagerRoiDataModified);
}

mitk::ToolManager* QmitkToolRoiDataSelectionBox::GetToolManager()
{
  return m_ToolManager;
}

void QmitkToolRoiDataSelectionBox::OnToolManagerRoiDataModified()
{
  if (m_SelfCall)
    return;
  //UpdateComboBoxData();
}

void QmitkToolRoiDataSelectionBox::OnRoiDataSelectionChanged(const QString& name)
{
  if (name.compare(tr("")) == 0)
    return;

  m_lastSelectedName = name;
  m_boundingObjectWidget->setEnabled(false);
  mitk::DataNode::Pointer selection = NULL;

  if ( name.compare(tr("none"))==0);
  else if (name.compare(tr("bounding objects"))==0)
  {
    m_boundingObjectWidget->setEnabled(true);
    selection = m_boundingObjectWidget->GetAllBoundingObjects();
  }
  else
  {
    selection = m_ToolManager->GetDataStorage()->GetNamedNode(name.toLocal8Bit().data());
  }

  if (selection == m_lastSelection)
    return;

  m_lastSelection = selection;
  m_SelfCall = true;
  m_ToolManager->SetRoiData(selection);
  m_SelfCall = false;
}

void QmitkToolRoiDataSelectionBox::UpdateComboBoxData()
{
  m_segmentationComboBox->clear();
  m_segmentationComboBox->addItem(tr("none"));
  m_segmentationComboBox->insertSeparator(1);


  //predicates for combobox
  mitk::NodePredicateProperty::Pointer isBinary= mitk::NodePredicateProperty::New("binary", mitk::BoolProperty::New(true));
  mitk::NodePredicateDataType::Pointer isImage= mitk::NodePredicateDataType::New("Image");
  mitk::NodePredicateProperty::Pointer isHelperObject= mitk::NodePredicateProperty::New("helper object", mitk::BoolProperty::New(true));
  mitk::NodePredicateNOT::Pointer isNotHelperObject = mitk::NodePredicateNOT::New(isHelperObject);
  mitk::NodePredicateAND::Pointer segmentationPredicate = mitk::NodePredicateAND::New(isImage, isBinary, isNotHelperObject);

  mitk::DataStorage::SetOfObjects::ConstPointer allSegmentations = m_ToolManager->GetDataStorage()->GetSubset(segmentationPredicate);
  QStringList names;

  for (mitk::DataStorage::SetOfObjects::const_iterator it = allSegmentations->begin(); it != allSegmentations->end(); ++it)
  {
    mitk::DataNode::Pointer node = *it;

    QString name = QString::fromLocal8Bit(node->GetName().c_str());
    names.append(name);
  }
  if (names.length() > 0)
  {
    m_segmentationComboBox->addItems(names);
    m_segmentationComboBox->insertSeparator(names.length()+2);
  }

  m_segmentationComboBox->addItem(tr("bounding objects"));

  int id = m_segmentationComboBox->findText(m_lastSelectedName);
  if (id < 0)
    this->OnRoiDataSelectionChanged(tr("none"));
  else
    m_segmentationComboBox->setCurrentIndex(id);
}

void QmitkToolRoiDataSelectionBox::setEnabled(bool flag)
{
  if (!flag)
    this->OnRoiDataSelectionChanged(tr("none"));
  m_segmentationComboBox->setEnabled(flag);
}