#include "rendering/engine.h"

namespace Rendering {

	RenderEngine3D::RenderEngine3D(Camera* camera, Rendering::Shader* defaultShader, Rendering::Shader* lightShader, Rendering::Shader* debugShader)
		:
		m_camera(camera),
		m_defaultModelShader(defaultShader),
		m_lightShader(lightShader),
		m_debugShader(debugShader)
	{
		m_debugBoxModel = Primitives::getNewCubeModel();
		updateProjection();
		glEnable(GL_DEPTH_TEST);
	}

	RenderEngine3D::~RenderEngine3D()
	{
		glDisable(GL_DEPTH_TEST);
	}

	/////// CAMERA & PROJECTION ///////

	void updateShaderView(Rendering::Shader* s, glm::mat4& view, glm::vec3& viewPos)
	{
		s->activate();
		s->setUniformMat4("view", view);
		s->setUniformVec3("viewPos", viewPos);
	}
	void RenderEngine3D::updateShaderView()
	{
		// updated every frame
		glm::mat4 view = m_camera->getViewMatrix();
		glm::vec3 viewPos = m_camera->getPosition();
		Rendering::updateShaderView(m_defaultModelShader, view, viewPos);

		for (auto it = m_shaders.begin(); it != m_shaders.end(); it++)
		{
			if (*it == nullptr) continue;
			Rendering::updateShaderView(*it, view, viewPos);
		}

		if (m_lightShader != nullptr)
		{
			Rendering::updateShaderView(m_lightShader, view, viewPos);
		}
		if (m_debugShader != nullptr)
		{
			Rendering::updateShaderView(m_debugShader, view, viewPos);
		}
	}

	void RenderEngine3D::updateProjection()
	{
		m_projection = glm::perspective(m_cameraFOV, m_screenWidth / m_screenHeight, MINIMUM_RENDER_DISTANCE, m_maxRenderDistance);
		updateShaderProjection();
	}

	void updateShaderProjection(Rendering::Shader* s, glm::mat4& projection)
	{
		s->activate();
		s->setUniformMat4("projection", projection);
	}
	void RenderEngine3D::updateShaderProjection()
	{
		// updated only when resizing the window or changing the FOV or the maxRenderDistance
		Rendering::updateShaderProjection(m_defaultModelShader, m_projection);
		for (auto it = m_shaders.begin(); it != m_shaders.end(); it++)
		{
			if (*it == nullptr) continue;
			Rendering::updateShaderProjection(*it, m_projection);
		}

		if (m_lightShader != nullptr)
		{
			Rendering::updateShaderProjection(m_lightShader, m_projection);
		}
		if (m_debugShader != nullptr)
		{
			Rendering::updateShaderProjection(m_debugShader, m_projection);
		}
	}


	/////// OBJECTS ///////

	int RenderEngine3D::createObject(std::string path, double scale, Rendering::Shader* customShader)
	{
		return createObject(path, glm::vec3(scale), customShader);
	}
	int RenderEngine3D::createObject(std::string path, glm::vec3 scale, Rendering::Shader* customShader)
	{
		// generate Mesh
		Rendering::Model* model = new Rendering::Model(path);
		return createObject(model, scale, customShader);
	}
	int RenderEngine3D::createObject(Rendering::Model* model, double scale, Rendering::Shader* customShader)
	{
		return createObject(model, glm::vec3(scale), customShader);
	}
	int RenderEngine3D::createObject(Rendering::Model* model, glm::vec3 scale, Rendering::Shader* customShader)
	{
		Rendering::Object newObject = Rendering::Object(model, scale);
		
		int idx = m_objects.size();
		m_objects.push_back(newObject);
		m_shaders.push_back(customShader);	// sera nullptr por defecto
		return idx;
	}

	void RenderEngine3D::deleteObject(int id)
	{
		m_objects.erase(m_objects.begin() + id);
		m_shaders.erase(m_shaders.begin() + id);
	}

	void RenderEngine3D::updateObject(int id, glm::vec3 position, glm::quat orientation)
	{
		m_objects[id].setPosition(position);
		m_objects[id].setOrientation(orientation);
	}

	void RenderEngine3D::updateObjectPosition(int id, glm::vec3 position)
	{
		m_objects[id].setPosition(position);
	}

	void RenderEngine3D::updateObjectOrientation(int id, glm::quat orientation)
	{
		m_objects[id].setOrientation(orientation);
	}

