#include "stem.h"
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Group>
#include <osg/PrimitiveSet>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osgUtil/SmoothingVisitor>
#include <cmath>

Stem::Stem(int recursion_level, int num_segments, int curve_back, int curve_variation, Stem *parent) : recursion_level(recursion_level), num_segments(num_segments), curve_back(curve_back), curve_variation(curve_variation), parent(parent) {
	this->stem = make_stem();
}

// Function taken from http://forum.openscenegraph.org/viewtopic.php?t=3289&view=previous
osg::Geometry * Stem::create_truncated_cone_geometry(float start_radius, float end_radius, float length, osg::Vec4 *colour) {
	// Here's our cone/cylinder implementation. We will create the top/bottom
	// by triangle fans and the body will connect the two as a quad strip

	int numberSegments = 40;
	float angle = 0.0f;
	float angleDelta = 2.0f * osg::PI/(float)numberSegments;

	// Create arrays to hold the X & Y coeffiecients that we will re-use
	// throughout the creation of vertices
	float * xTop = NULL;
	float * yTop = NULL;
	float * xBottom = NULL;
	float * yBottom = NULL;

	xTop = (float *) malloc(sizeof(float) * numberSegments+1);
	yTop = (float *) malloc(sizeof(float) * numberSegments+1);
	xBottom = (float *) malloc(sizeof(float) * numberSegments+1);
	yBottom = (float *) malloc(sizeof(float) * numberSegments+1);

	for (int i = 0; i < numberSegments; i++,angle -= angleDelta) {
		// Compute the cos/sin of the current angle as we rotate around the cylinder
		float cosAngle = cosf(angle);
		float sinAngle = sinf(angle);

		// Compute the top/bottom locations
		xTop[i] = cosAngle * end_radius;
		yTop[i] = sinAngle * end_radius;

		xBottom[i] = cosAngle * start_radius;
		yBottom[i] = sinAngle * start_radius;
	}		

	// Put the last point equal to the first point so the cylinder
	// is complete
	xTop[numberSegments] = xTop[0];
	yTop[numberSegments] = yTop[0];
	xBottom[numberSegments] = xBottom[0];
	yBottom[numberSegments] = yBottom[0];

	// Compute the number of vertices required for the top and bottom
	int numTopBottomVertices = numberSegments + 2;

	// Compute the number of vertices required for the body
	int numBodyVertices = (numberSegments + 1) * 2;

	// Create an array to hold the cylinder vertices
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
	vertices->reserve(2 * numTopBottomVertices);				
	
	int j = 0;

	// Create three primitive sets for the top, bottom and body of the cone
	osg::ref_ptr<osg::DrawElementsUByte> topPrimitive = new osg::DrawElementsUByte(osg::PrimitiveSet::TRIANGLE_FAN);
	osg::ref_ptr<osg::DrawElementsUByte> bottomPrimitive = new osg::DrawElementsUByte(osg::PrimitiveSet::TRIANGLE_FAN);
	osg::ref_ptr<osg::DrawElementsUByte> bodyPrimitive = new osg::DrawElementsUByte(osg::PrimitiveSet::QUAD_STRIP);

	// Allocate sufficient memory for the top/bottom and body
	topPrimitive->reserve(numTopBottomVertices);
	bottomPrimitive->reserve(numTopBottomVertices);
	bodyPrimitive->reserve(numBodyVertices);

	//
	// Create the vertices and indices for the bottom
	//
	
	// Create the centre vertex in the trianglestrip that will form the bottom
	vertices->push_back(osg::Vec3f(0.0f, 0.0f, 0.0f));			
	bottomPrimitive->push_back(j++);

	// Create the surrounding vertices for the bottom
	for (int i = 0; i <= numberSegments; i++) {
		// Set the vertex location
		vertices->push_back(osg::Vec3f(xBottom[i], yBottom[i], 0.0f));

		// Set the index to the vertex in the bottom primitive
		bottomPrimitive->push_back(j++);
	}

	//
	// Create the vertices and indices for the top
	//

	// Create the centre vertex in the trianglestrip that will form the top
	vertices->push_back(osg::Vec3f(0.0f, 0.0f, length));
	topPrimitive->push_back(j++);

	// Create surrounding vertices for the top in reverse order, so the normals
	// point the other way
	for (int i = numberSegments; i >=0; i--) {
		// Set the vertex location
		vertices->push_back(osg::Vec3f(xTop[i], yTop[i], length));

		// Set the index to the vertex in the bottom primitive
		topPrimitive->push_back(j++);
	}

	//
	// Finally create the indices for the body vertices
	//
	
	// To do this we will take one vertex from top and bottom alternately as the body
	// is constructed using a quad strip. So we take two variables, k and l, K starts at the bottom
	// and L at the top (remember top vertices are in reverse order due to normals) and work inwards
	for (int k = 1, l = vertices->size() - 1; k < numTopBottomVertices; k++, l--) {
		bodyPrimitive->push_back(k);
		bodyPrimitive->push_back(l);
	}

	// Note no memory management for coneBodyGeometry as this is returned from the function
	osg::Geometry * coneBodyGeometry = new osg::Geometry();
	
	// Set the vertices on the cone
	coneBodyGeometry->setVertexArray(vertices);			

	// Set the PrimitiveSets on the geometry object
	coneBodyGeometry->addPrimitiveSet(topPrimitive);
	coneBodyGeometry->addPrimitiveSet(bottomPrimitive);
	coneBodyGeometry->addPrimitiveSet(bodyPrimitive);

	// Set the Colour to the entire object
	osg::ref_ptr<osg::Vec4Array> colourArray = new osg::Vec4Array();
	colourArray->push_back(osg::Vec4(*colour));
	coneBodyGeometry->setColorArray(colourArray.get());			
	coneBodyGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	
	return coneBodyGeometry;
/*
	finally {
		delete [] xTop;
		delete [] yTop;
		delete [] xBottom;
		delete [] yBottom;
	}
*/
} 

osg::Group * Stem::make_stem() {
	osg::Group *stem = new osg::Group();
	// Make one demo truncated cone for now.
	osg::Geode* stem_geode = new osg::Geode();
	osg::Vec4* colour = new osg::Vec4(1.0, 0.5, 0.1, 1.0);
	stem_geode->addDrawable(this->create_truncated_cone_geometry(6.0, 3.0, 15.0, colour));
	stem->addChild(stem_geode);
	osgUtil::SmoothingVisitor sv;
	stem->accept(sv);
	return stem;
}
