#include "light/light.hpp"

Light::Light(const glm::vec3 &position, const glm::vec3 &color, const float &intensity)
{
  this->position = position;
  this->color = color;
  this->intensity = intensity;
}

const glm::vec3 &Light::getPosition() const
{
  return position;
}

const glm::vec3 &Light::getColor() const
{
  return color;
}

const float &Light::getIntensity() const
{
  return intensity;
}

void Light::setPosition(const glm::vec3 &position)
{
  this->position = position;
}

void Light::setColor(const glm::vec3 &color)
{
  this->color = color;
}

void Light::setIntensity(const float &intensity)
{
  this->intensity = intensity;
}
