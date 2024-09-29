#include "ui_components/drag_float.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

DragFloat::DragFloat()
    : label_(nullptr),
      value_(0.f),
      speed_(0.f),
      min_(0.f),
      max_(0.f)
{
}

bool DragFloat::onUiComponent()
{
  if (ImGui::DragFloat(label_, &value_, speed_, min_, max_))
  {
    return true;
  }

  return false;
}

void DragFloat::setLabel(const char *label)
{
  label_ = label;
}

void DragFloat::setValue(const float value)
{
  value_ = value;
}

void DragFloat::setSpeed(const float speed)
{
  speed_ = speed;
}

void DragFloat::setMin(const float min)
{
  min_ = min;
}

void DragFloat::setMax(const float max)
{
  max_ = max;
}

float DragFloat::getValue() const
{
  return value_;
}
