
#include <glm/glm.hpp>

class World;
class MeshSectionComponent;


class PhysicsLibrary
{

public:

	static bool LineTrace(const World* &inWorld, const glm::vec3 &startPos, const glm::vec3 &endPos, const glm::vec3 &impactPoint, const glm::vec3 &impactNormal, MeshSectionComponent* hitComponent);

};