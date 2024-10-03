#include "light/light.hpp"

Light::Light(const glm::vec3 &position, const glm::vec3 &color)
{
  this->position = position;
  this->color = color;
}

const glm::vec3 &Light::getPosition() const
{
  return position;
}

const glm::vec3 &Light::getColor() const
{
  return color;
}

void Light::setPosition(const glm::vec3 &position)
{
  this->position = position;
}

void Light::setColor(const glm::vec3 &color)
{
  this->color = color;
}
