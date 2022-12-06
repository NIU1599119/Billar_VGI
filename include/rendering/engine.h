#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

#include "camera/camera.h"
#include "rendering/shader.h"
#include "rendering/object.h"
#include "rendering/lightPoint.h"
#include "rendering/primitives.h"

#define MINIMUM_RENDER_DISTANCE 0.01

namespace Rendering {

	class RenderEngine3D
	{
	public:
		RenderEngine3D(Camera* camera, Rendering::Shader defaultShader, Rendering::Shader* lightShader = nullptr, Rendering::Shader* debugShader = nullptr);
		~RenderEngine3D();

		/////// CAMERA & PROJECTION ///////
		void setCameraFOV(double fov) { m_cameraFOV = fov; updateProjection(); };	// vertical
		void setMaxRenderDistance(double distance) { m_maxRenderDistance = distance; updateProjection(); };
		void setProjectionResolution(double width, double height) { m_screenWidth = width; m_screenHeight = height; updateProjection(); };

		Camera* getCamera() { return m_camera; };
		void updateShaderView();

		/////// OBJECTS ///////
		// generar un nuevo modelo
		int createObject(std::string& path, double scale = 1.0, Rendering::Shader* customShader = nullptr);			// definimos el path del modelo y la escala inicial del objeto
		int createObject(std::string& path, glm::vec3 scale, Rendering::Shader* customShader = nullptr);			// definimos el path del modelo y la escala inicial del objeto
		// utilizar uno ya existente
		int createObject(Rendering::Model* model, double scale = 1.0, Rendering::Shader* customShader = nullptr);	// definimos el path del modelo y la escala inicial del objeto
		int createObject(Rendering::Model* model, glm::vec3 scale, Rendering::Shader* customShader = nullptr);		// definimos el path del modelo y la escala inicial del objeto
		
		// Nota : despues de borrar un objeto se tienen que actualizar los indices de los objetos que van despues a uno menos
		void deleteObject(int id);

		void updateObject(int id, glm::vec3 position, glm::quat orientation);	// usar para pasar datos del bullet
		void updateObjectPosition(int id, glm::vec3 position);
		void updateObjectOrientation(int id, glm::quat orientation);
		void updateObjectOrientation(int id, glm::vec3 axis, double radians);
		void updateObjectScaling(int id, glm::vec3 scaling);	// escala relativa (poner 2 significa que sera 2 veces mas grande, sin importar la escala introducida al inicializar)
		void updateObjectScaling(int id, double scaling);	// escala relativa (poner 2 significa que sera 2 veces mas grande, sin importar la escala introducida al inicializar)

		void draw(int id);
		void drawAll();

		bool existsObjectID(int id) { return (m_objects.size() > id && id > 0); };

		/////// LIGHTS ///////
		int addLight();

		void setLightPosition(int id, glm::vec3 position);
		void setLightColor(int id, glm::vec3 color);
		void setLightPolinomial(int id, float constant, float linear, float cuadratic);

		void drawLights();
		void updateShaderLighting();

		/////// DEBUGGING ///////
		
		Rendering::Shader* getDebuggingShader() { return m_debugShader; };	// returns the debugging shader to let the collision box use it


	private:
		/////// CAMERA & PROJECTION ///////
		double m_screenWidth = 16.0, m_screenHeight = 9.0;
		double m_cameraFOV = 45.0;	// vertical
		double m_maxRenderDistance = 100.0;
		glm::mat4 m_projection;
		Camera* m_camera;

		void updateShaderProjection();
		void updateProjection();

		/////// OBJECTS ///////
		Rendering::Shader m_defaultModelShader;			// shader por defecto
		std::vector<Rendering::Shader*> m_shaders;	// posibles shaders especificos para algunos objetos (si son nullptr se utiliza el m_generalModelShader)
		std::vector<Rendering::Object> m_objects;	// estructura con los objetos que se renderizan cada frame

		/////// LIGHTS ///////
		
		static const int MAX_LIGHTS = 5;				// poner el definido en el shader por defecto
		std::vector<Rendering::LightPoint> m_lights;	// luces

		const double LIGHT_SCALE = 0.1;		// for the debug model
		Rendering::Shader* m_lightShader;
		Rendering::Model* m_debugBoxModel;

		/////// DEBUGGING ///////
		Rendering::Shader* m_debugShader;

	};
}