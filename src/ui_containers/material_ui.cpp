#include "ui_containers/material_ui.hpp"

MaterialUi::MaterialUi()
{
  roughness.setLabel("roughness");
  roughness.setMin(0.f);
  roughness.setMax(1.f);
  roughness.setSpeed(0.001f);

  metallic.setLabel("metallic");
  metallic.setMin(0.f);
  metallic.setMax(1.f);
  metallic.setSpeed(0.001f);

  ambientOcclusion.setLabel("ambientOcclusion");
  ambientOcclusion.setMin(0.f);
  ambientOcclusion.setMax(1.f);
  ambientOcclusion.setSpeed(0.001f);

  albedo.setLabel("albedo");
  albedo.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
}

MaterialUi::~MaterialUi()
{
}

bool MaterialUi::onUiComponents()
{
  bool ret = false;
  ret |= roughness.onUiComponent();
  ret |= metallic.onUiComponent();
  ret |= ambientOcclusion.onUiComponent();
  ret |= albedo.onUiComponent();
  return ret;
}

void MaterialUi::onChange(const MaterialParameter &param)
{
  roughness.setValue(param.roughness);
  metallic.setValue(param.metallic);
  ambientOcclusion.setValue(param.ambientOcclusion);
  albedo.setColor(param.albedo);
}

void MaterialUi::getMaterialParam(MaterialParameter &param) const
{
  param.roughness = roughness.getValue();
  param.metallic = metallic.getValue();
  param.ambientOcclusion = ambientOcclusion.getValue();
  param.albedo = albedo.getColor();
}
