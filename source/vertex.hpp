#pragma once

struct Vertex {
	float position[3];
	TexCoord<float> texCoord;
	float normal[3];
};

const Vertex cubeVertices[6][4] = {
	// First face (PZ)
	{
    	{ {-0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, +1.0f} },
    	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, +1.0f} },
    	{ {+0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, +1.0f} },
    	{ {-0.5f, +0.5f, +0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, +1.0f} },
	},

	// Second face (MZ)
	{
    	{ {-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
    	{ {+0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
    	{ {-0.5f, +0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
    	{ {+0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
	},

	// Third face (PX)
	{
    	{ {+0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f} },
    	{ {+0.5f, +0.5f, +0.5f}, {0.0f, 0.0f}, {+1.0f, 0.0f, 0.0f} },
    	{ {+0.5f, +0.5f, -0.5f}, {1.0f, 0.0f}, {+1.0f, 0.0f, 0.0f} },
    	{ {+0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {+1.0f, 0.0f, 0.0f} },
	},

	// Fourth face (MX)
	{
    	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
    	{ {-0.5f, +0.5f, +0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
    	{ {-0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
    	{ {-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
	},

	// Fifth face (PY)
	{
    	{ {-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, +1.0f, 0.0f} },
    	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, +1.0f, 0.0f} },
    	{ {-0.5f, +0.5f, +0.5f}, {1.0f, 0.0f}, {0.0f, +1.0f, 0.0f} },
    	{ {+0.5f, +0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, +1.0f, 0.0f} },
    },

	// Sixth face (MY)
	{
    	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
    	{ {+0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
    	{ {+0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
    	{ {-0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
	},
};

const u16 faceIndices[6] = {
    0, 1, 2,
    1, 0, 3,
};
