#include "ui_containers/material_ui.hpp"
#include "constants/material_constansts.hpp"

MaterialUi::MaterialUi()
{
  roughness.setLabel(MaterialConstants::ROUGHNESS_UI_LABEL);
  roughness.setMin(MaterialConstants::ROUGHNESS_MIN);
  roughness.setMax(MaterialConstants::ROUGHNESS_MAX);
  roughness.setSpeed(MaterialConstants::ROUGHNESS_UI_SPEED);

  metallic.setLabel(MaterialConstants::METALLIC_UI_LABEL);
  metallic.setMin(MaterialConstants::METALLIC_MIN);
  metallic.setMax(MaterialConstants::METALLIC_MAX);
  metallic.setSpeed(MaterialConstants::METALLIC_UI_SPEED);

  ambientOcclusion.setLabel(MaterialConstants::AMBIENT_OCCLUSION_UI_LABEL);
  ambientOcclusion.setMin(MaterialConstants::AMBIENT_OCCLUSION_MIN);
  ambientOcclusion.setMax(MaterialConstants::AMBIENT_OCCLUSION_MAX);
  ambientOcclusion.setSpeed(MaterialConstants::AMBIENT_OCCLUSION_UI_SPEED);

  albedo.setLabel(MaterialConstants::ALBEDO_UI_LABEL);
  albedo.setColor(MaterialConstants::ALBEDO_UI_DEFAULT);
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
