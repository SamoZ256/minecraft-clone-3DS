#include "block.hpp"
#include "chunk.hpp"
#include "world.hpp"

// Shader includes
#include "main_vertex_shbin.h"
#include "gui_vertex_shbin.h"

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

// Perspective
const float NEAR_PLANE = 0.001f;
const float FAR_PLANE = RENDER_DISTANCE * CHUNK_WIDTH;

// Background color
const u32 BG_COLOR_WITH_ALPHA = 0x0080E0FF;
const u32 BG_COLOR_REVERSED = 0xE08000;

// Player
const float GRAVITY = -15.0f;
const float JUMP_SPEED = 8.0f;
const float SPEED_IN_FLIGHT_MODE = 8.0f;
const float SPEED = 5.0f;
const u8 MAX_JUMPS = 3;

// Timers
const float CLICK_TIME = 0.1f;

struct GuiVertex {
    float position[2];
};

const GuiVertex guiVertices[] = {
    { { -1.0f, -1.0f } },
    { {  1.0f, -1.0f } },
    { { -1.0f,  1.0f } },
    { {  1.0f,  1.0f } },
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
    //consoleInit(GFX_BOTTOM, NULL);

    Result res = romfsInit();
    if (res != 0) {
        std::cout << "Failed to initialize RomFS (code: 0x" << std::hex << res << ")" << std::endl;
        return 1;
    }

    // Random seed
    srand(time(NULL));

	// -------- Graphics initialization --------

	// Top render target
	C3D_RenderTarget* topRenderTarget = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetOutput(topRenderTarget, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	// Bottom render target
	C3D_RenderTarget* bottomRenderTarget = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH16);
	C3D_RenderTargetSetOutput(bottomRenderTarget, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	// Shader
	DVLB_s* mainVertexShaderDvlb = DVLB_ParseFile((u32*)main_vertex_shbin, main_vertex_shbin_size);
	shaderProgram_s mainProgram;
	shaderProgramInit(&mainProgram);
	shaderProgramSetVsh(&mainProgram, &mainVertexShaderDvlb->DVLE[0]);

	DVLB_s* guiVertexShaderDvlb = DVLB_ParseFile((u32*)gui_vertex_shbin, gui_vertex_shbin_size);
	shaderProgram_s guiProgram;
	shaderProgramInit(&guiProgram);
	shaderProgramSetVsh(&guiProgram, &guiVertexShaderDvlb->DVLE[0]);

	// Uniforms
	int uMainViewProj = shaderInstanceGetUniformLocation(mainProgram.vertexShader, "viewProj");
	int uMainPosition = shaderInstanceGetUniformLocation(mainProgram.vertexShader, "position");

	int uGuiPosition = shaderInstanceGetUniformLocation(guiProgram.vertexShader, "position");
	int uGuiScale = shaderInstanceGetUniformLocation(guiProgram.vertexShader, "scale");
	int uGuiTexOffset = shaderInstanceGetUniformLocation(guiProgram.vertexShader, "texOffset");

	// Vertex attributes
	C3D_AttrInfo mainAttrInfo;
	AttrInfo_Init(&mainAttrInfo);
	AttrInfo_AddLoader(&mainAttrInfo, 0, GPU_FLOAT, 3); // Position
	AttrInfo_AddLoader(&mainAttrInfo, 1, GPU_FLOAT, 2); // Texture coordinate
	AttrInfo_AddLoader(&mainAttrInfo, 2, GPU_FLOAT, 3); // Normal

	C3D_AttrInfo guiAttrInfo;
	AttrInfo_Init(&guiAttrInfo);
	AttrInfo_AddLoader(&guiAttrInfo, 0, GPU_FLOAT, 2); // Position

	// Vertex buffers
	void* guiVboData = linearAlloc(sizeof(guiVertices));
	memcpy(guiVboData, guiVertices, sizeof(guiVertices));
	C3D_BufInfo guiVbo;
	BufInfo_Init(&guiVbo);
	BufInfo_Add(&guiVbo, guiVboData, sizeof(GuiVertex), 1, 0x0);

	// Load texture
	C3D_Tex* texture = loadT3XTexture("romfs:/gfx/texture_atlas.t3x");
	C3D_TexBind(0, texture);

	// TEV

	// Main
	C3D_TexEnv mainEnv0;
	C3D_TexEnvInit(&mainEnv0);
	C3D_TexEnvSrc(&mainEnv0, C3D_Both, GPU_TEXTURE0, GPU_FRAGMENT_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
	C3D_TexEnvFunc(&mainEnv0, C3D_RGB, GPU_MODULATE);

	C3D_TexEnv mainEnv1;
	C3D_TexEnvInit(&mainEnv1);
	C3D_TexEnvSrc(&mainEnv1, C3D_RGB, GPU_CONSTANT, GPU_PREVIOUS, GPU_PRIMARY_COLOR);
	C3D_TexEnvFunc(&mainEnv1, C3D_RGB, GPU_INTERPOLATE);
	C3D_TexEnvSrc(&mainEnv1, C3D_Alpha, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
	C3D_TexEnvFunc(&mainEnv1, C3D_Alpha, GPU_REPLACE);
	C3D_TexEnvColor(&mainEnv1, BG_COLOR_REVERSED);

	// GUI
	C3D_TexEnv guiEnv0;
	C3D_TexEnvInit(&guiEnv0);
	C3D_TexEnvSrc(&guiEnv0, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
	C3D_TexEnvFunc(&guiEnv0, C3D_RGB, GPU_REPLACE);

	C3D_TexEnv guiEnv1;
	C3D_TexEnvInit(&guiEnv1);

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
	C3D_FVec lightVector = float3(0.25f, 1.0f, 0.5f);
	C3D_Light light;
	C3D_LightInit(&light, &lightEnvironment);
	C3D_LightColor(&light, 1.0, 1.0, 1.0);
	C3D_LightPosition(&light, &lightVector);

	// Fog
	//C3D_FogLut fogLut;
	//FogLut_Exp(&fogLut, -0.5f, 1.0f, NEAR_PLANE, FAR_PLANE);
	//C3D_FogGasMode(GPU_FOG, GPU_PLAIN_DENSITY, false);
	//C3D_FogColor(BG_COLOR_REVERSED);
	//C3D_FogLutBind(&fogLut);

	// Camera
	Camera camera;
	camera.aabb.position.y = CHUNK_HEIGHT + 2.0f;
	camera.aabb.scale = float3(0.6f, 1.8f, 0.6f);

	bool flyMode = false;
	float yMomentum = 0.0f;
	u8 jumpCount = MAX_JUMPS;

	// World
	World world(camera, uMainPosition);

	touchPosition lastTouch{0, 0};

	// Timers
	float clickTimer = 0.0f;

	// -------- Main loop --------
    while (aptMainLoop()) {
        // TODO: should this be here?
        gspWaitForVBlank();
        hidScanInput();

        // Calculate delta time
        // HACK
        float dt = 1.0f / 30.0f;

        // Timers
        if (clickTimer != 0.0f) {
            clickTimer = std::max(clickTimer - dt, 0.0f);
        }

        // Click
        if (hidKeysDown() & KEY_TOUCH) {
            clickTimer = CLICK_TIME;
        }

        // Rotate
        touchPosition touch;
        hidTouchRead(&touch);
        // If held, but not pressed just this frame
        if (hidKeysHeld() & KEY_TOUCH && clickTimer == 0.0f) {
            float rotX = (touch.py - lastTouch.py) * 0.008f;
            float rotY = (touch.px - lastTouch.px) * 0.008f;

            camera.direction = Quat_Rotate(camera.direction, camera.up,                                                -rotY, false);
            camera.direction = Quat_Rotate(camera.direction, FVec3_Normalize(FVec3_Cross(camera.direction, camera.up)), rotX, false);
        }
        lastTouch = touch;

        // D-Pad
        C3D_FVec dpad = float3(0.0f);
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
        float speed = flyMode ? SPEED_IN_FLIGHT_MODE : SPEED;
        C3D_FVec movement = FVec3_Cross(camera.direction, camera.up) * dpad.x * dt * speed;
                 movement =  movement + camera.direction             * dpad.y * dt * speed;
        if (flyMode) {
            camera.aabb.position = camera.aabb.position + movement;
        } else {
            movement.y = yMomentum * dt;
            yMomentum += GRAVITY * dt;
            yMomentum = std::min(yMomentum, 10.0f);
            bool isOnGround = false;
            bool wallJump = false;
            world.moveCamera(movement, isOnGround, wallJump);
            if (isOnGround) {
                yMomentum = 0.0f;
            }
            if (wallJump || isOnGround) {
                jumpCount = MAX_JUMPS;
            }
            bool jump = (((hidKeysDown() & KEY_A) || (hidKeysDown() & KEY_B)) && jumpCount > 0);
            if (jump || wallJump) {
                yMomentum = JUMP_SPEED;
                if (jump) {
                    jumpCount--;
                }
            }
        }

        // Block breaking and placing
        if ((hidKeysDown() & KEY_X) || (hidKeysDown() & KEY_Y)) {
            Intersection intersection = world.getIntersection();
            if (hidKeysDown() & KEY_X) {
                world.breakBlock(intersection);
            } else {
                world.placeBlock(intersection, BlockType::Dirt);
            }
        }

        float slider = osGet3DSliderState();
    	float interOcularDistance = slider / 3.0f;

        // Render
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        {
            // -------- Top screen --------
            C3D_RenderTargetClear(topRenderTarget, C3D_CLEAR_ALL, BG_COLOR_WITH_ALPHA, 0);
           	C3D_FrameDrawOn(topRenderTarget);

            // Projection matrx
            C3D_Mtx projection;
    		Mtx_PerspStereoTilt(&projection, C3D_AngleFromDegrees(40.0f), C3D_AspectRatioTop, NEAR_PLANE, FAR_PLANE, interOcularDistance, 2.0f, true);

            // View matrix
            C3D_Mtx view;
    		Mtx_LookAt(&view, float3(0.0f), camera.direction, camera.up, true);

            C3D_Mtx viewProj;
            Mtx_Multiply(&viewProj, &projection, &view);

            // Bind shader program
            C3D_BindProgram(&mainProgram);

            // Bind attribute info
            C3D_SetAttrInfo(&mainAttrInfo);

            // Bind TEV
            C3D_SetTexEnv(0, &mainEnv0);
            C3D_SetTexEnv(1, &mainEnv1);

            // Depth test
            C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);

           	// Alpha test
           	C3D_AlphaTest(true, GPU_GREATER, 0.5f);

    		// Update uniforms
    		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uMainViewProj, &viewProj);

            // Draw the world
            world.render();

            // -------- Bottom screen --------
            C3D_RenderTargetClear(bottomRenderTarget, C3D_CLEAR_ALL, 0xFFFFFFFF, 0);
           	C3D_FrameDrawOn(bottomRenderTarget);

            // Bind shader program
            C3D_BindProgram(&guiProgram);

            // Bind attribute info
            C3D_SetAttrInfo(&guiAttrInfo);

            // Bind TEV
            C3D_SetTexEnv(0, &guiEnv0);
            C3D_SetTexEnv(1, &guiEnv1);

            // Depth test
            C3D_DepthTest(false, GPU_ALWAYS, GPU_WRITE_COLOR);

           	// Alpha test
           	C3D_AlphaTest(false, GPU_ALWAYS, 0.0f);

            // Bind VBO
            C3D_SetBufInfo(&guiVbo);

            // Set scale
            C3D_FVUnifSet(GPU_VERTEX_SHADER, uGuiScale, 1.0f, 1.0f, 1.0f, 1.0f);

            for (u8 i = 0; i < 5; i++) {
                // Set position
                C3D_FVUnifSet(GPU_VERTEX_SHADER, uGuiPosition, 0.0f, 0.0f, 0.0f, 0.0f);

                // Set texture offset
                C3D_FVUnifSet(GPU_VERTEX_SHADER, uGuiTexOffset, (float)i * TEXTURE_SIZE_WITH_BORDER_NORM, 0.0f, 0.0f, 0.0f);

                // Draw
                C3D_DrawArrays(GPU_TRIANGLE_STRIP, 0, 4);
            }
        }
        C3D_FrameEnd(0);

        // Present
        //gfxFlushBuffers();
        //gfxSwapBuffers();
    }

    // Graphics deinitialization
    shaderProgramFree(&mainProgram);
	DVLB_Free(mainVertexShaderDvlb);

	// Deinitialization
    gfxExit();
    hidExit();
    // TODO: find out why this crashes
    //aptExit();
    srvExit();

    return 0;
}
