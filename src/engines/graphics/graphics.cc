#include "graphics.h"
#include "../../classes/stem.h"

#include <iostream>
#include <osg/Node>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Quat>
#include <osg/StateAttribute>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/Vec3>
#include <osg/ShapeDrawable> 
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerBase>
#include <ctime>
#include <cmath>

GraphicsEngine::GraphicsEngine() {}

void GraphicsEngine::ignite() {
	root = new osg::Group();
	tree_init();
	viewer_init();
}

void GraphicsEngine::cycle() {
	if (this->viewer.done())
		shutdown();
	render();
}

void GraphicsEngine::shutdown() {
	exit(1);
}

void GraphicsEngine::tree_init() {
	create_object();
}

void GraphicsEngine::viewer_init() {
	// Set up mouse camera manipulator
	this->viewer.setCameraManipulator(new osgGA::TrackballManipulator);

	// Assign the scene we created to the viewer
	this->viewer.setSceneData(this->root);
	// Create the windows and start the required threads
	this->viewer.realize();
}

void GraphicsEngine::render() {
	if (!this->viewer.done()) {
		// Dispatch the new frame, this wraps the following viewer operations:
		// 	advance() to the new frame
		//	eventTraversal() that collects events and passes them on to the 
		// event handlers and event callbacks
		// 	updateTraversal() to call the update callbacks
		//	renderingTraversals() that synchornizes all the rendering threads
		// (if any) and dispatch cull, draw, and swap buffers

		this->viewer.frame();
	}
}

void GraphicsEngine::create_object() {
	osg::PositionAttitudeTransform* obj_transform =
		new osg::PositionAttitudeTransform();
	Stem *stem_node_parent = NULL;
	Stem *stem_node_child = NULL;

	for (int i = 0; i < 2; ++i) {
		int orientation;
		if ((i % 2) == 0) {
			orientation = Orientation::VERTICAL;
		} else {
			orientation = Orientation::HORIZONTAL;
		}

		// Create a trunk with a bunch of subbranches.
		// Trunk is not set to have variable characteristics, but subbranches are.
		for (int j = 0; j < i * 17 + 1; ++j) {
			if (i == 0) {
				stem_node_parent = new Stem(i, orientation, 0.0, 70.0, 10.0, 0.0, osg::Vec3(0.0, 0.0, 0.0), 4, 0, 0, 0, 0, 0, 0.0, 0.0, NULL);
				osg::PositionAttitudeTransform* stem_trans = stem_node_parent->stem_trans;
				obj_transform->addChild(stem_trans);
			} else {
				float len_onto_parent = rand() % (int) (stem_node_parent->get_length() - (stem_node_parent->get_length() / 3)) + (stem_node_parent->get_length() / 3);
				float len_onto_parent_percentage = (stem_node_parent->get_length() - len_onto_parent) / stem_node_parent->get_length();
				int num_segments = len_onto_parent_percentage * 10 + 2;
				stem_node_child = new Stem(
					i,
					orientation,
					rand() % (180 - -180) + -180,
					(stem_node_parent->get_length() / len_onto_parent) * 10,
					len_onto_parent_percentage * 10.0,
					len_onto_parent,
					osg::Vec3(0.0, 0.0, 0.0),
					num_segments,
					rand() % (-10 - -90) + -90,
					0,
					rand() % (-10 - -90) + -90,
					0,
					0,
					10.0,
					rand() % (8 - 7) + 7,
					stem_node_parent);
				osg::PositionAttitudeTransform* stem_trans = stem_node_child->stem_trans;
				obj_transform->addChild(stem_trans);
				if (j == i * 15) {
					stem_node_parent = stem_node_child;
				}
			}
		}
	}

	osg::StateSet* state_set = new osg::StateSet();
	osg::Material *mat = new osg::Material;
	mat->setDiffuse(osg::Material::FRONT, osg::Vec4(1.0, 0.5, 0.1, 1.0));
	mat->setAmbient(osg::Material::FRONT, osg::Vec4(0.25, 0.0, 0.0, 1.0));
	mat->setEmission(osg::Material::FRONT, osg::Vec4(0.0, 0.0, 0.0, 1.0));
	state_set->setAttribute(mat);
	obj_transform->setStateSet(state_set);

	// Set position.
	osg::Vec3 obj_pos(0.0, 0.0, 0.0);
	obj_transform->setPosition(obj_pos);

	// Scale model such that it fits on the screen.
	float orig_mesh_radius = obj_transform->getBound().radius();
	float desired_radius = 9.5;
	float scale_amt = 1 / (orig_mesh_radius / desired_radius);
	obj_transform->setScale(osg::Vec3(scale_amt, scale_amt, scale_amt));

	this->root->addChild(obj_transform);
}
