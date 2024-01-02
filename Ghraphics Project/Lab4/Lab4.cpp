// THIS LAB STARTS WITH CODE FROM TUTORIAL 1 FROM OPENGL-TUTORIAL.ORG CODE

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>	// for value_ptr used in lights array uniform

using namespace glm;

#include "common/model.h"
#include "common/shader.h"
#include "common/GLError.h"
#include "common/skybox.h"
//#include "common/controls.hpp"

#include "SOIL2.h"


#include "WKRB_Camera.h"

void GLAPIENTRY
MessageCallback(GLenum source,
				 GLenum type,
				 GLuint id,
				 GLenum severity,
				 GLsizei length,
				 const GLchar* message,
				 const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}


void Input()
{



}


void Update(float deltaTime)
{



}

int main( void )
{


#pragma region INIT

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int window_height = 1080, window_Width = 1920;

	vec2 Resolution;

	Resolution.x = window_Width;
	Resolution.y = window_height;

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(Resolution.x, Resolution.y, "Wesley Beck Assesment", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	check_gl_error();

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	check_gl_error();

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwPollEvents();
	glfwSetCursorPos(window, Resolution.x / 2, Resolution.y / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	check_gl_error();

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	check_gl_error();

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	check_gl_error();

#pragma endregion


#pragma region Shader Init
	// shader for mat (which has ambient occlusion texture in the ambient channel that is multiplied by diffuse colour texture)
	// and specularity colour map
	Shader StandardShaderWithAO("StandardShadingWithAOSpecmaps.vertexshader", "StandardShadingWithAOSpecmaps.fragmentshader");
	check_gl_error();

	// shader for our standard models with 1 diffuse texture
	Shader StandardShader("StandardShading.vertexshader", "StandardShading.fragmentshader");
	check_gl_error();

	// used to draw a full white light sphere!
	Shader lightShader("LightShading.vertexshader", "LightShading.fragmentshader");
	check_gl_error();

	// used by the skybox, basically uses cubemap texture to draw the correct albedo (diffuse colour)
	Shader skyboxShader("skybox.vertexshader", "skybox.fragmentshader");
	check_gl_error();

	//PBR
	Shader PBR("PBR.vertexshader", "PBR.fragmentshader");

	// reflection shader version
	Shader StandardShaderWithReflect("StandardShadingWithReflect.vertexshader", "StandardShadingWithReflect.fragmentshader");
	check_gl_error();

	// refraction shader version
	Shader StandardShaderWithRefract("StandardShadingWithRefract.vertexshader", "StandardShadingWithRefract.fragmentshader");
	check_gl_error();

#pragma endregion

	//Skybox sky("xp.jpg", "xn.jpg", "yp.jpg", "yn.jpg", "zp.jpg", "zn.jpg");
	Skybox sky("../3dcontent/skybox/tropicalsunnyday/","TropicalSunnyDayRight.png", "TropicalSunnyDayLeft.png", "TropicalSunnyDayUp.png", "TropicalSunnyDayDown.png", "TropicalSunnyDayBack.png", "TropicalSunnyDayFront.png");

	Model togModel("../3dcontent/models/TOG/tog_II.obj");
	//matModel.SetPosition(glm::vec3(-1.0, -0.02, 0));
	//../3dcontent/models/panzer-ausf-b-fbx/german-ausf-b.fbx
	togModel.SetPosition(glm::vec3(-3, -0.02, 0));
	togModel.SetScale(vec3(0.5f, 0.5f, 0.5f));

	// left hand guy with no spec+AO for comparison
	//"../3dcontent/models/panzer-ausf-b-obj/german-ausf-b.obj"
	Model matModel2("../3dcontent/models/WoT_IS7/IS7.obj");
	matModel2.SetPosition(glm::vec3(5, 1, -4));
	matModel2.SetScale(vec3(0.5f, 0.5f, 0.5f));




	Model asusfB("../3dcontent/models/panzer-ausf-b-obj/german-ausf-b.obj");
	asusfB.SetPosition(glm::vec3(0, -0.02, 0));
	asusfB.SetScale(vec3(0.5f, 0.5f, 0.5f));
	//model 4 Turret
	//model 0 track Wheels

	Model asusfB2("../3dcontent/models/panzer-ausf-b-obj/german-ausf-b.obj");
	asusfB2.SetScale(vec3(0.5f, 0.5f, 0.5f));

	Model sphere("../3dcontent/models/sphere/sphere.obj");
	sphere.SetPosition(glm::vec3(3, 1, 0));

	// the "ground" mesh, in this case a cylinder with a slight tapered base
	Model baseModel("../3dcontent/models/MyTerrain/MyTerrain.obj");
	baseModel.SetScale(vec3(40.0f,40.0f,40.0f));
	baseModel.SetPosition(vec3(0.0f,-7.0f, 0.0f));
	// basically a sphere, but with a shader to make it entirely white
	//Model lightModel("../3dcontent/models/sphere/sphere.obj");
	//// naughty, should check if getmesh returns null!!!
	//lightModel.GetMesh(0)->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));
	check_gl_error();

	Model lights[] = { Model("../3dcontent/models/sphere/sphere.obj"),
		Model("../3dcontent/models/sphere/sphere.obj") ,
		Model("../3dcontent/models/sphere/sphere.obj") ,
		Model("../3dcontent/models/sphere/sphere.obj") };

	for (auto &model : lights)
	{
		model.GetMesh(0)->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));
	}

	WKRB_Camera mainCamera;


	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 7, 3), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	check_gl_error();


