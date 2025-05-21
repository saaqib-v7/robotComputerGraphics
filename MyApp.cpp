#include "MyApp.h"
#include "ObjParser.h"
#include "SDL_GLDebugMessageCallback.h"

#include <imgui.h>

CMyApp::CMyApp()
{
}

CMyApp::~CMyApp()
{
}

void CMyApp::SetupDebugCallback()
{
	// if the program is running in the debug context, allow and setup the debug callback function
	GLint context_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_FALSE);
		glDebugMessageCallback(SDL_GLDebugMessageCallback, nullptr);
	}
}

void CMyApp::InitShaders()
{
	m_programID = glCreateProgram();
	AttachShader(m_programID, GL_VERTEX_SHADER, "Shaders/Vert_PosNormTex.vert");
	AttachShader(m_programID, GL_FRAGMENT_SHADER, "Shaders/Frag_ZH.frag");
	LinkProgram(m_programID);
}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
}

MeshObject<Vertex> createCube()
{
	MeshObject<Vertex> mesh;

	mesh.vertexArray = {
		// Front face
		{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // Bottom-left
		{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // Bottom-right
		{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // Top-right
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // Top-left

		// Back face
		{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // Bottom-left
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},// Bottom-right
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // Top-right
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},  // Top-left

		// Left face
		{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},// Bottom-left
		{{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Bottom-right
		{{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // Top-right
		{{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // Top-left

		// Right face
		{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // Bottom-left
		{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Bottom-right
		{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // Top-right
		{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},   // Top-left

		// Top face
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // Bottom-left
		{{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // Bottom-right
		{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // Top-right
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // Top-left

		// Bottom face
		{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},// Bottom-left
		{{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // Bottom-right
		{{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},  // Top-right
		{{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // Top-left
	};

	mesh.indexArray = {// Front face
					   0, 1, 2, 2, 3, 0,
					   // Back face
					   4, 5, 6, 6, 7, 4,
					   // Left face
					   8, 9, 10, 10, 11, 8,
					   // Right face
					   12, 13, 14, 14, 15, 12,
					   // Top face
					   16, 17, 18, 18, 19, 16,
					   // Bottom face
					   20, 21, 22, 22, 23, 20 };

	return mesh;
}

MeshObject<Vertex> CMyApp::LoadMeshFromOBJ(const std::string& filePath)
{
	return ObjParser::parse(filePath);
}

void CMyApp::InitGeometry()
{
	const std::initializer_list<VertexAttributeDescriptor> vertexAttribList =
	{
		{0, offsetof(Vertex, position), 3, GL_FLOAT},
		{1, offsetof(Vertex, normal), 3, GL_FLOAT},
		{2, offsetof(Vertex, texcoord), 2, GL_FLOAT},
	};

	m_cubeGPU = CreateGLObjectFromMesh(createCube(), vertexAttribList);

	m_torso = CreateGLObjectFromMesh(LoadMeshFromOBJ("Assets/torso.obj"), vertexAttribList);
	m_head = CreateGLObjectFromMesh(LoadMeshFromOBJ("Assets/head.obj"), vertexAttribList);
	m_rightUpperArm = CreateGLObjectFromMesh(LoadMeshFromOBJ("Assets/right_upper_arm.obj"), vertexAttribList);
	m_rightLowerArm = CreateGLObjectFromMesh(LoadMeshFromOBJ("Assets/right_lower_arm.obj"), vertexAttribList);
	m_leftUpperArm = CreateGLObjectFromMesh(LoadMeshFromOBJ("Assets/left_upper_arm.obj"), vertexAttribList);
	m_leftLowerArm = CreateGLObjectFromMesh(LoadMeshFromOBJ("Assets/left_lower_arm.obj"), vertexAttribList);
	m_leftLeg = CreateGLObjectFromMesh(LoadMeshFromOBJ("Assets/leg.obj"), vertexAttribList);
	m_rightLeg = CreateGLObjectFromMesh(LoadMeshFromOBJ("Assets/leg.obj"), vertexAttribList);

}

void CMyApp::CleanGeometry()
{
	CleanOGLObject(m_cubeGPU);
}

void CMyApp::InitTextures()
{
	glCreateSamplers(1, &m_SamplerID);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ImageRGBA woodImage = ImageFromFile("Assets/Wood_Table_Texture.png");


	glCreateTextures(GL_TEXTURE_2D, 1, &m_woodTextureID);
	glTextureStorage2D(m_woodTextureID, NumberOfMIPLevels(woodImage), GL_RGBA8, woodImage.width, woodImage.height);
	glTextureSubImage2D(m_woodTextureID, 0, 0, 0, woodImage.width, woodImage.height, GL_RGBA, GL_UNSIGNED_BYTE, woodImage.data());

	glGenerateTextureMipmap(m_woodTextureID);

	// Load robot texture
	ImageRGBA robotImage = ImageFromFile("Assets/Robot_Texture.png");
	glCreateTextures(GL_TEXTURE_2D, 1, &m_robotTextureID);
	glTextureStorage2D(m_robotTextureID, NumberOfMIPLevels(robotImage), GL_RGBA8, robotImage.width, robotImage.height);
	glTextureSubImage2D(m_robotTextureID, 0, 0, 0, robotImage.width, robotImage.height, GL_RGBA, GL_UNSIGNED_BYTE, robotImage.data());
	glGenerateTextureMipmap(m_robotTextureID);

	// Load shine texture for robot shininess
	ImageRGBA shineImage = ImageFromFile("Assets/shine.png");
	glCreateTextures(GL_TEXTURE_2D, 1, &m_shineTextureID);
	glTextureStorage2D(m_shineTextureID, NumberOfMIPLevels(shineImage), GL_RGBA8, shineImage.width, shineImage.height);
	glTextureSubImage2D(m_shineTextureID, 0, 0, 0, shineImage.width, shineImage.height, GL_RGBA, GL_UNSIGNED_BYTE, shineImage.data());
	glGenerateTextureMipmap(m_shineTextureID);

}

void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_woodTextureID);

	glDeleteSamplers( 1, &m_SamplerID );

	glDeleteTextures(1, &m_shineTextureID);

}

bool CMyApp::Init()
{
	SetupDebugCallback();

	// set the screen clearing color to blue-ish
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	InitShaders();
	InitGeometry();
	InitTextures();

	// other initializations

	glEnable(GL_CULL_FACE); // enable the culling of backwards facing polygons (from the point of view of the camera)
	glCullFace(GL_BACK);    // GL_BACK: backwards facing polygons, GL_FRONT: forwards facing polygons

	glEnable(GL_DEPTH_TEST); // enabling depth testing (obstruction)

	// camera
	m_camera.SetView(
		glm::vec3(0.0f, 60.0f, 150.0f),  // camera position (eye)
		glm::vec3(0.0f, 0.0f, 0.0f),     // look at point (at)
		glm::vec3(0.0f, 1.0f, 0.0f));    // up vector (up)

	m_cameraManipulator.SetCamera(&m_camera);

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
	CleanTextures();
}

Ray CMyApp::CalculatePixelRay(glm::vec2 pixel) const
{
	// Calculating NDC coordinates
	glm::vec3 pickedNDC = glm::vec3(
		2.0f * (pixel.x + 0.5f) / m_windowSize.x - 1.0f,
		1.0f - 2.0f * (pixel.y + 0.5f) / m_windowSize.y, 0.0f);

	// Calculating the world coordinates with the help of the inverse ViewProj matrix
	glm::vec4 pickedWorld = glm::inverse(m_camera.GetViewProj()) * glm::vec4(pickedNDC, 1.0f);
	pickedWorld /= pickedWorld.w; // homogeneous division
	Ray ray;

	// Raycasting starting position is the camera's position
	ray.origin = m_camera.GetEye();
	// Direction is from the camera's position towards the clicked point's world coordinates
	// ATTENTION: NOT normalized vector!
	ray.direction = glm::vec3(pickedWorld) - ray.origin;
	return ray;
}

static bool HitPlane(const Ray& ray, const glm::vec3& planeQ, const glm::vec3& planeI, const glm::vec3& planeJ, Intersection& result)
{
	// palneQ + u * planeI + v * planeJ
	glm::mat3 A(-ray.direction, planeI, planeJ);
	glm::vec3 B = ray.origin - planeQ;

	if (fabsf(glm::determinant(A)) < 1e-6) return false;
	glm::vec3 X = glm::inverse(A) * B;

	if (X.x < 0.0) {
		return false;
	}
	result.t = X.x;
	result.uv.x = X.y;
	result.uv.y = X.z;

	return true;
}

void CMyApp::Update(const SUpdateInfo& updateInfo)
{
	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;
	
	if (m_IsPicking) {
		Ray ray = CalculatePixelRay(glm::vec2(m_PickedPixel.x, m_PickedPixel.y));
		glm::vec3 planeQ(0, 0, 0), planeI(1, 0, 0), planeJ(0, 0, 1);
		Intersection result;
		if (HitPlane(ray, planeQ, planeI, planeJ, result)) {
			m_TargetPoint = ray.origin + result.t * ray.direction;
			// Tabletop bounds (centered at 0,0,0)
			float halfX = CMyApp::TABLE_SIZE.x * 0.5f;
			float halfZ = CMyApp::TABLE_SIZE.z * 0.5f;

			// Clamp to tabletop bounds
			m_TargetPoint.x = glm::clamp(m_TargetPoint.x, -halfX, halfX);
			m_TargetPoint.z = glm::clamp(m_TargetPoint.z, -halfZ, halfZ);
			m_TargetPoint.y = TORSO_POS.y;
			m_HasTarget = true;
			m_IsRotating = true;
			m_IsMoving = false;
			glm::vec3 toTarget = glm::vec3(m_TargetPoint.x, 0, m_TargetPoint.z) - glm::vec3(m_RobotPosition.x, 0, m_RobotPosition.z);
			m_RobotTargetYaw = atan2(toTarget.x, toTarget.z);
		}
		m_IsPicking = false;
	}


	// Only animate legs if moving or rotating
	bool robotActive = m_IsRotating || m_IsMoving;
	if (robotActive)
		m_LegAnimTime += updateInfo.DeltaTimeInSec;

	// Handle rotation
	if (m_HasTarget && m_IsRotating) {
		float angleDiff = m_RobotTargetYaw - m_RobotYaw;
		// Normalize to [-pi, pi]
		angleDiff = atan2(sin(angleDiff), cos(angleDiff));
		float maxStep = m_RotationSpeed * updateInfo.DeltaTimeInSec;
		if (fabs(angleDiff) <= maxStep) {
			m_RobotYaw = m_RobotTargetYaw;
			m_IsRotating = false;
			m_IsMoving = true;
		}
		else {
			m_RobotYaw += (angleDiff > 0 ? 1 : -1) * maxStep;
		}
	}

	// Handle movement
	if (m_HasTarget && m_IsMoving && !m_IsRotating) {
		glm::vec3 toTarget = m_TargetPoint - m_RobotPosition;
		toTarget.y = 0; // move in XZ only
		float dist = glm::length(toTarget);
		float maxStep = m_MoveSpeed * updateInfo.DeltaTimeInSec;
		if (dist <= maxStep) {
			m_RobotPosition = m_TargetPoint;
			m_IsMoving = false;
			m_HasTarget = false;
		}
		else {
			m_RobotPosition += glm::normalize(toTarget) * maxStep;
		}
	}



	m_cameraManipulator.Update(updateInfo.DeltaTimeInSec);
	if (!m_FollowRobotCamera)
		m_cameraManipulator.Update(updateInfo.DeltaTimeInSec);

	if (m_FollowRobotCamera)
	{
		if (!m_HasPrevCamera) {
			m_PrevCamEye = m_camera.GetEye();
			m_PrevCamAt = m_camera.GetAt();
			m_PrevCamUp = m_camera.GetWorldUp();
			m_HasPrevCamera = true;
		}

		// --- Head world transform ---
		// 1. Robot world
		glm::mat4 robotWorld = glm::translate(glm::mat4(1.0f), m_RobotPosition)
			* glm::rotate(glm::mat4(1.0f), m_RobotYaw, glm::vec3(0, 1, 0));
		// 2. Head joint offset (from torso to neck joint)
		glm::vec3 headJointOffset(0.0f, 5.928f, -0.822f);
		glm::mat4 headJointMat = glm::translate(glm::mat4(1.0f), headJointOffset);
		// 3. Head rotation (pitch, then yaw)
		glm::mat4 headRot = glm::rotate(glm::mat4(1.0f), m_headAngleX, glm::vec3(1, 0, 0));
		headRot = glm::rotate(headRot, m_headAngleY, glm::vec3(0, 1, 0));
		// 4. Head local offset (to center of head mesh, if needed)
		// If your head mesh is centered at the joint, use (0,0,0)
		glm::vec3 headLocalOffset(0, 0, 0);

		glm::mat4 headWorldMat = robotWorld * headJointMat * headRot * glm::translate(glm::mat4(1.0f), headLocalOffset);

		// Head's world position
		glm::vec3 headWorld = glm::vec3(headWorldMat * glm::vec4(0, 0, 0, 1));
		// Head's forward and up directions in world space
		glm::vec3 headForward = glm::normalize(glm::vec3(headWorldMat * glm::vec4(0, 0, 1, 0)));
		glm::vec3 headUp = glm::normalize(glm::vec3(headWorldMat * glm::vec4(0, 1, 0, 0)));

		// Camera position: behind and above the head
		glm::vec3 camPos = headWorld - headForward * 18.0f + headUp * 6.0f; // tweak 18 and 6 as needed

		// Look direction: forward from head
		glm::vec3 lookAt = headWorld + headForward * 10.0f;

		// Up vector: use head's up
		glm::vec3 camUp = headUp;

		m_camera.SetView(camPos, lookAt, camUp);
	}
	else if (m_HasPrevCamera)
	{
		m_camera.SetView(m_PrevCamEye, m_PrevCamAt, m_PrevCamUp);
		m_HasPrevCamera = false;
	}


	// --- Laser simulation ---
	{
		glm::vec3 headLocal(0, 5, 3.5f);
		glm::mat4 robotWorld = glm::translate(glm::mat4(1.0f), m_RobotPosition) * glm::rotate(glm::mat4(1.0f), m_RobotYaw, glm::vec3(0, 1, 0));
		glm::vec3 headWorld = glm::vec3(robotWorld * glm::vec4(headLocal, 1.0f));

		// Compose head rotation: first pitch (X), then yaw (Y)
		glm::mat4 headRot = glm::rotate(glm::mat4(1.0f), m_headAngleY, glm::vec3(0, 1, 0));
		headRot = glm::rotate(headRot, m_headAngleX, glm::vec3(1, 0, 0));

		glm::vec3 localForward(0, 0, 1);
		glm::vec3 dirWorld = glm::vec3(robotWorld * headRot * glm::vec4(localForward, 0.0f));

		// Intersect with tabletop plane y=0
		if (fabs(dirWorld.y) > 1e-6f) {
			float t = -headWorld.y / dirWorld.y;
			if (t > 0) {
				glm::vec3 hitPoint = headWorld + t * dirWorld;
				float halfX = CMyApp::TABLE_SIZE.x * 0.5f;
				float halfZ = CMyApp::TABLE_SIZE.z * 0.5f;
				if (hitPoint.x >= -halfX && hitPoint.x <= halfX && hitPoint.z >= -halfZ && hitPoint.z <= halfZ) {
					m_LaserLightActive = true;
					m_LaserLightPos = hitPoint + glm::vec3(0, 1, 0);
				}
				else {
					m_LaserLightActive = false;
				}
			}
			else {
				m_LaserLightActive = false;
			}
		}
		else {
			m_LaserLightActive = false;
		}
	}


}

void CMyApp::SetCommonUniforms()
{
	// - Uniform parameters

	// - view and projection matrices
	glUniformMatrix4fv( ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));
	glUniform3fv(ul("cameraPosition"), 1, glm::value_ptr(m_camera.GetEye()));
	glUniform1i(ul("laserLightActive"), m_LaserLightActive ? 1 : 0);
	glUniform3fv(ul("laserLightPos"), 1, glm::value_ptr(m_LaserLightPos));
	glUniform1i(ul("debugView"), static_cast<int>(m_DebugView));


	// - Setting up the light sources
	//glUniform3fv( ul("cameraPosition"), 1, glm::value_ptr(m_camera.GetEye()));
	//glUniform4fv( ul("lightPosition"), 1, glm::value_ptr(m_lightPosition));
	//glUniform3fv( ul("La"), 1, glm::value_ptr(m_La));
	//glUniform3fv( ul("Ld"), 1, glm::value_ptr(m_Ld));
	//glUniform3fv( ul("Ls"), 1, glm::value_ptr(m_Ls));
	//glUniform1f( ul("lightConstantAttenuation"), m_lightConstantAttenuation);
	//glUniform1f( ul("lightLinearAttenuation"), m_lightLinearAttenuation);
	//glUniform1f( ul("lightQuadraticAttenuation"), m_lightQuadraticAttenuation);
}

void CMyApp::DrawObject(OGLObject& obj, const glm::mat4& world) {
	glUniformMatrix4fv( ul("world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv( ul("worldInvTransp"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(world))));
	glBindVertexArray(obj.vaoID);
	glDrawElements(GL_TRIANGLES, obj.count, GL_UNSIGNED_INT, nullptr);
}

glm::vec3 CMyApp::CalculateLegAnim( float time, float animOffset )
{
	float legAnim = -((time * (glm::two_pi<float>()) * LEG_SPEED) + animOffset);
	return glm::vec3(0.0, glm::clamp(sinf(legAnim), 0.0f, 1.0f), cosf(legAnim) * LEG_SPREAD);
}

void CMyApp::Render()
{
	// clear the framebuffer (GL_COLOR_BUFFER_BIT)
	// ... and the depth buffer (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	SetCommonUniforms();

	glUniform1i( ul("textureImage"), 0);

	glBindTextureUnit(0, m_woodTextureID);
	glBindSampler(0, m_SamplerID);

	// Tabletop dimensions
	const glm::vec3 tabletopSize = glm::vec3(200.0f, 10.0f, 200.0f);
	// Leg dimensions
	const glm::vec3 legSize = glm::vec3(20.0f, 120.0f, 20.0f);

	// Tabletop: center upper surface at (0,0,0)
	// Since the cube is centered at origin and unit sized, scale by tabletopSize,
	// then move down by half the tabletop height so the upper surface is at y=0.
	glm::mat4 tabletopModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -tabletopSize.y * 0.5f, 0.0f));
	tabletopModel = glm::scale(tabletopModel, tabletopSize);
	// Set isTabletop = 1 before drawing the tabletop
	glUniform1i(ul("isTabletop"), 1);
	DrawObject(m_cubeGPU, tabletopModel);

	// Leg positions (x, z), y will be calculated
	const float legOffsetX = 80.0f; // (tabletopSize.x/2 - legSize.x/2)
	const float legOffsetZ = 80.0f; // (tabletopSize.z/2 - legSize.z/2)
	const float legTopY = -tabletopSize.y; // tabletop bottom surface
	const float legCenterY = legTopY - legSize.y * 0.5f; // center of leg

	glm::vec3 legPositions[4] = {
		glm::vec3(legOffsetX, legCenterY,  legOffsetZ), // (+x, +z)
		glm::vec3(-legOffsetX, legCenterY,  legOffsetZ), // (-x, +z)
		glm::vec3(-legOffsetX, legCenterY, -legOffsetZ), // (-x, -z)
		glm::vec3(legOffsetX, legCenterY, -legOffsetZ)  // (+x, -z)
	};

	for (int i = 0; i < 4; ++i) {
		glm::mat4 legModel = glm::translate(glm::mat4(1.0f), legPositions[i]);
		legModel = glm::scale(legModel, legSize);
		// Set isTabletop = 0
		glUniform1i(ul("isTabletop"), 0);
		DrawObject(m_cubeGPU, legModel);
	}

	glBindTextureUnit(0, m_robotTextureID);
	glBindSampler(0, m_SamplerID);
	glBindTextureUnit(1, m_shineTextureID); // Bind shine texture to unit 1
	glUniform1i(ul("shineTexture"), 1);     // Tell the shader to use texture unit 1

	if (m_ShowDebugCube && m_HasTarget) {
		// When drawing the debug cube:
		glm::vec3 cubePos = m_TargetPoint;
		cubePos.y = 0.0f + m_DebugCubeSize * 0.5f; // tabletop top + half cube height
		glm::mat4 debugCubeModel = glm::translate(glm::mat4(1.0f), cubePos);
		debugCubeModel = glm::scale(debugCubeModel, glm::vec3(m_DebugCubeSize));
		glUniform1i(ul("isTabletop"), 0);
		DrawObject(m_cubeGPU, debugCubeModel);

	}
	// Torso (root)
	glm::mat4 torsoModel = glm::translate(glm::mat4(1.0f), m_RobotPosition);
	torsoModel = glm::rotate(torsoModel, m_RobotYaw, glm::vec3(0, 1, 0));
	DrawObject(m_torso, torsoModel);

	// Head
	glm::mat4 headModel = torsoModel;
	headModel = glm::translate(headModel, glm::vec3(0.0f, 5.928f, -0.822f));
	headModel = glm::rotate(headModel, m_headAngleX, glm::vec3(1, 0, 0));
	headModel = glm::rotate(headModel, m_headAngleY, glm::vec3(0, 1, 0));
	DrawObject(m_head, headModel);

	// Right Upper Arm
	glm::mat4 rUpperArmModel = torsoModel;
	rUpperArmModel = glm::translate(rUpperArmModel, glm::vec3(-5.402f, 2.966f, -0.878f));
	rUpperArmModel = glm::rotate(rUpperArmModel, m_rightUpperArmAngle, glm::vec3(1, 0, 0));
	DrawObject(m_rightUpperArm, rUpperArmModel);

	// Right Lower Arm
	glm::mat4 rLowerArmModel = rUpperArmModel;
	rLowerArmModel = glm::translate(rLowerArmModel, glm::vec3(-0.114f, -4.633f, -0.005f));
	rLowerArmModel = glm::rotate(rLowerArmModel, m_rightLowerArmAngle, glm::vec3(1, 0, 0));
	DrawObject(m_rightLowerArm, rLowerArmModel);

	// Left Upper Arm
	glm::mat4 lUpperArmModel = torsoModel;
	lUpperArmModel = glm::translate(lUpperArmModel, glm::vec3(5.402f, 2.966f, -0.878f));
	lUpperArmModel = glm::rotate(lUpperArmModel, m_leftUpperArmAngle, glm::vec3(1, 0, 0));
	DrawObject(m_leftUpperArm, lUpperArmModel);

	// Left Lower Arm
	glm::mat4 lLowerArmModel = lUpperArmModel;
	lLowerArmModel = glm::translate(lLowerArmModel, glm::vec3(0.114f, -4.633f, -0.005f));
	lLowerArmModel = glm::rotate(lLowerArmModel, m_leftLowerArmAngle, glm::vec3(1, 0, 0));
	DrawObject(m_leftLowerArm, lLowerArmModel);

	// Right Leg (animated)
	glm::vec3 rightLegAnim = CalculateLegAnim(m_LegAnimTime, glm::pi<float>());
	glm::mat4 rLegModel = torsoModel;
	rLegModel = glm::translate(rLegModel, glm::vec3(-2.867f, -4.883f, -0.771f) + rightLegAnim);
	DrawObject(m_rightLeg, rLegModel);

	// Left Leg (animated)
	glm::vec3 leftLegAnim = CalculateLegAnim(m_LegAnimTime, 0.0f);
	glm::mat4 lLegModel = torsoModel;
	lLegModel = glm::translate(lLegModel, glm::vec3(2.867f, -4.883f, -0.771f) + leftLegAnim);
	DrawObject(m_leftLeg, lLegModel);

	glBindTextureUnit(0, m_robotTextureID);
	glBindSampler(0, m_SamplerID);
	glBindTextureUnit(1, m_shineTextureID); // Bind shine texture to unit 1


	glBindTextureUnit(0, 0);
	glBindSampler(0, 0);

	glBindVertexArray(0);
	// turn the shader off
	glUseProgram(0);
}

void CMyApp::RenderGUI()
{
	//ImGui::ShowDemoWindow();
	ImGui::SliderAngle("Head X", &m_headAngleX, glm::radians(-180.0f), glm::radians(180.0f));
	ImGui::SliderAngle("Head Y", &m_headAngleY, glm::radians(-360.0f), glm::radians(360.0f));
	ImGui::SliderAngle("Right Upper Arm", &m_rightUpperArmAngle, glm::radians(-270.0f), glm::radians(270.0f));
	ImGui::SliderAngle("Right Lower Arm", &m_rightLowerArmAngle, glm::radians(-90.0f), glm::radians(270.0f));
	ImGui::SliderAngle("Left Upper Arm", &m_leftUpperArmAngle, glm::radians(-270.0f), glm::radians(270.0f));
	ImGui::SliderAngle("Left Lower Arm", &m_leftLowerArmAngle, glm::radians(-90.0f), glm::radians(270.0f));

	ImGui::Checkbox("Show Debug Cube", &m_ShowDebugCube);
	ImGui::SliderFloat("Debug Cube Size", &m_DebugCubeSize, 5.0f, 50.0f);
	ImGui::SliderFloat("Rotation Speed", &m_RotationSpeed, 0.1f, 5.0f);
	ImGui::SliderFloat("Move Speed", &m_MoveSpeed, 1.0f, 100.0f);

	ImGui::Checkbox("Follow Robot Camera", &m_FollowRobotCamera);

	const char* debugViewItems[] = {
	"Default (Shading + Texture)",
	"Normal as Color",
	"Shading (No Texture)",
	"Diffuse Texture Only",
	"Shininess Texture Only"
	};
	int debugViewIdx = static_cast<int>(m_DebugView);
	if (ImGui::Combo("Debug View", &debugViewIdx, debugViewItems, IM_ARRAYSIZE(debugViewItems))) {
		m_DebugView = static_cast<DebugView>(debugViewIdx);
	}

}



// https://wiki.libsdl.org/SDL2/SDL_KeyboardEvent
// https://wiki.libsdl.org/SDL2/SDL_Keysym
// https://wiki.libsdl.org/SDL2/SDL_Keycode
// https://wiki.libsdl.org/SDL2/SDL_Keymod

void CMyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{
	if (key.repeat == 0) // It was pressed for the first time
	{
		if (key.keysym.sym == SDLK_F5 && key.keysym.mod & KMOD_CTRL)
		{
			CleanShaders();
			InitShaders();
		}
		if (key.keysym.sym == SDLK_F1)
		{
			GLint polygonModeFrontAndBack[2] = {};
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
			glGetIntegerv( GL_POLYGON_MODE, polygonModeFrontAndBack ); // Query the current polygon mode! It gives separate modes for front and back.
			GLenum polygonMode = ( polygonModeFrontAndBack[ 0 ] != GL_FILL ? GL_FILL : GL_LINE ); // Let's toggle between FILL and LINE!
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
			glPolygonMode( GL_FRONT_AND_BACK, polygonMode ); // Let's set the new one!
		}

		if (key.keysym.sym == SDLK_LCTRL || key.keysym.sym == SDLK_RCTRL)
		{
			m_IsCtrlDown = true;
		}
	}
	m_cameraManipulator.KeyboardDown(key);
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
	m_cameraManipulator.KeyboardUp(key);
	if (key.keysym.sym == SDLK_LCTRL || key.keysym.sym == SDLK_RCTRL)
	{
		m_IsCtrlDown = false;
	}
}

// https://wiki.libsdl.org/SDL2/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	m_cameraManipulator.MouseMove(mouse);
}

// https://wiki.libsdl.org/SDL2/SDL_MouseButtonEvent

void CMyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
	if ( m_IsCtrlDown )
	{
		m_IsPicking = true;
	}
	m_PickedPixel = { mouse.x, mouse.y };
}

void CMyApp::MouseUp(const SDL_MouseButtonEvent& mouse)
{
}

// https://wiki.libsdl.org/SDL2/SDL_MouseWheelEvent

void CMyApp::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
	m_cameraManipulator.MouseWheel(wheel);
}

// two new parameters for the resized window's width (_w) and height (_h)
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	m_windowSize = glm::uvec2(_w, _h);
	m_camera.SetAspect(static_cast<float>(_w) / _h);
}

// For handling other, more exotic events that haven't been handled yet
// https://wiki.libsdl.org/SDL2/SDL_Event

void CMyApp::OtherEvent(const SDL_Event& ev)
{

}
