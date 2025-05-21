#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// Utils
#include "Camera.h"
#include "CameraManipulator.h"
#include "GLUtils.hpp"

struct SUpdateInfo
{
	float ElapsedTimeInSec = 0.0f; // Elapsed time since the start of the program
	float DeltaTimeInSec   = 0.0f; // Elapsed time since the last update
};

struct Ray
{
	glm::vec3 origin;
	glm::vec3 direction;
};

struct Intersection
{
	glm::vec2 uv;
	float t;
};

enum class DebugView
{
	Default = 0,
	NormalAsColor,
	ShadingNoTexture,
	DiffuseTextureOnly,
	ShininessTextureOnly
};




class CMyApp
{
public:
	CMyApp();
	~CMyApp();

	bool Init();
	void Clean();

	void Update(const SUpdateInfo&);
	void Render();
	void RenderGUI();

	void KeyboardDown(const SDL_KeyboardEvent&);
	void KeyboardUp(const SDL_KeyboardEvent&);
	void MouseMove(const SDL_MouseMotionEvent&);
	void MouseDown(const SDL_MouseButtonEvent&);
	void MouseUp(const SDL_MouseButtonEvent&);
	void MouseWheel(const SDL_MouseWheelEvent&);
	void Resize(int, int);

	void OtherEvent(const SDL_Event&);

protected:
	void SetupDebugCallback();

	// Data variables

	float m_ElapsedTimeInSec = 0.0f;
	// Robot mesh objects
	OGLObject m_torso, m_head;
	OGLObject m_rightUpperArm, m_rightLowerArm;
	OGLObject m_leftUpperArm, m_leftLowerArm;
	OGLObject m_leftLeg, m_rightLeg;

	// Robot texture
	GLuint m_robotTextureID = 0;

	// Animation/GUI state
	float m_headAngleX = 0.0f, m_headAngleY = 0.0f;
	float m_rightUpperArmAngle = 0.0f, m_rightLowerArmAngle = 0.0f;
	float m_leftUpperArmAngle = 0.0f, m_leftLowerArmAngle = 0.0f;

	// OBJ loader helper
	MeshObject<Vertex> LoadMeshFromOBJ(const std::string& filePath);


    // Robot

	static constexpr float LEG_SPEED = 1.0;
	static constexpr float LEG_SPREAD = 1.5;
	static constexpr glm::vec3 RELATIVE_LEFT_ARM_LOWER = glm::vec3(0.114, -4.633, -0.005);
	static constexpr glm::vec3 RELATIVE_RIGHT_ARM_LOWER = glm::vec3(-0.114, -4.633, -0.005);
	static constexpr glm::vec3 TORSO_POS = glm::vec3(0, 17.525, 0);

	static constexpr glm::vec3 RELATIVE_LEFT_ARM_UPPER = glm::vec3(5.402, 2.966, -0.878);
	static constexpr glm::vec3 RELATIVE_RIGHT_ARM_UPPER = glm::vec3(-5.402, 2.966, -0.878);
	static constexpr glm::vec3 RELATIVE_HEAD_POS = glm::vec3(0.000, 5.928, -0.822);
	static constexpr glm::vec3 RELATIVE_LEFT_LEG_POS = glm::vec3(2.867, -4.883, -0.771);
	static constexpr glm::vec3 RELATIVE_RIGHT_LEG_POS = glm::vec3(-2.867, -4.883, -0.771);
	static constexpr glm::vec3 AVG_EYE_POS_OFFSET = glm::vec3(0, 5, 3.5);

    // Table
	static constexpr glm::vec3 TABLE_SIZE = glm::vec3(200.0f, 10.0f, 200.0f);
	static constexpr glm::vec3 TABLE_POS = glm::vec3(0, -TABLE_SIZE.y * 0.5f, 0);
	static constexpr glm::vec3 TABLE_LEG_SIZE = glm::vec3(20, 120, 20);

	// Picking

	glm::ivec2 m_PickedPixel = glm::ivec2( 0, 0 );
	bool m_IsPicking = false;
	bool m_IsCtrlDown = false;

	glm::uvec2 m_windowSize = glm::uvec2(0, 0);

	Ray CalculatePixelRay(glm::vec2 pickerPos) const;


	// Camera
	Camera m_camera;
	CameraManipulator m_cameraManipulator;

	// OpenGL stuff

	// Variables needed for the shaders
	GLuint m_programID = 0; // program of the shaders

	// Initialization, and deletion of the shaders
	void InitShaders();
	void CleanShaders();

	// Variables related to geometry

	void SetCommonUniforms();
	void DrawObject(OGLObject& obj, const glm::mat4& world);

	OGLObject m_cubeGPU = {};
	static glm::vec3 CalculateLegAnim(float time, float animOffset);

	// Initialization and deletion of geometry
	void InitGeometry();
	void CleanGeometry();

	// Texturing and its variables
	GLuint m_SamplerID = 0;

	GLuint m_woodTextureID = 0;

	GLuint m_shineTextureID = 0;

	bool m_ShowDebugCube = false;

	float m_DebugCubeSize = 10.0f;

	void InitTextures();
	void CleanTextures();

	// Robot movement state
	glm::vec3 m_RobotPosition = glm::vec3(0.0f, 17.525f, 0.0f); // initial position (same as torso)
	float m_RobotYaw = 0.0f; // current facing angle (radians)
	glm::vec3 m_TargetPoint = glm::vec3(0.0f);
	bool m_HasTarget = false;
	bool m_IsRotating = false;
	bool m_IsMoving = false;
	float m_RobotTargetYaw = 0.0f;

	// Animation
	float m_LegAnimTime = 0.0f;


	// GUI speeds
	float m_RotationSpeed = 1.0f; // radians/sec
	float m_MoveSpeed = 30.0f;    // units/sec

	// Laser light state
	bool m_LaserLightActive = false;
	glm::vec3 m_LaserLightPos = glm::vec3(0.0f);

	bool m_FollowRobotCamera = false;
	glm::vec3 m_PrevCamEye, m_PrevCamAt, m_PrevCamUp;
	bool m_HasPrevCamera = false;
	DebugView m_DebugView = DebugView::Default;

};
