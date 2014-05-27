#include <string.h>
#include <iostream>
#include <unistd.h>

#include "engines/graphics/graphics.h"

int main() {
	GraphicsEngine *g = new GraphicsEngine();
	g->ignite();
	while (1) {
		g->cycle();
	}

	return 0;
}