	void RenderEngine3D::updateObjectOrientation(int id, glm::vec3 axis, double radians)
	{
		m_objects[id].setOrientation(radians, axis);
	}

	void RenderEngine3D::updateObjectScaling(int id, glm::vec3 scaling)
	{
		m_objects[id].setScaling(scaling);
	}

	void RenderEngine3D::updateObjectScaling(int id, double scaling)
	{
		m_objects[id].setScaling(scaling);
	}

	void RenderEngine3D::draw(int id)
	{
		Rendering::Shader* objectShader = m_shaders[id];
		if (objectShader == nullptr) objectShader = m_defaultModelShader;
		m_objects[id].draw(objectShader);
	}

	void RenderEngine3D::drawAll()
	{
		// esta version del renderizado es mas eficiente (lo hace todo de golpe)
		updateShaderLighting();

		for (int i = 0; i < m_objects.size(); i++)
		{
			Rendering::Shader* objectShader = m_shaders[i];
			if (objectShader == nullptr) objectShader = m_defaultModelShader;
			m_objects[i].draw(objectShader);
		}
	}

	/////// LIGHTS ///////
	int RenderEngine3D::addLight()
	{
		if (m_lights.size() >= MAX_LIGHTS)
		{
			LOG_ERROR("No se pueden poner mas de %d luces", MAX_LIGHTS);
			return -1;	// no se pueden poner mas luces
		}
		m_lights.push_back(Rendering::LightPoint());
		return m_lights.size() - 1;	// indice de la luz
	}

	void RenderEngine3D::setLightPosition(int id, glm::vec3 position)
	{
		assert(id >= 0);
		assert(id < MAX_LIGHTS);
		m_lights[id].setPosition(position);
	}
	void RenderEngine3D::setLightColor(int id, glm::vec3 color)
	{
		assert(id >= 0);
		assert(id < MAX_LIGHTS);
		m_lights[id].setColor(color);
	}

	void RenderEngine3D::setLightPolinomial(int id, float constant, float linear, float cuadratic)
	{
		assert(id >= 0);
		assert(id < MAX_LIGHTS);
		m_lights[id].setPolinomial(constant, linear, cuadratic);
	}

	void RenderEngine3D::drawLights()
	{
		GL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));    // wireframe rendering

		for (int i = 0; i < m_lights.size(); i++)
		{
			glm::vec3 pos = m_lights[i].getPosition();
			glm::vec3 color = m_lights[i].getColor();
			glm::mat4 translate = glm::mat4(1.0f);
			translate = glm::translate(translate, pos);
			glm::mat4 orientation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(LIGHT_SCALE));
			glm::mat4 model = translate * orientation * scale;

			m_lightShader->activate();
			m_lightShader->setUniformVec3("lightColor", color);
			m_lightShader->setUniformMat4("model", model);
			m_lightShader->setUniformMat4("normalRotation", orientation);

			m_debugBoxModel->draw(m_lightShader);
		}
		GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));    // disable wireframe rendering
	}
	
	void updateShaderLighting(Rendering::Shader* s, int num)
	{
		s->activate();
		s->setUniformInt("n_pointLights", num);
	}
	void updateShaderLighting(Rendering::Shader* s, int lightID, Rendering::LightPoint& lightPoint)
	{
		lightPoint.updateShader(lightID, s);
		// estructura de las luces de los shaders:
		/*
		struct PointLight {
			vec3 position;

			float constant;
			float linear;
			float quadratic;

			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
		};
		#define NR_POINT_LIGHTS 4

		uniform PointLight pointLights[NR_POINT_LIGHTS];
		uniform int n_pointLights;
		*/
	}
	void RenderEngine3D::updateShaderLighting()
	{
		for (int i = 0; i < m_lights.size(); i++)
		{
			Rendering::updateShaderLighting(m_defaultModelShader, i, m_lights[i]);

			for (auto it = m_shaders.begin(); it != m_shaders.end(); it++)
			{
				if (*it == nullptr) continue;
				Rendering::updateShaderLighting(*it, i, m_lights[i]);
			}
		}

		Rendering::updateShaderLighting(m_defaultModelShader, m_lights.size());

		for (auto it = m_shaders.begin(); it != m_shaders.end(); it++)
		{
			if (*it == nullptr) continue;
			Rendering::updateShaderLighting(*it, m_lights.size());
		}
	}
}
