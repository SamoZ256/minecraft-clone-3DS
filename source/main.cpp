#include <iostream>

#include <3ds.h>
#include <citro3d.h>

// Shader includes
#include "vshader_shbin.h"

const u32 DISPLAY_TRANSFER_FLAGS = \
		(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
		GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
		GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));

struct Vertex {
	float positions[3];
	float texcoords[2];
	float normals[3];
};

const Vertex vertices[] = {
	// First face (PZ)
	// First triangle
	{ {-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, +1.0f} },
	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, +1.0f} },
	{ {+0.5f, -0.5f, +0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, +1.0f} },
	// Second triangle
	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, +1.0f} },
	{ {-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, +1.0f} },
	{ {-0.5f, +0.5f, +0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, +1.0f} },

	// Second face (MZ)
	// First triangle
	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
	{ {+0.5f, +0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
	{ {-0.5f, +0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
	// Second triangle
	{ {+0.5f, +0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
	{ {+0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },

	// Third face (PX)
	// First triangle
	{ {+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {+1.0f, 0.0f, 0.0f} },
	{ {+0.5f, +0.5f, -0.5f}, {1.0f, 0.0f}, {+1.0f, 0.0f, 0.0f} },
	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f} },
	// Second triangle
	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f} },
	{ {+0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {+1.0f, 0.0f, 0.0f} },
	{ {+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {+1.0f, 0.0f, 0.0f} },

	// Fourth face (MX)
	// First triangle
	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
	{ {-0.5f, -0.5f, +0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
	{ {-0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
	// Second triangle
	{ {-0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
	{ {-0.5f, +0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },

	// Fifth face (PY)
	// First triangle
	{ {-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, +1.0f, 0.0f} },
	{ {-0.5f, +0.5f, +0.5f}, {1.0f, 0.0f}, {0.0f, +1.0f, 0.0f} },
	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, +1.0f, 0.0f} },
	// Second triangle
	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, +1.0f, 0.0f} },
	{ {+0.5f, +0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, +1.0f, 0.0f} },
	{ {-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, +1.0f, 0.0f} },

	// Sixth face (MY)
	// First triangle
	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
	{ {+0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
	{ {+0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
	// Second triangle
	{ {+0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
	{ {-0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
};

const C3D_Material material = {
	{ 0.2f, 0.2f, 0.2f }, // Ambient
	{ 0.4f, 0.4f, 0.4f }, // Diffuse
	{ 0.8f, 0.8f, 0.8f }, // Specular0
	{ 0.0f, 0.0f, 0.0f }, // Specular1
	{ 0.0f, 0.0f, 0.0f }, // Emission
};

int main(int argc, char **argv) {
    // Initialization
    srvInit();
    aptInit();
    hidInit();
    gfxInitDefault();
    gfxSet3D(true);
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

    // Init console for debugging
    consoleInit(GFX_BOTTOM, NULL);

	// -------- Graphics initialization --------

	// Top render target
	C3D_RenderTarget* topRenderTarget = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetOutput(topRenderTarget, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	C3D_RenderTargetClear(topRenderTarget, C3D_CLEAR_ALL, 0xFFFFFFFF, 0);

	// Bottom render target
	C3D_RenderTarget* bottomRenderTarget = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetOutput(bottomRenderTarget, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	C3D_RenderTargetClear(bottomRenderTarget, C3D_CLEAR_ALL, 0xFF0000FF, 0);

	// Shader
	DVLB_s* vertexShader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgram_s program;
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vertexShader_dvlb->DVLE[0]);

	// Uniforms
	C3D_BindProgram(&program);
	int uProjection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
	int uModelView = shaderInstanceGetUniformLocation(program.vertexShader, "modelView");

	// Vertex attributes
	C3D_AttrInfo* attributeInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attributeInfo);
	AttrInfo_AddLoader(attributeInfo, 0, GPU_FLOAT, 3); // Position
	AttrInfo_AddLoader(attributeInfo, 1, GPU_FLOAT, 2); // Texture coordinate
	AttrInfo_AddLoader(attributeInfo, 2, GPU_FLOAT, 3); // Normal

	// Vertex buffer
	void* vboData = linearAlloc(sizeof(vertices));
	memcpy(vboData, vertices, sizeof(vertices));
	C3D_BufInfo* bufferInfo = C3D_GetBufInfo();
	BufInfo_Init(bufferInfo);
	// TODO: what is the 0x210?
	BufInfo_Add(bufferInfo, vboData, sizeof(Vertex), 3, 0x210);

	// Environment
	C3D_TexEnv* environment = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(environment, C3D_Both, GPU_FRAGMENT_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR, GPU_PRIMARY_COLOR);
	C3D_TexEnvOpRgb(environment, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR);
	C3D_TexEnvOpAlpha(environment, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA);
	C3D_TexEnvFunc(environment, C3D_Both, GPU_ADD);

	C3D_LightEnv lightEnvironment;
	C3D_LightEnvInit(&lightEnvironment);
	C3D_LightEnvBind(&lightEnvironment);
	C3D_LightEnvMaterial(&lightEnvironment, &material);

	C3D_LightLut lutPhong;
	LightLut_Phong(&lutPhong, 30);
	C3D_LightEnvLut(&lightEnvironment, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &lutPhong);

	// Lighting properties
	C3D_FVec lightVector = {{ 16.0, 0.5, 0.0, 0.0 }};
	C3D_Light light;
	C3D_LightInit(&light, &lightEnvironment);
	C3D_LightColor(&light, 1.0, 1.0, 1.0);
	C3D_LightPosition(&light, &lightVector);

	// TODO: remove
	float distZ = 0.0f;
	float angleX = 0.0f;
	float angleY = 0.0f;

	// -------- Main loop --------
    while (aptMainLoop()) {
        std::cout << "Hello world" << std::endl;
        gspWaitForVBlank();
        hidScanInput();

        if (hidKeysDown() & KEY_START)
            break;

        float slider = osGet3DSliderState();
    	float interOcularDistance = slider / 3.0f;

        // Render
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        {
            //C3D_RenderTargetClear(topRenderTarget, C3D_CLEAR_ALL, 0xFFFFFFFF, 0);
           	C3D_FrameDrawOn(topRenderTarget);

            // Compute projection matrix.
            C3D_Mtx projectionMatrix;
            Mtx_Identity(&projectionMatrix);
    		//Mtx_PerspStereoTilt(&projectionMatrix, 40.0f * (std::acos(-1) / 180.0f), 400.0f / 240.0f, 0.01f, 1000.0f, interOcularDistance, 2.0f, false);
    		//Mtx_Translate(&projectionMatrix, 0.0, 0.0, -10.0 + distZ, false);

    		// Calculate model view matrix.
    		C3D_Mtx modelView;
    		Mtx_Identity(&modelView);
    		//Mtx_Translate(&modelView, 0.0, 0.0, -2.0 + sinf(angleX), false);
    		//Mtx_RotateX(&modelView, angleX, true);
    		//Mtx_RotateY(&modelView, angleY, true);

    		//if (interOcularDistance >= 0.0f){
    		//	angleX += 0.05f;
    		//	angleY += 0.05f;
    		//}

    		// Update uniforms
    		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uProjection, &projectionMatrix);
    		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uModelView, &modelView);

    		// Draw the vertex buffer objects.
    		C3D_DrawArrays(GPU_TRIANGLES, 0, sizeof(vertices) / sizeof(Vertex));
        }
        C3D_FrameEnd(0);

        // Present
        //gfxFlushBuffers();
        //gfxSwapBuffers();
    }

    // Graphics deinitialization
    shaderProgramFree(&program);
	DVLB_Free(vertexShader_dvlb);

	// Deinitialization
    gfxExit();
    hidExit();
    // TODO: find out why this crashes
    //aptExit();
    srvExit();

    return 0;
}
