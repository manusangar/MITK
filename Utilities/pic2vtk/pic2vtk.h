#ifndef Pic2vtk_h
#define Pic2vtk_h

#include <vtkImageReader.h>
#include <ipPic/ipPic.h>

/** @file
 *	DKFZ - Heidelberg
 *
 *	Module Name:	Pic2vtk
 *
 *	Module Klasse: 	ADT
 *
 *	Beschreibung:
 *
 *
 *	Exportierte Klassen und Funktionen:
 *
 *
 *	Importierte Klassen und Funktionen:
 *
 *
 *	@warning		Dieses Modul wurde noch nicht abgenommen!
 *
 *	@version		1.0
 *
 *  @date			03.07.2000
 *
 *  @author			Marcus Vetter
 *
 */

class Pic2vtk {

private:

	// verhindert das erzeugen eines Objects.
	Pic2vtk() {};

public:

	static vtkImageData* loadVtkImage( char* fileName );
	static vtkImageData* loadImage( char* fileName, bool littleEndian = true );
	static vtkImageData* convert( ipPicDescriptor* pic );
	static vtkImageData* convert( ipPicDescriptor* pic, unsigned long mask, int move );
	static ipPicDescriptor* convert( vtkImageData* vtkImage );
	static ipPicDescriptor* convertSigned2Unsigned( ipPicDescriptor*, unsigned long* offset = NULL );

	static vtkImageData* convertVectorImage( ipPicDescriptor* pic );
	static ipPicDescriptor* convertVectorImage( vtkImageData* vtkImage );

};

#endif // Pic2vtk_h
