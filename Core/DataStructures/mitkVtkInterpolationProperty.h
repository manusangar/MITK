#ifndef _MITK_VTK_INTERPOLATION_PROPERTY__H_
#define _MITK_VTK_INTERPOLATION_PROPERTY__H_

#include "mitkEnumerationProperty.h"

namespace mitk
{

/**
 * Encapsulates the enumeration vtkInterpolation. Valid values are 
 * (VTK constant/Id/string representation):
 * VTK_FLAT/0/Flat, VTK_GOURAUD/1/Gouraud, VTK_PHONG/2/Phong
 * Default is the Gouraud interpolation
 */
class VtkInterpolationProperty : public EnumerationProperty
{
public:

  mitkClassMacro( VtkInterpolationProperty, EnumerationProperty );
  
  /**
   * Constructor. Sets the representation to a default value of surface(2)
   */
  VtkInterpolationProperty( );
  
  /**
   * Constructor. Sets the interpolation to the given value. If it is not
   * valid, the interpolation is set to gouraud(1)
   * @param value the integer representation of the interpolation
   */
  VtkInterpolationProperty( const IdType& value );
  
  /**
   * Constructor. Sets the interpolation to the given value. If it is not
   * valid, the representation is set to gouraud(1)
   * @param value the string representation of the interpolation
   */
  VtkInterpolationProperty( const std::string& value );
  
  /**
   * Returns the current interpolation value as defined by VTK constants.
   * @returns the current interpolation as VTK constant.
   */
  virtual int GetVtkInterpolation();

  /**
   * Sets the interpolation type to VTK_FLAT.
   */
  virtual void SetInterpolationToFlat();
  
  /**
   * Sets the interpolation type to VTK_WIREFRAME.
   */
  virtual void SetInterpolationToGouraud();
  
  /**
   * Sets the interpolation type to VTK_SURFACE.
   */
  virtual void SetInterpolationToPhong();
  
protected:

  /**
   * this function is overridden as protected, so that the user may not add
   * additional invalid interpolation types.
   */
  virtual bool AddEnum( const std::string& name, const IdType& id );

  /**
   * Adds the enumeration types as defined by vtk to the list of known 
   * enumeration values.
   */
  virtual void AddInterpolationTypes();
};

} // end of namespace mitk

#endif