//	glm::vec3 lightColour(1, 1, 1);

#pragma region Lighting
	glm::vec4 light(0, 0, 0, 25);


	GLfloat ambiantIntensity = 0.4;

	//vec3 lightPosition = vec3(4.0f, 3.0f, 3.0f);
	vec3 lightColour = vec3(1, 1, 1);

	GLfloat specIndensity = 5.0;
	GLfloat specShinny = 5.0;
	GLfloat attenuationDist = 4;

#pragma endregion


	// how much time has accumulaed?
	static float timeElapsed = 0.0f;

	double currentTime = glfwGetTime();
	double lastTime = currentTime;

	bool IsAnimatingTime = true;

	bool bShowNormalMap = false;

#pragma region Post-Processing INIT
	Shader screenShader("screenShader.vertexshader", "screenShader.fragmentshader");

	// framebuffer configuration
	// -------------------------
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// create a color attachment texture
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Resolution.x, Resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Resolution.x, Resolution.y); // use a single renderbuffer object for both a depth AND stencil buffer.

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it


																								  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
							 // positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



	bool postP = true;

	bool grayScale = false;
	bool Inverted = false;
	bool sepia = false;
	bool spiral = false;
	bool gammaCorrection = false;

	GLfloat gammaValue = 2.2f;
	GLfloat sAmmount = 2.0f;

