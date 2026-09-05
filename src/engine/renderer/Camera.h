#pragma once 

#include <glm/glm.hpp>

class Camera {
public:
	// CAMERA STANDARD 3D VIEW FRUSTUM PROPERTIES	
	Camera(float fovDegrees, float aspectRatio, float nearClip, float farClip);
	~Camera() = default;


	// MOVEMENT GETTERS 
	void SetPosition(const glm::vec3& position) {
		m_Position = position; RecalculateViewMatrix(); 
	}
	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3& GetFrontVector() const { return m_CameraFront; }
	const glm::vec3& GetRightVector() const { return m_CameraRight; }

	// MOUSE LOOK INPUT PROCESSING HOOK
	void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

	// MATRIX GETTERS PASSED ONTO SHADER UNIFORMS LATER
	const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
	const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

private:
	void RecalculateViewMatrix();

private:
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ViewProjectionMatrix;

	// CAMERA VECTORS 
	glm::vec3 m_Position = { 0.0f, 0.0f, 3.0f }; // Start back on Z-axis so we look at the square
	glm::vec3 m_CameraFront = { 0.0f, 0.0f, -1.0f }; // Looking down negative Z
	glm::vec3 m_CameraUp = { 0.0f, 1.0f, 0.0f };
	glm::vec3 m_CameraRight = { 1.0f, 0.0f, 0.0f };
	glm::vec3 m_WorldUp = { 0.0f, 1.0f, 0.0f };

	// EULER ANGLES ROTATION METRICES
	float m_Yaw = -90.0f; // Faced straight forward
	float m_Pitch = 0.0f;  // Level horizon look
	float m_MouseSensitivity = 0.1f;
};