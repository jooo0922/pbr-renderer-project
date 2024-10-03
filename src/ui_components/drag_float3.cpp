#include "ui_components/drag_float3.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

DragFloat3::DragFloat3()
    : label_(nullptr),
      value_(0.f),
      speed_(0.f),
      min_(0.f),
      max_(0.f)
{
}

bool DragFloat3::onUiComponent()
{
  if (ImGui::DragFloat3(label_, &value_[0], speed_, min_, max_))
  {
    return true;
  }

  return false;
}

void DragFloat3::setLabel(const char *label)
{
  label_ = label;
}

void DragFloat3::setValue(const glm::vec3 &value)
{
  value_ = value;
}

void DragFloat3::setSpeed(const float speed)
{
  speed_ = speed;
}

void DragFloat3::setMin(const float min)
{
  min_ = min;
}

void DragFloat3::setMax(const float max)
{
  max_ = max;
}

glm::vec3 DragFloat3::getValue() const
{
  return value_;
}
