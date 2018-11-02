#pragma once

//include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


using namespace glm;

 class WKRB_Camera
{
public:
	WKRB_Camera();
	~WKRB_Camera();

	
	mat4 getViewMatrix() {
		return ViewMatrix;
	}

	mat4 getProjectionMatrix() {
		return ProjectionMatrix;
	}

	vec3 getCameraPosition()
	{
		return position;
	}



	void computeMatricesFromInputs(float deltaTime, GLFWwindow * window);

	void LookAt(vec3 object,vec3 up = vec3(0, 1, 0));

private:

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;


	//Camera Position
	vec3 position = vec3(0, 1, 5);

	// Initial horizontal angle : toward -Z
	float horizontalAngle = 3.14f;
	// Initial vertical angle : none
	float verticalAngle = 0.0f;
	// Initial Field of View
	float initialFoV = 45.0f;

	float speed = 3.0f; // 3 units / second
	float mouseSpeed = 0.005f;


};
