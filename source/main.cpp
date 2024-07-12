#include "chunk.hpp"
#include "world.hpp"

// Shader includes
#include "vshader_shbin.h"

const u32 DISPLAY_TRANSFER_FLAGS = \
		(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
		GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
		GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));

const C3D_Material material = {
	{ 0.2f, 0.2f, 0.2f }, // Ambient
	{ 0.4f, 0.4f, 0.4f }, // Diffuse
	{ 0.8f, 0.8f, 0.8f }, // Specular0
	{ 0.0f, 0.0f, 0.0f }, // Specular1
	{ 0.0f, 0.0f, 0.0f }, // Emission
};

C3D_FVec vec3ScalarMultiply(C3D_FVec v, float s) {
    return C3D_FVec{ .z = v.z * s, .y = v.y * s, .x = v.x * s };
}

// Function to load a T3X texture from a file
C3D_Tex* loadT3XTexture(const char* filename) {
    // Load the T3X file into memory
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cout << "Failed to open file '" << filename << "'" << std::endl;
        return nullptr;
    }

    // Parse the T3X file
    C3D_Tex* texture = (C3D_Tex*)malloc(sizeof(C3D_Tex));
    Tex3DS_Texture t3x = Tex3DS_TextureImportStdio(file, texture, nullptr, true);
    if (!t3x) {
        std::cout << "Failed to create texture '" << filename << "'" << std::endl;
        return nullptr;
    }

    // Free the T3X file data
    Tex3DS_TextureFree(t3x);

    return texture;
}

const float NEAR_PLANE = 0.001f;
const float FAR_PLANE = RENDER_DISTANCE * CHUNK_WIDTH;

