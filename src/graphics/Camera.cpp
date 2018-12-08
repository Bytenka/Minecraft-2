#include "Camera.h"

#include <math.h>

#include "../utils/Exceptions.h"

namespace tk
{
Camera::Camera(glm::dvec3 position, glm::dvec3 target)
	: m_position(position) //, m_direction(glm::normalize(position - target))
{
	// Gets the local right vector by using the direction vector and up vector,
	// and getting the "normal" vector of the plane created by these vectors (done with cross product)

	// Something is up here. initialDir.z and .x should be inverted,
	// and "-asin" is incorrect. It works, but something is wrong
	// Adding PI or other offset is expected in some cases
	glm::dvec3 initialDir = glm::normalize(target - position);
	m_yaw = glm::degrees(atan2(initialDir.z, initialDir.x) + glm::pi<double>());
	m_pitch = glm::degrees(-asin(initialDir.y));
	//rotate(0, 0);
}

void Camera::rotate(double pitch, double yaw)
{
	m_pitch += pitch;
	m_yaw += yaw;

	if (m_pitch < -89.99999)
		m_pitch = -89.99999;

	if (m_pitch > 89.99999)
		m_pitch = 89.99999;

	if (m_yaw > 180.0)
		m_yaw -= 360.0;

	if (m_yaw < -180.0)
		m_yaw += 360.0;

	glm::dvec3 newRot;
	newRot.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
	newRot.y = sin(glm::radians(m_pitch));
	newRot.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));

	m_direction = glm::normalize(newRot);
	m_right = glm::normalize(glm::cross(glm::dvec3(0.0, 1.0, 0.0), m_direction));
	m_up = glm::normalize(glm::cross(m_direction, m_right));
	m_forward = glm::normalize(glm::cross(glm::dvec3(0.0, 1.0, 0.0), m_right));

	//LOG_TRACE("dir: {}, {}, {}", m_direction.x, m_direction.y, m_direction.z);
	//LOG_TRACE("rig: {}, {}, {}", m_right.x, m_right.y, m_right.z);
	//LOG_TRACE("up : {}, {}, {}", m_up.x, m_up.y, m_up.z);
	//LOG_TRACE("for: {}, {}, {}\n\n", m_forward.x, m_forward.y, m_forward.z);
}
} // namespace tk