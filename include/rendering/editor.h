#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "rendering/engine.h"

namespace Rendering
{
	class RuntimeModelEditor
	{
	public:
		RuntimeModelEditor(Rendering::RenderEngine3D* engine) : m_engine(engine) {};
		void update();
	private:
		std::string m_directory = "models/pool_table/scene.gltf";
		double m_scale = 0.109025;
		std::vector<int> m_runtimeModels;
		std::vector<std::string> m_runtimeModelsDirectories;
		std::vector<glm::vec3> m_modelRotations;

		Rendering::RenderEngine3D* m_engine;
	};
}