const u32 BG_COLOR_WITH_ALPHA = 0x0080E0FF;
const u32 BG_COLOR_REVERSED = 0xE08000;

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

    Result res = romfsInit();
    if (res != 0) {
        std::cout << "Failed to initialize RomFS (code: 0x" << std::hex << res << ")" << std::endl;
        return 1;
    }

	// -------- Graphics initialization --------

	// Top render target
	C3D_RenderTarget* topRenderTarget = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetOutput(topRenderTarget, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	// Bottom render target
	C3D_RenderTarget* bottomRenderTarget = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetOutput(bottomRenderTarget, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	// Shader
	DVLB_s* vertexShader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgram_s program;
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vertexShader_dvlb->DVLE[0]);

	// Uniforms
	C3D_BindProgram(&program);
	int uViewProj = shaderInstanceGetUniformLocation(program.vertexShader, "viewProj");
	int uPosition = shaderInstanceGetUniformLocation(program.vertexShader, "position");

	// Vertex attributes
	C3D_AttrInfo* attributeInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attributeInfo);
	AttrInfo_AddLoader(attributeInfo, 0, GPU_FLOAT, 3); // Position
	AttrInfo_AddLoader(attributeInfo, 1, GPU_FLOAT, 2); // Texture coordinate
	AttrInfo_AddLoader(attributeInfo, 2, GPU_FLOAT, 3); // Normal

	// Load texture
	C3D_Tex* texture = loadT3XTexture("romfs:/gfx/texture_atlas.t3x");
	C3D_TexBind(0, texture);

	// Environment
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvInit(env);
	C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_FRAGMENT_PRIMARY_COLOR, (GPU_TEVSRC)0);
	C3D_TexEnvFunc(env, C3D_RGB, GPU_MODULATE);
	//env = C3D_GetTexEnv(1);
	//C3D_TexEnvInit(env);
	//C3D_TexEnvSrc(env, C3D_Both, GPU_PREVIOUS, GPU_FRAGMENT_SECONDARY_COLOR, (GPU_TEVSRC)0);
	//C3D_TexEnvFunc(env, C3D_RGB, GPU_ADD);

	// Light
	C3D_LightEnv lightEnvironment;
	C3D_LightEnvInit(&lightEnvironment);
	C3D_LightEnvBind(&lightEnvironment);
	C3D_LightEnvMaterial(&lightEnvironment, &material);

	// Light LUT texture
	C3D_LightLut phongLut;
	LightLut_Phong(&phongLut, 30);
	C3D_LightEnvLut(&lightEnvironment, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &phongLut);

	// Lighting properties
	C3D_FVec lightVector = {{ .z = 0.5f, .y = 1.0f, .x = 0.25f }};
	C3D_Light light;
	C3D_LightInit(&light, &lightEnvironment);
	C3D_LightColor(&light, 1.0, 1.0, 1.0);
	C3D_LightPosition(&light, &lightVector);

	// Fog
	C3D_FogLut fogLut;
	FogLut_Exp(&fogLut, 0.05f, 1.0f, 48.0f, 50.0f);
	//C3D_FogGasMode(GPU_FOG, GPU_PLAIN_DENSITY, false);
	C3D_FogColor(BG_COLOR_REVERSED);
	//C3D_FogLutBind(&fogLut);

	// Alpha test
	C3D_AlphaTest(true, GPU_GREATER, 0.5f);

	// Camera
	Camera camera;
	camera.position.y = CHUNK_HEIGHT + 2.0f;

	// World
	World world(camera, uPosition);

	touchPosition lastTouch{0, 0};

	// HACK
	float t = 0.0f;

	// -------- Main loop --------
    while (aptMainLoop()) {
        // TODO: should this be here?
        gspWaitForVBlank();
        hidScanInput();

        C3D_FVec dpad{.y = 0.0f, .x = 0.0f};
        if (hidKeysHeld() & KEY_UP)
            dpad.y += 1.0f;
        if (hidKeysHeld() & KEY_DOWN)
            dpad.y -= 1.0f;
        // TODO: find out why left and right is swapped
        if (hidKeysHeld() & KEY_LEFT)
            dpad.x += 1.0f;
        if (hidKeysHeld() & KEY_RIGHT)
            dpad.x -= 1.0f;

        if (dpad.x != 0.0f || dpad.y != 0.0f)
            dpad = FVec3_Normalize(dpad);

        // Move
        C3D_FVec movement =                     vec3ScalarMultiply(FVec3_Cross(camera.direction, camera.up), dpad.x * 0.5f);
                 movement = FVec3_Add(movement, vec3ScalarMultiply(            camera.direction            , dpad.y * 0.5f));
        //movement.y = 0.0f;
        camera.position = FVec3_Add(camera.position, movement);

        // Rotate
        touchPosition touch;
        hidTouchRead(&touch);
        // If held, but not pressed just this frame
        if ((hidKeysHeld() & KEY_TOUCH) && !(hidKeysDown() & KEY_TOUCH)) {
            float rotX = (touch.py - lastTouch.py) * 0.008f;
            float rotY = (touch.px - lastTouch.px) * 0.008f;

            camera.direction = Quat_Rotate(camera.direction, camera.up,                                                -rotY, false);
            camera.direction = Quat_Rotate(camera.direction, FVec3_Normalize(FVec3_Cross(camera.direction, camera.up)), rotX, false);
        }
        lastTouch = touch;

        float slider = osGet3DSliderState();
    	float interOcularDistance = slider / 3.0f;

        // Render
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        {
            C3D_RenderTargetClear(topRenderTarget, C3D_CLEAR_ALL, BG_COLOR_WITH_ALPHA, 0);
           	C3D_FrameDrawOn(topRenderTarget);

            // Projection matrx
            C3D_Mtx projection;
    		Mtx_PerspStereoTilt(&projection, C3D_AngleFromDegrees(40.0f), C3D_AspectRatioTop, NEAR_PLANE, FAR_PLANE, interOcularDistance, 2.0f, true);

            // View matrix
            C3D_Mtx view;
            // HACK: divide the position by 2
            C3D_FVec tempPos = vec3ScalarMultiply(camera.position, 0.5f);
    		Mtx_LookAt(&view, tempPos, FVec3_Add(tempPos, camera.direction), camera.up, true);

            C3D_Mtx viewProj;
            Mtx_Multiply(&viewProj, &projection, &view);

            //std::cout << camera.position.x << " " << camera.position.z << std::endl;

    		// Update uniforms
    		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uViewProj, &viewProj);

            // Draw the world
            world.render();
        }
        C3D_FrameEnd(0);

        t += 0.05f;

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
