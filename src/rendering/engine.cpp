#include "rendering/engine.h"

namespace Rendering {

	RenderEngine3D::RenderEngine3D(Camera* camera, Rendering::Shader* defaultShader, Rendering::Shader* shadowShader, Rendering::Shader* lightShader, Rendering::Shader* debugShader)
		:
		m_camera(camera),
		m_defaultModelShader(defaultShader),
		m_shadowModelShader(shadowShader),
		m_lightShader(lightShader),
		m_debugShader(debugShader)
	{
		m_debugBoxModel = Primitives::getNewCubeModel();
		updateProjection();
		GL(glEnable(GL_MULTISAMPLE));
		//GL(glEnable(GL_CULL_FACE));	// para poner esto habria que arreglar el modelo de la habitacion
		GL(glEnable(GL_DEPTH_TEST));
		GL(glEnable(GL_STENCIL_TEST));
		GL(glDepthFunc(GL_LESS));
		GL(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
		GL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
	}

	RenderEngine3D::~RenderEngine3D()
	{
		GL(glEnable(GL_MULTISAMPLE));
		//GL(glDisable(GL_CULL_FACE));
		GL(glDisable(GL_DEPTH_TEST));
		GL(glDisable(GL_STENCIL_TEST));
		// render engine se encarga de borrar todos los modelos utilizados
		std::vector<Rendering::Model*> uniqueModels;
		for (int i = 0; i < m_models.size(); i++)
		{
			if (!m_manageModel[i]) continue;	// si no se encarga de borrarlo se sale
			if (m_models[i] == nullptr) continue;
			std::vector<Rendering::Model*>::iterator it = std::find(uniqueModels.begin(), uniqueModels.end(), m_models[i]);
			if (it == uniqueModels.end())
			{
				uniqueModels.push_back(m_models[i]);
			}
			m_models[i] = nullptr;
		}

		for (int i = 0; i < uniqueModels.size(); i++)
		{
			delete uniqueModels[i];
		}
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
		Rendering::updateShaderView(m_shadowModelShader, view, viewPos);

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
		Rendering::updateShaderProjection(m_shadowModelShader, m_projection);
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
		Rendering::Model* model = Rendering::createModel(path);
		if (model == nullptr) return -1;
		return createObject(model, scale, true, customShader);
	}
	int RenderEngine3D::createObject(Rendering::Model* model, double scale, bool manage, Rendering::Shader* customShader)
	{
		return createObject(model, glm::vec3(scale), manage, customShader);
	}
	int RenderEngine3D::createObject(Rendering::Model* model, glm::vec3 scale, bool manage, Rendering::Shader* customShader)
	{
		Rendering::Object newObject = Rendering::Object(model, scale);
		
		int idx = m_objects.size();
		m_objects.push_back(newObject);
		m_models.push_back(model);
		m_manageModel.push_back(manage);
		m_shaders.push_back(customShader);	// sera nullptr por defecto
		return idx;
	}

	void RenderEngine3D::disableObject(int id)
	{
		m_objects[id].setDisabled(true);
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

	void RenderEngine3D::drawWithOutline(int id, glm::vec3 color)
	{
		Rendering::Shader* objectShader = m_shaders[id];
		if (objectShader == nullptr) objectShader = m_defaultModelShader;

		GL(glStencilFunc(GL_ALWAYS, 1, 0xFF)); // all fragments should pass the stencil test
		GL(glStencilMask(0xFF)); // enable writing to the stencil buffer

		m_objects[id].draw(objectShader);

		GL(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
		GL(glStencilMask(0x00)); // disable writing to the stencil buffer
		GL(glDisable(GL_DEPTH_TEST));
		
		m_debugShader->activate();
		m_debugShader->setUniformVec3("color", color);

		glm::vec3 previous = m_objects[id].getScaling();
		m_objects[id].setScaling(previous * 1.2f);
		m_objects[id].draw(m_debugShader);
		m_objects[id].setScaling(previous);

		GL(glStencilMask(0xFF)); // enable writing to the stencil buffer
		GL(glStencilFunc(GL_ALWAYS, 0, 0xFF)); // all fragments should pass the stencil test
		GL(glEnable(GL_DEPTH_TEST));
	}

	void RenderEngine3D::drawAllMinus(int id)
	{
		// esta version del renderizado es mas eficiente (lo hace todo de golpe)
		updateShaderLighting();

		for (int i = 0; i < m_objects.size(); i++)
		{
			if (id == i) continue;
			Rendering::Shader* objectShader = m_shaders[i];
			if (objectShader == nullptr) objectShader = m_defaultModelShader;
			m_objects[i].draw(objectShader);
		}

		for (int i = 0; i < m_lines.size(); i++)
		{
			m_lines[i].setViewProjection(m_camera->getViewMatrix(), m_projection);
			m_lines[i].draw();
		}
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

		for (int i = 0; i < m_lines.size(); i++)
		{
			m_lines[i].setViewProjection(m_camera->getViewMatrix(), m_projection);
			m_lines[i].draw();
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
			Rendering::updateShaderLighting(m_shadowModelShader, i, m_lights[i]);

			for (auto it = m_shaders.begin(); it != m_shaders.end(); it++)
			{
				if (*it == nullptr) continue;
				Rendering::updateShaderLighting(*it, i, m_lights[i]);
			}
		}

		Rendering::updateShaderLighting(m_defaultModelShader, m_lights.size());
		Rendering::updateShaderLighting(m_shadowModelShader, m_lights.size());

		for (auto it = m_shaders.begin(); it != m_shaders.end(); it++)
		{
			if (*it == nullptr) continue;
			Rendering::updateShaderLighting(*it, m_lights.size());
		}
	}
}
