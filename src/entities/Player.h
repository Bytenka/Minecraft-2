#pragma once

#include "../application/Window.h"
#include "../graphics/Camera.h"

namespace tk
{
class Player
{
public:
  Player(const glm::dvec3 &initialPosition = {0, 0, 0},
         const glm::dvec3 &initialDirection = {0, 0, -1});
  ~Player();

  void update(const Window &window) noexcept;

  inline const Camera &getCamera() const noexcept { return m_camera; }

private:
  void poolKeys(const Window &window) noexcept;
  void poolMouse(const Window &window) noexcept;

private:
  Camera m_camera;
  double m_mouseSensitivity = 0.1;

public:
  Player(const Player &) = delete;
  void operator=(const Player &) = delete;
};
} // namespace tk
