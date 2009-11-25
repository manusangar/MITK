#ifndef QMITKsegmentationVIEW_H_
#define QMITKsegmentationVIEW_H_

#include <cherryIPartListener.h>

#include <QmitkFunctionality.h>
#include <QmitkStandardViews.h>
#include <QmitkStdMultiWidgetEditor.h>

#include "ui_QmitkSegmentationControls.h"
#include <cherryISelectionListener.h>
#include "mitkDataTreeNodeSelection.h"
#include <cherryICherryPreferences.h>

#include "mitkSegmentationSelectionProvider.h"

/**
 * \ingroup org_mitk_gui_qt_segmentation_internal
 */
class QmitkSegmentationView : public QObject, public QmitkFunctionality
{
  Q_OBJECT

public:
  static std::string ORGAN_COLOR_STRING;
  static std::string CreateOrganColorString();
  void SetFocus();

  QmitkSegmentationView();
  virtual ~QmitkSegmentationView();
    
  void OnSurfaceCalculationDone();
    
  void OnNewNodesGenerated();
  void OnNewNodeObjectsGenerated(mitk::ToolManager::DataVectorType*);

  virtual void Activated();
  virtual void Deactivated();

protected:

  void CreateQtPartControl(QWidget* parent);

  public:

    ///
    /// Called when a StdMultiWidget is available.
    ///
    virtual void StdMultiWidgetAvailable(QmitkStdMultiWidget& stdMultiWidget);
    ///
    /// Called when no StdMultiWidget is available.
    ///
    virtual void StdMultiWidgetNotAvailable();

    ///
    /// Called when no StdMultiWidget is getting closed.
    ///
    virtual void StdMultiWidgetClosed(QmitkStdMultiWidget& stdMultiWidget);

    void OnThresholdingToolManagerToolModified();
  
    void OnPreferencesChanged(const cherry::ICherryPreferences*);
  protected slots:

    void SelectionChanged(cherry::IWorkbenchPart::Pointer sourcepart, cherry::ISelection::ConstPointer selection);

    void CreateNewSegmentation();

    void OnToolSelected(int id);
    void OnSegmentationMethodSelected(int id);
    void OnReferenceNodeSelected(const mitk::DataTreeNode*);
    void OnWorkingDataSelectionChanged(const mitk::DataTreeNode*);

    void SendSelectedEvent( mitk::DataTreeNode* referenceNode, mitk::DataTreeNode* workingNode );

    void CreateSurface(bool);
    void CreateSmoothedSurface(bool);
    void ImageStatistics(bool);
    void AutocropSelected(bool);
    void ThresholdImage(bool);
    void ThresholdingDone(int);

  protected:

    typedef std::vector<mitk::DataTreeNode*> NodeList;
    NodeList GetSelectedNodes() const;

    void CheckImageAlignment();
    void CreateASurface(bool smoothed);

    void ApplyDisplayOptions(mitk::DataTreeNode* node);

    void UpdateFromCurrentDataManagerSelection();

    void ExtendOrganList(std::string organname,mitk::Color colorname);
  
    cherry::ICherryPreferences::Pointer m_SegmentationPreferencesNode;

    QmitkStdMultiWidget * m_MultiWidget;

    QWidget* m_Parent;

    Ui::QmitkSegmentationControls * m_Controls;
    unsigned long m_ObserverTag;
    mitk::DataStorage::Pointer m_DataStorage;

    mitk::DataTreeNodeSelection::ConstPointer m_CurrentSelection;
    cherry::ISelectionListener::Pointer m_SelectionListener;
    friend struct cherry::SelectionChangedAdapter<QmitkSegmentationView>;
  
    QAction* m_CreateSurfaceAction;
    QAction* m_CreateSmoothSurfaceAction;
    QAction* m_StatisticsAction;
    QAction* m_AutocropAction;
    QAction* m_ThresholdAction;

    QDialog* m_ThresholdingDialog;
    mitk::ToolManager::Pointer m_ThresholdingToolManager;

    bool m_ShowSegmentationsAsOutline;
    bool m_ShowSegmentationsAsVolumeRendering;

    mitk::SegmentationSelectionProvider::Pointer m_SelectionProvider;

    QStringList organColorList;

  /// from QmitkSegmentation

};

#endif /*QMITKsegmentationVIEW_H_*/
