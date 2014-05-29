#include "stem.h"
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/PrimitiveSet>
#include <osg/Matrixd>
#include <osg/MatrixTransform>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Vec3>
#include <osg/Vec3d>
#include <osg/Vec4>
#include <osgUtil/SmoothingVisitor>
#include <cmath>
#include <iostream>

Stem::Stem(int recursion_level, int orientation, float diameter_rotation, float length, float base_radius, float length_onto_parent, osg::Vec3 base_pos, int num_segments, int curve, int curve_back, int curve_variation, int seg_splits, int base_splits, float split_angle, float split_angle_variation, Stem *parent) : recursion_level(recursion_level), orientation(orientation), diameter_rotation(diameter_rotation), length(length), base_radius(base_radius), length_onto_parent(length_onto_parent), base_pos(base_pos), num_segments(num_segments), curve(curve), curve_back(curve_back), curve_variation(curve_variation), seg_splits(seg_splits), base_splits(base_splits), split_angle(split_angle), split_angle_variation(split_angle_variation), parent(parent) {
	this->stem = new osg::Group;
	this->stem_trans = new osg::PositionAttitudeTransform;
	make_stem();
}

float Stem::get_length() {
	return this->length;
}

float Stem::get_base_radius() {
	return this->base_radius;
}

osg::Vec3 Stem::get_base_pos() {
	return this->base_pos;
}

osg::Vec3 Stem::get_top_pos() {
	return this->top_pos;
}

