#include "Camera.h"

#include <math.h>
#include <iostream>

namespace tk
{
Camera::Camera(glm::dvec3 position, glm::dvec3 target)
	: m_position(position), m_direction(glm::normalize(position - target))
{
	// Gets the local right vector by using the direction vector and up vector,
	// and getting the "normal" vector of the plane created by these vectors (done with cross product)
	//m_yaw = glm::asin(initialDir.y);
	//m_pitch = glm::pow(glm::atan(initialDir.x, initialDir.z), 2);

	m_right = glm::normalize(glm::cross(glm::dvec3(0.0, 1.0, 0.0), m_direction));
	m_up = glm::normalize(glm::cross(m_right, m_direction));
	m_forward = glm::normalize(glm::cross(m_right, m_up));
}

void Camera::rotate(double pitch, double yaw)
{
	m_yaw += yaw;
	m_pitch -= pitch;

	if (m_pitch < -89.9999)
		m_pitch = -89.9999;

	if (m_pitch > 89.9999)
		m_pitch = 89.9999;

	if (m_yaw > 180.0)
		m_yaw -= 360.0;

	if (m_yaw < -180.0)
		m_yaw += 360.0;

	glm::dvec3 newRot;
	newRot.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
	newRot.y = sin(glm::radians(m_pitch));
	newRot.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));

	m_direction = glm::normalize(newRot);
	m_right = glm::normalize(glm::cross(m_direction, glm::dvec3(0.0, 1.0, 0.0)));
	m_up = glm::normalize(glm::cross(m_direction, m_right));
	m_forward = glm::normalize(glm::cross(m_right, glm::dvec3(0.0, 1.0, 0.0)));
}
} // namespace tk