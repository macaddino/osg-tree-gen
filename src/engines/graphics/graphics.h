#ifndef _GRAPHICSENGINE_H
#define _GRAPHICSENGINE_H

#include <osg/Group>
#include <osgViewer/Viewer>

class GraphicsEngine {

	public:
		GraphicsEngine();

		void ignite();
		void cycle();
		void shutdown();

	private:
		osg::Group *root;
		osgViewer::Viewer viewer;

		void tree_init();								// creates a tree
		void viewer_init();							// initializes callbacks, viewing window, and scene
		void render();									// renders the world graph
		void create_object();		// creates a new tree to be rendered
};

#endif
