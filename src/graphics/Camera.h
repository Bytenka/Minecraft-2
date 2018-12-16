#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace tk
{
class Camera
{
public:
	Camera(
			glm::dvec3 position = glm::dvec3(0.0, 0.0, 0.0),
			glm::dvec3 target = glm::dvec3(0.0, 0.0, -1.0));
	~Camera(){};

	void rotate(double pitch, double yaw);
	inline void rotate(const glm::dvec2 &by) noexcept { rotate(by.x, by.y); }
	inline void move(glm::dvec3 direction) noexcept { m_position += direction; };
	inline void teleport(const glm::dvec3 &toLocation) noexcept { m_position = toLocation; };

	inline glm::mat4 getView() const { return glm::lookAt(m_position, m_position - m_direction, glm::dvec3(0.0, 1.0, 0.0)); }
	inline glm::dvec3 getDirection() const noexcept { return m_direction; }
	inline glm::dvec3 getForward() const noexcept { return m_forward; }
	inline glm::dvec3 getPosition() const noexcept { return m_position; }
	inline glm::dvec3 getRight() const noexcept { return m_right; }
	inline glm::dvec3 getUp() const noexcept { return m_up; }

private:
	glm::dvec3 m_position;
	glm::dvec3 m_direction;

	glm::dvec3 m_forward; // Like direction, but it's the way, not where the player looks
	glm::dvec3 m_right;
	glm::dvec3 m_up;

	double m_yaw = 0, m_pitch = 0;
};
} // namespace tk
