#include "staticMeshComponent.h"
#include "staticMesh.h"
#include "meshSectionComponent.h"
#include <glfw3/glfw3.h>
#include "../Engine/debugerTool.h"
#include <bullet3D/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <bullet3D/btBulletDynamicsCommon.h>
#include "../MathLibrary/vector3.h"
#include "../UI/EditorWindows/sceneComponentEditor.h"


void StaticMeshComponent::SetSimulatePhysic(bool bInSimulatePhysics)
{
	if (body)
	{
		btVector3 localInertia(0, 0, 0);
		btScalar mass = bInSimulatePhysics ? .5f : 0.f;
		body->setMassProps(mass, localInertia);
	}
}

bool StaticMeshComponent::GetSimulatePhysics()
{
	if (body)
	{
		return body->getMass() != 0;
	}
	return false;
}

StaticMeshComponent::StaticMeshComponent(Scene* inScene, StaticMesh* inMeshData, std::vector<Material*> newMaterialOverride /*= {}*/)
	: PrimitiveComponent(inScene)
{
	meshData = inMeshData;
	tmpMaterialOveride = newMaterialOverride;
}

StaticMeshComponent::StaticMeshComponent(Scene* inScene, StaticMesh* inMeshData, std::vector<Material*> newMaterialOverride /*= {}*/, btCollisionShape* physicBody, bool bSimulatePhysics)
	: StaticMeshComponent(inScene, inMeshData, newMaterialOverride)
{
// 	if (physicBody)
// 	{
// 		btCollisionShape* shape = physicBody;
// 
// 		btTransform myTransform;
// 		myTransform.setIdentity();
// 		myTransform.setOrigin(GetLocation().ToBulletVector());
// 
// 		btVector3 localInertia(0, 0, 0);
// 		btScalar mass = bSimulatePhysics ? .5f : 0.f;
// 		shape->calculateLocalInertia(mass, localInertia);
// 
// 		myMotionState = new btDefaultMotionState(myTransform);
// 		btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo(mass, myMotionState, shape, localInertia);
// 		body = new btRigidBody(myBoxRigidBodyConstructionInfo);
// 		GetScene()->GetPhysicWorld()->addRigidBody(body);
// 	}
}

StaticMeshComponent::~StaticMeshComponent()
{
	for (auto& section : sections)
	{
		delete section;
	}
	sections.clear();
	meshData = nullptr;
/*	if (body) GetScene()->GetPhysicWorld()->removeRigidBody(body);*/
	delete body;
	delete myMotionState;
}

void StaticMeshComponent::SetLocation(SVector3 newLocation)
{
	PrimitiveComponent::SetLocation(newLocation);
	if (body)
	{
		btTransform newTransform;
		newTransform = body->getWorldTransform();
		newTransform.setOrigin(newLocation.ToBulletVector());
		body->setWorldTransform(newTransform);
		body->activate(true);
	}
}

void StaticMeshComponent::SetForwardVector(glm::vec3 newForwardVector)
{
	PrimitiveComponent::SetForwardVector(newForwardVector);
	if (body)
	{
		btTransform newTransform;
		newTransform = body->getWorldTransform();
		btQuaternion rotation = newTransform.getRotation();
		rotation.setRotation(SVector3(newForwardVector).ToBulletVector(), rotation.getAngle());
		newTransform.setRotation(rotation);
		body->setWorldTransform(newTransform);
		body->activate(true);
	}
}

void StaticMeshComponent::SetAngle(float newAngle)
{
	PrimitiveComponent::SetAngle(newAngle);
	if (body)
	{
		btTransform newTransform;
		newTransform = body->getWorldTransform();
		btQuaternion rotation = newTransform.getRotation();
		rotation.setRotation(rotation.getAxis(), ((newAngle * glm::pi<float>()) / 180.f));
		newTransform.setRotation(rotation);
		body->setWorldTransform(newTransform);
		body->activate(true);
	}
}

void StaticMeshComponent::SetScale3D(SVector3 newScale3D)
{
	PrimitiveComponent::SetScale3D(newScale3D);
}


SVector3 StaticMeshComponent::GetLocation() const
{
	if (body)
	{
		return SVector3(body->getWorldTransform().getOrigin()).ToGLVector();
	}
	return PrimitiveComponent::GetLocation();
}

float StaticMeshComponent::GetAngle() const
{
	if (body)
	{
		return (body->getWorldTransform().getRotation().getAngle() * 180.f) / glm::pi<float>();
	}
	return PrimitiveComponent::GetAngle();
}

glm::vec3 StaticMeshComponent::GetForwardVector() const
{
	if (body)
	{
		btQuaternion objectRotation = body->getWorldTransform().getRotation();
		return glm::vec3((float)objectRotation.getAxis().getX(), (float)objectRotation.getAxis().getY(), (float)objectRotation.getAxis().getZ());
	}
	return PrimitiveComponent::GetForwardVector();
}

SVector3 StaticMeshComponent::GetScale3D() const
{
	return PrimitiveComponent::GetScale3D();
}

void StaticMeshComponent::Tick()
{
	if (sections.size() == 0)
	{
		for (auto& section : meshData->GetSections())
		{
			MeshSectionComponent* newSection = new MeshSectionComponent(GetScene(), this, section);
			newSection->SetLocation(GetLocation());
			sections.push_back(newSection);
		}
		for (unsigned int i = 0; i < tmpMaterialOveride.size() && i < sections.size(); ++i)
		{
			sections[i]->materialOverride = tmpMaterialOveride[i];
		}
		RebuildLocalBounds();
	}

	if (body && body->isActive()&& GetSimulatePhysics())
	{
		MarkTransformDirty();
		if (GetLocation().z < -1000)
		{
			delete this;
			return;
		}
	}
	PrimitiveComponent::Tick();
}

void StaticMeshComponent::RebuildTransformData()
{
	PrimitiveComponent::RebuildTransformData();
	for (auto& section : sections)
	{
		section->SetLocation(GetLocation());
		section->SetAngle(GetAngle());
		section->SetRotation(GetRotation());
		section->SetScale3D(GetScale3D());
		section->SetForwardVector(GetForwardVector());
	}
}

void StaticMeshComponent::RebuildLocalBounds()
{
	localBounds = meshData->bounds;
	PrimitiveComponent::RebuildLocalBounds();
}

void StaticMeshComponent::DrawEditorWindow()
{
	PrimitiveComponent::DrawEditorWindow();
	if (body)
	{
		bool bSimulatePhys = GetSimulatePhysics();
		ImGui::Checkbox("Simulate physics", &bSimulatePhys);
		if (bSimulatePhys != GetSimulatePhysics())
		{
			SetSimulatePhysic(bSimulatePhys);
			if (bSimulatePhys)
			{
				body->activate(true);
			}
		}
	}
}
