#include "rendering/modelController.h"

void Rendering::ModelController::addObject(const std::string& path, glm::vec3& position, float scale = 1.0, float angle = 0, glm::vec3& direction = glm::vec3())
{
	Rendering::Model newModel(path);
	Rendering::Object newObject(&newModel);
	newObject.setPosition(position);

	if (scale != 1.0)
		newObject.setScaling(scale);
	if (angle != 0)
		newObject.setOrientation(0, direction);

	m_objectArray.push_back(newObject);
}

void Rendering::ModelController::drawObjects(Camera& camera)
{
	for (auto it = m_objectArray.begin(); it != m_objectArray.end(); it++)
	{
		(*it).draw(&m_modelShader, camera.getViewMatrix(), m_projection, camera.getPosition());
	}
}
