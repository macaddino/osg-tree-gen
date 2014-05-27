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

GraphicsEngine::GraphicsEngine() {}

void GraphicsEngine::ignite() {
	root = new osg::Group();
	tree_init();
	viewer_init();
}

void GraphicsEngine::cycle() {

	if (this->viewer.done())
		shutdown();

	//clock_t time1 = clock();
	// check packet queue for updates
	// if found object and event packet, compare each object to list of rendered objects
	// for each object which is already rendered, set UpdateObjectCallback (simply a boolean) to true -- this boolean can be mapped to the object
	// for each object we need to render which is not already present, call a function which creates a new object and adds it to the group, sets its updatecallback, etc
	// for each object which was rendered but isn't anymore, remove this object from the group node and free its update callback and free the node
	// SO for each object we need a struct which encompasses the obj ID, the UpdateObjectCallback bool, the object Node, and perhaps the transform matrix?

	render();

	//clock_t time2 = clock();
	//clock_t timediff = time2 - time1;
	//float timediff_sec = ((float)timediff) / CLOCKS_PER_SEC;
	//std::cout << "CYCLE LENGTH: " << timediff_sec << "SECONDS" << std::endl;
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
	// Three segments to the stem
	Stem *stem_node = new Stem(1, 3, 0, 1, 1, NULL);
	osg::Group* stem_group = stem_node->stem;
	osg::PositionAttitudeTransform* obj_transform =
		new osg::PositionAttitudeTransform();
	obj_transform->addChild(stem_group);

	osg::StateSet* state_set = new osg::StateSet();
	osg::Material *mat = new osg::Material;
	mat->setDiffuse(osg::Material::FRONT, osg::Vec4(1.0, 0.5, 0.1, 1.0));
	mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 1.0, 1.0, 1.0));
	mat->setAmbient(osg::Material::FRONT, osg::Vec4(0.25, 0.25, 0.25, 1.0));
	mat->setEmission(osg::Material::FRONT, osg::Vec4(0.0, 0.0, 0.0, 1.0));
	mat->setShininess(osg::Material::FRONT, 63.0);
	state_set->setAttribute(mat);
	obj_transform->setStateSet(state_set);

	// Set position.
	osg::Vec3 obj_pos(0.0, 0.0, 0.0);
	obj_transform->setPosition(obj_pos);

	float orig_mesh_radius = obj_transform->getBound().radius();
	// TODO: SET OBJECT RADIUS FROM STEM
	float desired_radius = 9.5;
	float scale_amt = 1 / (orig_mesh_radius / desired_radius);
	obj_transform->setScale(osg::Vec3(scale_amt, scale_amt, scale_amt));

	this->root->addChild(obj_transform);
}
