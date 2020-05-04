#include "renderer.h"

#include "global.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

namespace finnsie {

	// NOTE(CK): Global ResourceManager 
	//			 have to initialize here inside of the namespace
	ResourceManager* g_resourceManager;

	Renderer::Renderer()
	{
		::finnsie::g_resourceManager = new ResourceManager();

		this->color = glm::vec3(0.1f, 0.5f, 0.31f);
		this->colorChange = 0.01f;
		this->lampPos = glm::vec3(1.2f, 1.0f, 2.0f);

		this->projection = glm::mat4(1.0f);
		this->view = glm::mat4(1.0f);

		this->drawingNormals = false;

		// init shaders before uniforms
		initShaders();
		initUniforms();

		// TODO(CK): CLEAN UP
		// FOR LIGHTING
		// -------------------
		this->lightPos = glm::vec3(-120.0f, -40.0f, -50.0f);


		this->waterQuad = new Model("water",
									false,
									glm::vec3(-100.0f, -30.0f, 0.0f),
									glm::vec3(0.0f, 0.0f, 0.0f),
									10.0f,
									"content/primitives/quad/basic_quad.obj");
	}

	Renderer::~Renderer()
	{
		glDeleteVertexArrays(1, &this->cubeVAO);
		glDeleteVertexArrays(1, &this->lampVAO);
		glDeleteBuffers(1, &this->VBO);
	}

	// This begin render will happen before the model vector is looped through
	void Renderer::BeginRender(Camera& cam)
	{
		this->projection = glm::perspective(glm::radians(cam.Zoom),
										   (float)1080 / (float)720, 
											1.0f, 1000.0f); // NOTE(CK): near and far clipping distance
		this->view = cam.GetViewMatrix();
		this->camPos = cam.Position;
	}

	void Renderer::DrawModel(Model& model)
	{
		// Only use the modelShader when not drawing normals
		if (!drawingNormals)
			startShader(modelShader.id, objModelLoc, objProjLoc, objViewLoc);
		else
			startShader(normalShader.id, normalModelLoc, normalProjLoc, normalViewLoc);

		for (unsigned int i = 0; i < model.meshes.size(); i++)
		{
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;

			for (unsigned int j = 0; j < model.meshes[i].textures.size(); ++j)
			{
				glActiveTexture(GL_TEXTURE0 + j); // activate the proper texture unit before binding
				// retrieve texture number (the N in diffuse_TextureN)
				std::string number;
				std::string name = model.meshes[i].textures[j].type;

				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);
				else if (name == "texture_normal")
					number = std::to_string(normalNr++);
				else if (name == "texture_height")
					number = std::to_string(heightNr++);

				// now set the sampler to the correct texture unit
				int loc = glGetUniformLocation(activeModelShaderId, (name + number).c_str());
				glUniform1i(glGetUniformLocation(activeModelShaderId, (name + number).c_str()), j);
				// and finally bind the texture		
				glBindTexture(GL_TEXTURE_2D, model.meshes[i].textures[j].id);
			}
			
			// TODO(CK): CLEAN UP
			// Water distortion
			// --------------------
			glUniform1f(glGetUniformLocation(activeModelShaderId, "time"), glfwGetTime());

			glUniform3fv(glGetUniformLocation(activeModelShaderId, "light.position"), 1, &lightPos[0]);
			glUniform3fv(glGetUniformLocation(activeModelShaderId, "viewPos"), 1, &camPos[0]); // getting updated in BeginRender (probably not good)
			glUniform3f(glGetUniformLocation(activeModelShaderId, "light.ambient"), 0.8f, 0.8f, 0.8f);
			glUniform3f(glGetUniformLocation(activeModelShaderId, "light.diffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(activeModelShaderId, "light.specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(activeModelShaderId, "material.shininess"), 64.0f);


			// Set position, rotation and scale
			glm::mat4 matModel = glm::mat4(1.0f);

			glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
													glm::vec3(model.pos.x, model.pos.y, model.pos.z));
			matModel = matModel * matTranslate;

			glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
											model.orientation.z,
											glm::vec3(0.0f, 0.0f, 1.0f));
			matModel = matModel * rotateZ;

			glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
											model.orientation.y,
											glm::vec3(0.0f, 1.0f, 0.0f));
			matModel = matModel * rotateY;

			glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
											model.orientation.x,
											glm::vec3(1.0f, 0.0f, 0.0f));
			matModel = matModel * rotateX;

			glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
								 glm::vec3(model.scale, model.scale, model.scale));

			matModel = matModel * matScale;
			glUniformMatrix4fv(activeModelLoc, 1, GL_FALSE, glm::value_ptr(matModel));

			/*
			// INVERSE WAS FROM GRAPHICS CLASS
				GLint matModel_loc = glGetUniformLocation(shaderProgID, "matModel");
				GLint matModelInvTran_loc = glGetUniformLocation(shaderProgID, "matModelInvTrans");
			*/

			if (model.wireFrame)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			// Draw mesh
			glBindVertexArray(model.meshes[i].VAO);
			glDrawElements(GL_TRIANGLES, model.meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// Always good practice to set everything back to defaults once configured
			// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		if (model.viewNormals && !drawingNormals)
		{
			// Normals are only drawn on the next draw call
			drawingNormals = true;
			DrawModel(model);
			drawingNormals = false;
		}
	}

	void Renderer::DrawWater()
	{
		startShader(modelShader.id, objModelLoc, objProjLoc, objViewLoc);

		for (unsigned int i = 0; i < waterQuad->meshes.size(); i++)
		{
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;

			for (unsigned int j = 0; j < waterQuad->meshes[i].textures.size(); ++j)
			{
				glActiveTexture(GL_TEXTURE0 + j); // activate the proper texture unit before binding
				// retrieve texture number (the N in diffuse_TextureN)
				std::string number;
				std::string name = waterQuad->meshes[i].textures[j].type;

				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);
				else if (name == "texture_normal")
					number = std::to_string(normalNr++);
				else if (name == "texture_height")
					number = std::to_string(heightNr++);

				// now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(activeModelShaderId, (name + number).c_str()), j);
				// and finally bind the texture		
				glBindTexture(GL_TEXTURE_2D, waterQuad->meshes[i].textures[j].id);
			}

			// TODO(CK): CLEAN UP
			// Water distortion
			// --------------------
			glUniform1f(glGetUniformLocation(activeModelShaderId, "time"), glfwGetTime());

			glUniform3fv(glGetUniformLocation(activeModelShaderId, "light.position"), 1, &lightPos[0]);
			glUniform3fv(glGetUniformLocation(activeModelShaderId, "viewPos"), 1, &camPos[0]); // getting updated in BeginRender (probably not good)
			glUniform3f(glGetUniformLocation(activeModelShaderId, "light.ambient"), 0.8f, 0.8f, 0.8f);
			glUniform3f(glGetUniformLocation(activeModelShaderId, "light.diffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(activeModelShaderId, "light.specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(activeModelShaderId, "material.shininess"), 64.0f);


			// Set position, rotation and scale
			glm::mat4 matModel = glm::mat4(1.0f);

			glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
													glm::vec3(waterQuad->pos.x, waterQuad->pos.y, waterQuad->pos.z));
			matModel = matModel * matTranslate;

			glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
											waterQuad->orientation.z,
											glm::vec3(0.0f, 0.0f, 1.0f));
			matModel = matModel * rotateZ;

			glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
											waterQuad->orientation.y,
											glm::vec3(0.0f, 1.0f, 0.0f));
			matModel = matModel * rotateY;

			glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
											waterQuad->orientation.x,
											glm::vec3(1.0f, 0.0f, 0.0f));
			matModel = matModel * rotateX;

			glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
											glm::vec3(waterQuad->scale, waterQuad->scale, waterQuad->scale));

			matModel = matModel * matScale;
			glUniformMatrix4fv(activeModelLoc, 1, GL_FALSE, glm::value_ptr(matModel));

			/*
			// INVERSE WAS FROM GRAPHICS CLASS
				GLint matModel_loc = glGetUniformLocation(shaderProgID, "matModel");
				GLint matModelInvTran_loc = glGetUniformLocation(shaderProgID, "matModelInvTrans");
			*/

			if (waterQuad->wireFrame)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			// Draw mesh
			glBindVertexArray(waterQuad->meshes[i].VAO);
			glDrawElements(GL_TRIANGLES, waterQuad->meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// Always good practice to set everything back to defaults once configured
			// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		return;
	}

	void Renderer::EndRender()
	{
		return;
	}

	void Renderer::startShader(unsigned int shaderId, int modelLoc, int projLoc, int viewLoc)
	{
		glUseProgram(shaderId);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		this->activeModelShaderId = shaderId;
		this->activeModelLoc = modelLoc;
	}

	void Renderer::DrawTextureCube(unsigned int shaderId, PrimitiveModel textureCube, glm::vec3 cubePositions[],
		int projLoc, int viewLoc, int modelLoc, glm::mat4 projection, glm::mat4 view)
	{
		glUseProgram(shaderId);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glBindVertexArray(textureCube.VAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	void Renderer::DrawLamp(unsigned int shaderId, PrimitiveModel lightCube, unsigned int lightModelLoc,
							unsigned int lightProjLoc, unsigned int lightViewLoc, glm::mat4 projection,
							glm::mat4 view, glm::vec3 lampPos)
	{
		glUseProgram(shaderId);
		glUniformMatrix4fv(lightProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(lightViewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lampPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(lightCube.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void Renderer::DrawTextureNormalCube(unsigned int shaderId, PrimitiveModel textureCube, glm::vec3 cubePositions[],
		int projLoc, int viewLoc, int modelLoc, glm::mat4 projection,
		glm::mat4 view, glm::vec3 cameraPos, glm::vec3 lightPos, unsigned int specularMap, unsigned int diffuseMap)
	{
		glUseProgram(shaderId);
		glUniform3fv(glGetUniformLocation(shaderId, "light.position"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(shaderId, "viewPos"), 1, &cameraPos[0]);

		glUniform3f(glGetUniformLocation(shaderId, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(shaderId, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(shaderId, "light.specular"), 1.0f, 1.0f, 1.0f);

		glUniform1f(glGetUniformLocation(shaderId, "material.shininess"), 64.0f);

		// set the projection and view from the camera
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		for (unsigned int i = 0; i < 10; ++i)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			// now bind the diffuse and specular maps
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap);

			glBindVertexArray(textureCube.VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);

		}
		// set back to defaults the active texture is now 0 again
		glActiveTexture(GL_TEXTURE0);
	}


	void Renderer::Shutdown()
	{
		delete ::finnsie::g_resourceManager;
	}

	void Renderer::initShaders()
	{
		// Need a shader for models
		// TODO(CK): The game probably should pass the shaders to the renderer
		// the renderer shouldn't have the shaders init inside because the shaders
		// could be different...
		this->modelShader = ::finnsie::g_resourceManager->GenerateShader(001, "shaders/vert_model.glsl", "shaders/frag_model.glsl", NULL);
		this->normalShader = ::finnsie::g_resourceManager->GenerateShader(002, "shaders/vert_normal_model.glsl", "shaders/frag_normal_model.glsl", "shaders/geo_normal_model.glsl");
	}

	void Renderer::initUniforms()
	{
		// uniforms
		this->objProjLoc = glGetUniformLocation(modelShader.id, "projection");
		this->objViewLoc = glGetUniformLocation(modelShader.id, "view");
		this->objModelLoc = glGetUniformLocation(modelShader.id, "model");

		this->normalProjLoc = glGetUniformLocation(normalShader.id, "projection");
		this->normalViewLoc = glGetUniformLocation(normalShader.id, "view");
		this->normalModelLoc = glGetUniformLocation(normalShader.id, "model");
	}
}