#pragma endregion



	// lights
	// ------





	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f,  10.0f),
		glm::vec3( 5.0f,	2.0f,	-7.0f),
		glm::vec3(	4.0f, 4.0f,   -9.0f),
		glm::vec3(10.0f, 2.0f, -9.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(100.0f, 100.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	unsigned int matalic = SOIL_load_OGL_texture("../3dcontent/models/sphere/octostoneMetallic.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_TEXTURE_REPEATS); //TextureFromFile(str.C_Str(), directory);
	unsigned int Albedo = SOIL_load_OGL_texture("../3dcontent/models/sphere/octostoneAlbedo.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_TEXTURE_REPEATS); //TextureFromFile(str.C_Str(), directory);
	unsigned int AO = SOIL_load_OGL_texture("../3dcontent/models/sphere/octostoneAmbient_Occlusionc.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_TEXTURE_REPEATS); //TextureFromFile(str.C_Str(), directory);
	unsigned int normal = SOIL_load_OGL_texture("../3dcontent/models/sphere/octostoneNormalc.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_TEXTURE_REPEATS); //TextureFromFile(str.C_Str(), directory);
	unsigned int roughness = SOIL_load_OGL_texture("../3dcontent/models/sphere/octostoneRoughness2.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_TEXTURE_REPEATS); //TextureFromFile(str.C_Str(), directory);

	unsigned int terrainAlbedo = SOIL_load_OGL_texture("../3dcontent/models/sphere/NewProjectBitmapOutput513.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_TEXTURE_REPEATS); //TextureFromFile(str.C_Str(), directory);


	

	float reflectionamount = 0.3f;
	float indexofrefraction = 0.5f;


	mainCamera.getCameraPosition();

	float tankSpeed = 2.0f;
	vec3 movingTankPos = vec3(3, 1.02, 0);
	asusfB2.SetPosition(movingTankPos);



	bool blinnPhong = 0;



	bool tankForward = true;
	do{

		if (postP == true)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);			
		}

		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		// Compute time difference between current and last frame
		currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);

		if(IsAnimatingTime)
			timeElapsed += deltaTime;
		// update light position
		float lightdistance = 4.0f;
		light.y = lightdistance * cosf(timeElapsed);
		light.z = lightdistance * sinf(timeElapsed);

		glm::vec3 lightpos(light);
		lights[0].GetMesh(0)->SetPosition(lightpos);

		
		lights[1].GetMesh(0)->SetPosition(lightPositions[1]);
		lights[2].GetMesh(0)->SetPosition(lightPositions[2]);
		lights[3].GetMesh(0)->SetPosition(lightPositions[3]);



		//check_gl_error();
		mainCamera.computeMatricesFromInputs(deltaTime,window);

		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		{
			mainCamera.LookAt(lightpos);
		}

		Projection = mainCamera.getProjectionMatrix();
		View = mainCamera.getViewMatrix();


		glm::vec3 cameraPosition = mainCamera.getCameraPosition();


		// skybox code causes skyboxShader to be bound.. might want to leave this as another step?
		// skybox always comes first, so other models can overwrite colour buffer
		sky.Draw(View,Projection, skyboxShader);

		// draw the "fancy" mat first, he's got AO!
		StandardShaderWithAO.use();
		StandardShaderWithAO.setFloat("ambientlevel", ambiantIntensity);
		StandardShaderWithAO.setVec3("lightColour", lightColour);
		StandardShaderWithAO.setVec4("light", light);
		StandardShaderWithAO.setFloat("specularIntensity",specIndensity);
		StandardShaderWithAO.setBool("blinnPhong",blinnPhong);

	/*	glActiveTexture(GL_TEXTURE11);
		glBindTexture(GL_TEXTURE_2D, terrainAlbedo);
		glUniform1i(glGetUniformLocation(StandardShaderWithAO.ID, "texture_diffuse1"), terrainAlbedo);
		*/

		baseModel.Render(View, Projection, StandardShaderWithAO);

		//glActiveTexture(GL_TEXTURE11); // activate the texture unit									
		//glBindTexture(GL_TEXTURE_2D, 0);

		togModel.Render(View, Projection, StandardShaderWithAO);
	

		glActiveTexture(GL_TEXTURE20);
		glBindTexture(GL_TEXTURE_CUBE_MAP, sky.cubemaptextureID);

		StandardShaderWithReflect.use();
		StandardShaderWithReflect.setInt("texcube", 20);
		StandardShaderWithReflect.setFloat("reflectionamount", reflectionamount);
		StandardShaderWithReflect.setFloat("ambientlevel", ambiantIntensity);
		StandardShaderWithReflect.setVec3("lightColour", lightColour);
		StandardShaderWithReflect.setVec4("light", light);
		StandardShaderWithReflect.setVec3("cameraPosition", cameraPosition);

		asusfB.Render(View, Projection, StandardShaderWithReflect);

		// draw the diffuse mat, he's got AO!
		StandardShaderWithRefract.use();
		StandardShaderWithRefract.setInt("texcube", 20);
		StandardShaderWithRefract.setFloat("reflectionamount", reflectionamount);
		StandardShaderWithRefract.setFloat("ambientlevel", ambiantIntensity);
		StandardShaderWithRefract.setVec3("lightColour", lightColour);
		StandardShaderWithRefract.setVec4("light", light);
		StandardShaderWithRefract.setVec3("cameraPosition", cameraPosition);
		StandardShaderWithRefract.setFloat("indexofrefraction", indexofrefraction);

		asusfB2.Render(View, Projection, StandardShaderWithRefract);


		glActiveTexture(GL_TEXTURE20); // activate the texture unit									
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // bind texture unit back to 0

		// now bind and use our "Standard phong lighting shader for both "normal" mat and the base mesh
		//StandardShader.use();
		// global ambient light level for everything rendered with this shader
		//StandardShader.setFloat("ambientlevel", 0.3f);
		//StandardShader.setVec3("lightColour", lightColour);
		//StandardShader.setVec4("light", light);
		//StandardShader.setFloat("specularIntensity", specIndensity);
		//StandardShader.setFloat("specShininess",specShinny);




		PBR.use();
		
		PBR.setFloat("ambientlevel", ambiantIntensity);



		/*glActiveTexture(GL_TEXTURE10);
		glUniform1i(glGetUniformLocation(PBR.ID, "metallicMap"), matalic);
		glBindTexture(GL_TEXTURE_2D, matalic);*/



		lightPositions[0] = lightpos;

		/*PBR.setVec3("albedo", 0.5f, 0.0f, 0.0f);
		PBR.setFloat("ao", 1.0f);
		PBR.setFloat("metallic", 0.8f);
		PBR.setFloat("roughness", 0.1);*/
		//PBR.setInt("nLights", 4);
		//PBR.setVec3("lightPositions", vec3(lightPositions[0]));
		//PBR.setVec3("lightColors", vec3(lightColors[0]));
		
		for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
		{
			PBR.setVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
			PBR.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
	
		}

		//for (unsigned int i = 0; i < lightPositions->length()+1; ++i)
		//{
		//	PBR.setVec3("pointLights["+ std::to_string(i) +"].position", lightPositions[i]);
		//	PBR.setVec3("pointLights[" + std::to_string(i) + "].colour", lightColors[i]);
		//	//PBR.setVec3("lightPositions["+ std::to_string(i) +"].ambient", 0.05f, 0.05f, 0.05f);
		//	//PBR.setVec3("lightPositions["+ std::to_string(i) +"].diffuse", 0.8f, 0.8f, 0.8f);
		//	//PBR.setVec3("lightPositions["+ std::to_string(i) +"].specular", 1.0f, 1.0f, 1.0f);
		//	//PBR.setFloat("lightPositions["+ std::to_string(i) +"].constant", 1.0f);
		//	//PBR.setFloat("lightPositions["+ std::to_string(i) +"].linear", 0.09);
		//	//PBR.setFloat("lightPositions["+ std::to_string(i) +"].quadratic", 0.032);
		//}

		
		matModel2.Render(View, Projection, PBR);

		glActiveTexture(GL_TEXTURE11);
		glUniform1i(glGetUniformLocation(PBR.ID, "texture_diffuse1"), Albedo);
		glBindTexture(GL_TEXTURE_2D, Albedo);

		glActiveTexture(GL_TEXTURE12);
		glUniform1i(glGetUniformLocation(PBR.ID, "texture_ao1"), AO);
		glBindTexture(GL_TEXTURE_2D, AO);

		glActiveTexture(GL_TEXTURE13);
		glUniform1i(glGetUniformLocation(PBR.ID, "texture_specular1"), roughness);
		glBindTexture(GL_TEXTURE_2D, roughness);

		glActiveTexture(GL_TEXTURE14);
		glUniform1i(glGetUniformLocation(PBR.ID, "texture_height1"), normal);
		glBindTexture(GL_TEXTURE_2D, normal);

		sphere.Render(View,Projection,PBR);

		glActiveTexture(GL_TEXTURE10); // activate the texture unit									
		glBindTexture(GL_TEXTURE_2D, 0); // bind texture unit back to 0
		glActiveTexture(GL_TEXTURE11); // activate the texture unit									
		glBindTexture(GL_TEXTURE_2D, 0); // bind texture unit back to 0
		glActiveTexture(GL_TEXTURE12); // activate the texture unit									
		glBindTexture(GL_TEXTURE_2D, 0); // bind texture unit back to 0
		glActiveTexture(GL_TEXTURE13); // activate the texture unit									
		glBindTexture(GL_TEXTURE_2D, 0); // bind texture unit back to 0
		glActiveTexture(GL_TEXTURE14); // activate the texture unit									
		glBindTexture(GL_TEXTURE_2D, 0); // bind texture unit back to 0


		// using a sphere for a light so we can vizualize the light position
		// it has a custom shader that basically emits solid white
		lightShader.use();
		for (auto &light : lights)
		{
			light.Render(View, Projection, lightShader);
		}
		

		if (postP == true)
		{

			// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
									  // clear all relevant buffers
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
			glClear(GL_COLOR_BUFFER_BIT);

			screenShader.use();

			screenShader.setBool("grayScale", grayScale);
			screenShader.setBool("inverted", Inverted);
			screenShader.setBool("sepia", sepia);
			screenShader.setBool("spiral", spiral);
			screenShader.setVec2("resolution", Resolution);
			screenShader.setFloat("sAmmount", sAmmount);
			screenShader.setFloat("gammaValue",gammaValue);
			screenShader.setBool("gamaCorrection",gammaCorrection);

			glBindVertexArray(quadVAO);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
			glDrawArrays(GL_TRIANGLES, 0, 6);

		}
		// Swap buffers
		glfwSwapBuffers(window);

		// For the next frame, the "last time" will be "now"
		lastTime = currentTime;

		glfwPollEvents();

		Input();


		// FIXME - this could be a lot better to control, think about how to improve it! e.g. orbit some position with light?
		// toggle the status of time animation
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			IsAnimatingTime = !IsAnimatingTime;
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
		{
			IsAnimatingTime = true;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
		{
			IsAnimatingTime = false;
		}


		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			postP = !postP;
		}

		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		{
			grayScale = !grayScale;
		}

		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		{
			Inverted = !Inverted;
		}

		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		{
			sepia = !sepia;
		}

		if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		{
			spiral = !spiral;
		}


		if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
		{
			gammaCorrection = !gammaCorrection;
		}

		if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
		{
			gammaValue += 0.1f *deltaTime;
		}


		if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
		{
			gammaValue -= 0.1f *deltaTime;
			if (gammaValue < 0)
				gammaValue = 0;
		}

		if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
		{
			sAmmount += 0.1f *deltaTime;
			
		}

		if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
		{
			sAmmount -= 0.1f *deltaTime;
		}




#
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			reflectionamount += 0.01f;
			if (reflectionamount > 1.0f) reflectionamount = 1.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			reflectionamount -= 0.01f;
			if (reflectionamount < 0.0f) reflectionamount = 0.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
		{
			indexofrefraction += 0.01f;
			//if (reflectionamount > 1.0f) reflectionamount = 1.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
		{
			indexofrefraction -= 0.01f;
			//if (reflectionamount < 0.0f) reflectionamount = 0.0f;
		}


		indexofrefraction = clamp(indexofrefraction, -1.0f, 1.0f);


		if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
		{
			tankForward = !tankForward;
		}

		if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS)
		{
			blinnPhong = true;
		}
		if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
		{
			blinnPhong = false;
		}







		std::cout << "reflection amount: " << reflectionamount << "  index of refraction: " << indexofrefraction << std::endl;

		std::cout << "Gamma Value" << gammaValue << std::endl;



		//Updates game state
		Update(deltaTime);

		//Rotate Turret
		quat rotation = asusfB.GetMesh(4)->GetRotation();
		rotation *= glm::angleAxis(glm::radians(30.f * deltaTime) , glm::vec3(0.f, 1.f, 0.f));;
		asusfB.GetMesh(4)->SetRotation(rotation);

		
		

		if (tankForward && movingTankPos.z < 5)
		{
			movingTankPos += (vec3(0, 0, 1)*tankSpeed)*deltaTime;
			asusfB2.SetPosition(movingTankPos);

		}
		else if(!tankForward && movingTankPos.z > 0)
		{
			movingTankPos += (vec3(0, 0, -1)*tankSpeed)*deltaTime;
			asusfB2.SetPosition(movingTankPos);
		}

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteFramebuffers(1, &framebuffer);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

