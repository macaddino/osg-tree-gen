#ifndef _STEM_H
#define _STEM_H

#include <osg/Group>
#include <osg/Geometry>
#include <osg/Vec4>

class Stem {
	public:
		Stem(int recursion_level, int num_segments, int curve_back, int curve_variation, Stem* parent);
		osg::Group* stem;		// Renderable node of the stem

	private:
		int recursion_level;
		int num_segments;		// Referred to as nCurveRes in Weber paper
		int curve;				// Referred to as nCurve in Weber paper
		int curve_back;			// Referred to as nCurveBack in Weber paper
		int curve_variation;		// Referred to as nCurveV in the Weber paper
		Stem* parent;

		osg::Geometry * create_truncated_cone_geometry(float start_radius, float end_radius, float length, osg::Vec4* colour);
		osg::Group * make_stem(int num_segments, float stem_len);
};

#endif
