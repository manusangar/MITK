set(CPP_FILES

  # DataStructures Connectomics
  IODataStructures/mitkConnectomicsNetwork.cpp
  IODataStructures/mitkConnectomicsNetworkReader.cpp
  IODataStructures/mitkConnectomicsNetworkIOFactory.cpp
  IODataStructures/mitkConnectomicsNetworkSerializer.cpp
  IODataStructures/mitkConnectomicsNetworkWriter.cpp
  IODataStructures/mitkConnectomicsNetworkWriterFactory.cpp
  IODataStructures/mitkConnectomicsNetworkDefinitions.cpp
  IODataStructures/mitkConnectomicsConstantsManager.cpp

  # DataStructures
  IODataStructures/mitkConnectomicsObjectFactory.cpp

  # Rendering Connectomics
  Rendering/mitkConnectomicsNetworkMapper3D.cpp
  Rendering/mitkConnectomicsRenderingSchemeProperty.cpp
  Rendering/mitkConnectomicsRenderingEdgeFilteringProperty.cpp
  Rendering/mitkConnectomicsRenderingNodeFilteringProperty.cpp
  Rendering/mitkConnectomicsRenderingEdgeColorParameterProperty.cpp
  Rendering/mitkConnectomicsRenderingEdgeRadiusParameterProperty.cpp
  Rendering/mitkConnectomicsRenderingNodeColorParameterProperty.cpp
  Rendering/mitkConnectomicsRenderingNodeRadiusParameterProperty.cpp
  Rendering/mitkConnectomicsRenderingNodeThresholdParameterProperty.cpp
  Rendering/mitkConnectomicsRenderingEdgeThresholdParameterProperty.cpp

  # Algorithms Connectomics
  Algorithms/mitkConnectomicsNetworkCreator.cpp
  Algorithms/mitkConnectomicsHistogramBase.cpp
  Algorithms/mitkConnectomicsDegreeHistogram.cpp
  Algorithms/mitkConnectomicsShortestPathHistogram.cpp
  Algorithms/mitkConnectomicsBetweennessHistogram.cpp
  Algorithms/mitkConnectomicsHistogramCache.cpp
  Algorithms/mitkConnectomicsSyntheticNetworkGenerator.cpp
  Algorithms/mitkConnectomicsSimulatedAnnealingPermutationBase.cpp
  Algorithms/mitkConnectomicsSimulatedAnnealingPermutationModularity.cpp
  Algorithms/mitkConnectomicsSimulatedAnnealingManager.cpp
  Algorithms/mitkConnectomicsSimulatedAnnealingCostFunctionBase.cpp
  Algorithms/mitkConnectomicsSimulatedAnnealingCostFunctionModularity.cpp
  Algorithms/itkConnectomicsNetworkToConnectivityMatrixImageFilter.cpp

)

set(H_FILES
  # Rendering Connectomics
  Rendering/mitkConnectomicsNetworkMapper3D.h
  Rendering/mitkConnectomicsRenderingProperties.h
  Rendering/mitkConnectomicsRenderingSchemeProperty.h
  Rendering/mitkConnectomicsRenderingEdgeFilteringProperty.h
  Rendering/mitkConnectomicsRenderingNodeFilteringProperty.h
  Rendering/mitkConnectomicsRenderingEdgeColorParameterProperty.h
  Rendering/mitkConnectomicsRenderingEdgeRadiusParameterProperty.h
  Rendering/mitkConnectomicsRenderingNodeColorParameterProperty.h
  Rendering/mitkConnectomicsRenderingNodeRadiusParameterProperty.h
  Rendering/mitkConnectomicsRenderingNodeThresholdParameterProperty.h
  Rendering/mitkConnectomicsRenderingEdgeThresholdParameterProperty.h


  # Datastructures Connectomics
  IODataStructures/mitkConnectomicsNetwork.h
  IODataStructures/mitkConnectomicsNetworkReader.h
  IODataStructures/mitkConnectomicsNetworkIOFactory.h
  IODataStructures/mitkConnectomicsNetworkSerializer.h
  IODataStructures/mitkConnectomicsNetworkWriter.h
  IODataStructures/mitkConnectomicsNetworkWriterFactory.h
  IODataStructures/mitkConnectomicsNetworkDefinitions.h
  IODataStructures/mitkConnectomicsConstantsManager.h

  # Algorithms Connectomics
  Algorithms/mitkConnectomicsNetworkCreator.h
  Algorithms/mitkConnectomicsHistogramBase.h
  Algorithms/mitkConnectomicsDegreeHistogram.h
  Algorithms/mitkConnectomicsShortestPathHistogram.h
  Algorithms/mitkConnectomicsBetweennessHistogram.h
  Algorithms/mitkConnectomicsHistogramCache.h
  Algorithms/mitkConnectomicsSyntheticNetworkGenerator.h
  Algorithms/mitkConnectomicsSimulatedAnnealingPermutationBase.h
  Algorithms/mitkConnectomicsSimulatedAnnealingPermutationModularity.h
  Algorithms/mitkConnectomicsSimulatedAnnealingManager.h
  Algorithms/mitkConnectomicsSimulatedAnnealingCostFunctionBase.h
  Algorithms/mitkConnectomicsSimulatedAnnealingCostFunctionModularity.h
  Algorithms/itkConnectomicsNetworkToConnectivityMatrixImageFilter.h


)

