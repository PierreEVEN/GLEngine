#include <Lighting/Light.h>

Light::Light(Scene* drawScene)
    : SceneComponent(drawScene)
{
}

void Light::Tick()
{
    SceneComponent::Tick();
}