// Function taken from http://forum.openscenegraph.org/viewtopic.php?t=3289&view=previous
osg::Geometry * Stem::create_truncated_cone_geometry(float start_radius, float end_radius, float length, osg::Vec4 *colour, float bottom_height) {
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
	vertices->push_back(osg::Vec3f(0.0f, 0.0f, 0.0f + bottom_height));			
	bottomPrimitive->push_back(j++);

	// Create the surrounding vertices for the bottom
	for (int i = 0; i <= numberSegments; i++) {
		// Set the vertex location
		vertices->push_back(osg::Vec3f(xBottom[i], yBottom[i], 0.0f + bottom_height));

		// Set the index to the vertex in the bottom primitive
		bottomPrimitive->push_back(j++);
	}

	//
	// Create the vertices and indices for the top
	//

	// Create the centre vertex in the trianglestrip that will form the top
	vertices->push_back(osg::Vec3f(0.0f, 0.0f, length + bottom_height));
	topPrimitive->push_back(j++);

	// Create surrounding vertices for the top in reverse order, so the normals
	// point the other way
	for (int i = numberSegments; i >=0; i--) {
		// Set the vertex location
		vertices->push_back(osg::Vec3f(xTop[i], yTop[i], length + bottom_height));

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
} 

void Stem::make_stem() {
	// Make the assumption that each segment will be of equal length
	float seg_length = this->length / this->num_segments;
	
	// For now, just decrement radii by one for each segment
	// Assume head radius is one unit less than base radius
	float head_radius = this->base_radius * .75;
	
	float rotate_degrees_first_half;
	float rotate_degrees_second_half;
	float random_degrees_rotation;

	if (this->curve_back == 0) {
		rotate_degrees_first_half = this->curve / (float) this->num_segments;
		rotate_degrees_second_half = rotate_degrees_first_half;
	}
	else {
		rotate_degrees_first_half = this->curve / (this->num_segments / 2);
		rotate_degrees_second_half = this->curve_back / (this->num_segments / 2);
	}

	random_degrees_rotation = this->curve_variation / this->num_segments;

	osg::Vec3 top_of_prev_rotated_cylinder = osg::Vec3(0.0, 0.0, 0.0);

	make_stem_segments(this->num_segments, 0, this->base_radius, head_radius, rotate_degrees_second_half, rotate_degrees_first_half, random_degrees_rotation, 1, top_of_prev_rotated_cylinder, 1);

	if (this->orientation == Orientation::HORIZONTAL) {
		// For horizontal stems, run stem through 2 rotation transforms
		// The first transform converts it from a vertical to a horizontal stem
		// The second transform determines its position around the parent stem
		osg::PositionAttitudeTransform *pre_rotate = new osg::PositionAttitudeTransform;
		pre_rotate->addChild(this->stem);
		pre_rotate->setAttitude((osg::Quat(osg::DegreesToRadians(0.0f),
			osg::Vec3d(0, 0, 1)))*(osg::Quat(osg::DegreesToRadians(90.0f),
			osg::Vec3d(1, 0, 0)))*(osg::Quat(osg::DegreesToRadians(0.0f),
			osg::Vec3d(0, 1, 0))));
	
		this->base_pos = osg::Vec3(this->parent->get_base_pos().x(), this->parent->get_base_pos().y(), this->parent->get_base_pos().z() + this->length_onto_parent);
		osg::Vec3 top_pos_before_rotate = osg::Vec3(this->base_pos.x(), this->base_pos.y() - this->length/2, this->base_pos.z());
		osg::Matrixd rot_mat = osg::Matrixd((osg::Quat(osg::DegreesToRadians(this->diameter_rotation),
			osg::Vec3d(0, 0, 1)))*(osg::Quat(osg::DegreesToRadians(0.0f),
			osg::Vec3d(1, 0, 0)))*(osg::Quat(osg::DegreesToRadians(0.0f),
			osg::Vec3d(0, 1, 0))));
		this->top_pos = rot_mat * top_pos_before_rotate;

		this->stem_trans->addChild(pre_rotate);
		this->stem_trans->setAttitude((osg::Quat(osg::DegreesToRadians(this->diameter_rotation),
			osg::Vec3d(0, 0, 1)))*(osg::Quat(osg::DegreesToRadians(0.0f),
			osg::Vec3d(1, 0, 0)))*(osg::Quat(osg::DegreesToRadians(0.0f),
			osg::Vec3d(0, 1, 0))));
		// Position stem on the midpoint of parent stem.
		if (this->recursion_level > 0) {
			this->stem_trans->setPosition(this->base_pos);
		}
	} else if (this->orientation == Orientation::VERTICAL) {
		// For vertical stems, only need to determine position around parent stem
		this->stem_trans->addChild(this->stem);
		this->stem_trans->setAttitude((osg::Quat(osg::DegreesToRadians(0.0f),
			osg::Vec3d(0, 0, 1)))*(osg::Quat(osg::DegreesToRadians(0.0f + this->diameter_rotation),
			osg::Vec3d(1, 0, 0)))*(osg::Quat(osg::DegreesToRadians(0.0f),
			osg::Vec3d(0, 1, 0))));
		
		if (this->recursion_level > 0) {
			this->base_pos = this->parent->get_top_pos();
			this->stem_trans->setPosition(this->base_pos);
		}
	}

}

void Stem::make_stem_segments(int num_segments, int seg_index, float b_radius, float h_radius, float rotate_degrees_second_half, float rotate_degrees_first_half, float random_degrees_rotation, int left, osg::Vec3 pivot_point, int split_angle_sign) {
	// Return without recursively calling function if we have rendered all segments.
	if (num_segments == 0)
		return;

	float rotation_amount;
	float seg_length = this->length / this->num_segments;

	// Have orange color for stem.
	osg::Vec4* colour = new osg::Vec4(1.0, 0.5, 0.1, 1.0);

	// Determine whether segment is in first or second half of stem.
	if (seg_index >= (this->num_segments / 2)) {
		rotation_amount = rotate_degrees_second_half + random_degrees_rotation;
	} else {
		rotation_amount = rotate_degrees_first_half + random_degrees_rotation;
	}

	osg::Geode* stem_geode = new osg::Geode();
	osg::PositionAttitudeTransform* stem_transform =
		new osg::PositionAttitudeTransform();
	stem_transform->addChild(stem_geode);

	osg::Quat rot_quat = osg::Quat(osg::DegreesToRadians(rotation_amount), osg::Vec3d(1, 0, 0));

	// Attach segment to previous segment.
	stem_geode->addDrawable(this->create_truncated_cone_geometry(b_radius, h_radius, seg_length, colour, seg_length * seg_index));

	// Rotate cylinder.
	stem_transform->setPivotPoint(pivot_point);
	stem_transform->setAttitude(rot_quat);

	this->stem->addChild(stem_transform);
	// Continue to decrement base and head radius for each segment until we have reached smallest head radius
	b_radius = b_radius * .75;
	h_radius = b_radius * .75;

	// Recursively create stem segments and splits
	if (this->seg_splits == 0) {
		make_stem_segments(num_segments - 1, seg_index + 1, b_radius, h_radius, rotate_degrees_second_half, rotate_degrees_first_half, random_degrees_rotation, 1, pivot_point, 1);
	} else if (this->seg_splits == 1) {
		// Add branch split angle if branches split
		float split_a = (this->split_angle + this->split_angle_variation);
		make_stem_segments(num_segments - 1, seg_index + 1, b_radius, h_radius, rotate_degrees_second_half, rotate_degrees_first_half, random_degrees_rotation - split_a*2, -1, pivot_point, -1);
		make_stem_segments(num_segments - 1, seg_index + 1, b_radius, h_radius, rotate_degrees_second_half, rotate_degrees_first_half, random_degrees_rotation + split_a*2, 1, pivot_point, 1);
	}
}

// E solo un trucco!
