#include "renderer/Camera.h"
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(float fovDegrees, float aspectRatio, float nearClip, float farClip) {
    // GENERATE STANDARD 3D PERSPECTIVE DEPTH SPACE MAPPING MATRIX
    m_ProjectionMatrix = glm::perspective(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);
    RecalculateViewMatrix();
}
void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= m_MouseSensitivity;
    yOffset *= m_MouseSensitivity;

    m_Yaw += xOffset;
    m_Pitch += yOffset;

    // MAKE SURE PLAYER CANT FLIP UPSIDE DOWN
    if (constrainPitch) {
        if (m_Pitch > 89.0f)  m_Pitch = 89.0f;
        if (m_Pitch < -89.0f) m_Pitch = -89.0f;
    }

    // RECALCULATE FRONT , RIGHT AND UP VECTORS BASED ON NEW ANGLES 
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_CameraFront = glm::normalize(front);

    // RECALCULATE COORDINATE AXIS SPACES 
    m_CameraRight = glm::normalize(glm::cross(m_CameraFront, m_WorldUp));
    m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_CameraFront));

    RecalculateViewMatrix();
}
void Camera::RecalculateViewMatrix() {
    // LOOKAT MATH EYE POINT, LOOK CENTER POINT , FRAME UP ALIGNMENTS NATIVELY
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_CameraFront, m_CameraUp);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
   
}