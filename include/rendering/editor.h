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
		std::string m_directory = "";
		double m_scale = 1.0;
		std::vector<int> m_runtimeModels;
		std::vector<std::string> m_runtimeModelsDirectories;

		Rendering::RenderEngine3D* m_engine;
	};

	void RuntimeModelEditor::update()
	{
		ImGui::Begin("Runtime Model Editor");

		ImGui::Text("Add a model");
		
		char directory[255]{};
		strncpy(directory, m_directory.c_str(), sizeof(directory) - 1);
		ImGui::InputText("", directory, sizeof(directory));
		m_directory = directory;

		ImGui::InputDouble("scale", &m_scale, 0.001);
		
		if (ImGui::Button("Add"))
		{
			ImGui::Text("Loading model, screen will freeze...");
			int loadedModelID = m_engine->createObject(m_directory, m_scale);
			m_runtimeModels.push_back(loadedModelID);
			m_runtimeModelsDirectories.push_back(directory);

			m_directory = "";
			m_scale = 1.0;
		}
		ImGui::Separator();

		ImGui::Text("Models:");
		for (int i = m_runtimeModels.size() - 1; i >= 0; i--)
		{
			if (ImGui::CollapsingHeader(m_runtimeModelsDirectories[i].c_str()))
			{
				ImGui::Indent(8);
				glm::vec3 position = m_engine->getObjectPosition(m_runtimeModels[i]);
				glm::quat orientation = m_engine->getObjectOrientation(m_runtimeModels[i]);
				orientation = glm::normalize(orientation);
				glm::vec3 euler = glm::degrees(glm::eulerAngles(orientation));
				glm::vec3 scaling = m_engine->getObjectScaling(m_runtimeModels[i]);

				ImGui::DragFloat3("Position", glm::value_ptr(position), 0.001);
				ImGui::Text("Orientation");
				ImGui::DragFloat("x", &euler.x, 1.0, -180, 180);
				ImGui::DragFloat("y", &euler.y, 1.0, -90, 90);
				ImGui::DragFloat("z", &euler.z, 1.0, -180, 180);
				//ImGui::DragFloat3("Orientation", glm::value_ptr(euler), 1.0, -180.0, 180);
				ImGui::Spacing();
				ImGui::DragFloat3("Scaling", glm::value_ptr(scaling), 0.001);
				if (ImGui::CollapsingHeader("Delete"))
				{
					ImGui::Indent(8);
					ImGui::Text("Object updates are locked");

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.5, 0.0, 0.0, 1.0 });	// set color red for the button
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.75, 0.0, 0.0, 1.0 });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 1, 0.0, 0.0, 1.0});
					if (ImGui::Button("Confirm"))
					{
						m_engine->deleteObject(m_runtimeModels[i]);
						m_runtimeModels.erase(std::find(m_runtimeModels.begin(), m_runtimeModels.end(), m_runtimeModels[i]));
						m_runtimeModelsDirectories.erase(std::find(m_runtimeModelsDirectories.begin(), m_runtimeModelsDirectories.end(), m_runtimeModelsDirectories[i]));
					}
					ImGui::PopStyleColor(3);
					ImGui::Unindent(8);
				}
				else
				{
					orientation = glm::quat(glm::radians(euler));
					m_engine->updateObject(m_runtimeModels[i], position, orientation);
					m_engine->updateObjectScaling(m_runtimeModels[i], scaling);
				}
				ImGui::Unindent(8);
			}
		}
		ImGui::Separator();

		ImGui::End();

	}
}
