#ifndef _STEM_H
#define _STEM_H

#include <osg/Group>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osg/Vec3>
#include <osg/Vec4>

enum Orientation {
	VERTICAL,
	HORIZONTAL
};

class Stem {
	public:
		Stem(int recursion_level, int orientation, float diameter_rotation, float length, float base_radius, float length_onto_parent, osg::Vec3 base_pos, int num_segments, int curve, int curve_back, int curve_variation, int seg_splits, int base_splits, float split_angle, float split_angle_variation, Stem* parent);
		osg::Group* stem;		// Node of the stem mesh containing all segments
		osg::PositionAttitudeTransform* stem_trans;		// Renderable transform node of stem
		float get_length();
		float get_base_radius();
		osg::Vec3 get_base_pos();
		osg::Vec3 get_top_pos();

	private:
		int recursion_level;
		int orientation;
		float diameter_rotation;	// Degree of rotation about the parent's z axis
		float length;				// Length of stem
		float base_radius;	// Radius of first segment in stem
		float length_onto_parent;		// How far from parent's base coords is stem placed
		osg::Vec3 base_pos;	// Pos coordinates of base of stem
		osg::Vec3 top_pos;

		int num_segments;		// Referred to as nCurveRes in Weber paper
		int curve;					// Referred to as nCurve in Weber paper
		int curve_back;			// Referred to as nCurveBack in Weber paper
		int curve_variation;// Referred to as nCurveV in the Weber paper

		int seg_splits;			// Referred to as nSegSplits in the Weber paper
		int base_splits;		// Referred to as nBaseSplits in the Weber paper
		float split_angle;	// Referred to as nSplitAngle in the Weber paper
		float split_angle_variation;	// Referred to as nSplitAngleV in Weber paper
		
		Stem* parent;

		osg::Geometry * create_truncated_cone_geometry(float start_radius, float end_radius, float length, osg::Vec4* colour, float bottom_height);
		void make_stem();
		void make_stem_segments(int num_segments, int seg_index, float b_radius, float h_radius, float rotate_degrees_second_half, float rotate_degrees_first_half, float random_degrees_rotation, int left, osg::Vec3 pivot_point, int prev_split_angle_sign);
};

#endif
