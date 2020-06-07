#include "renderer.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <fstream>

namespace finnsie {

	Renderer::Renderer()
	{
		uniformManager = new UniformManager();

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
		this->lightPos = glm::vec3(-138.843f, -19.247f, -14.226f);
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

		// TODO(CK): Not sure if this is proper. Look up how to do this properly
		// maybe move this logic into the game again.. if (drawNormals then call with the shader id
		if (model.viewNormals && !drawingNormals)
		{
			// Normals are only drawn on the next draw call
			drawingNormals = true;
			DrawModel(model);
			drawingNormals = false;
		}
	}

	void Renderer::DrawWater(Model* water, draw_info& drawInfo)
	{
		startShader(waterShader.id, watModelLoc, watProjLoc, watViewLoc);

		for (unsigned int i = 0; i < water->meshes.size(); i++)
		{
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;

			for (unsigned int j = 0; j < water->meshes[i].textures.size(); ++j)
			{
				glActiveTexture(GL_TEXTURE0 + j); // activate the proper texture unit before binding
				// retrieve texture number (the N in diffuse_TextureN)
				std::string number;
				std::string name = water->meshes[i].textures[j].type;

				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);
				else if (name == "texture_normal")
					number = std::to_string(normalNr++);
				else if (name == "texture_height")
					number = std::to_string(heightNr++);

				// now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(waterShader.id, (name + number).c_str()), j);
				// and finally bind the texture		
				glBindTexture(GL_TEXTURE_2D, water->meshes[i].textures[j].id);
			}

			// TODO(CK): CLEAN UP
			// Water distortion
			// --------------------

			glUniform1f(glGetUniformLocation(waterShader.id, "time"), glfwGetTime());

			// TODO(CK): Clean this up
			this->lightPos = glm::vec3(drawInfo.lightInfo.lightX, drawInfo.lightInfo.lightY, drawInfo.lightInfo.lightZ);
			
			// loop through draw info grabbed passed to gui from game. then game passes to renderer
			// can loop through all uniforms with shader.GetUniforms();
			glUniform3fv(glGetUniformLocation(waterShader.id, "lightPos"), 1, &lightPos[0]);
			glUniform3fv(glGetUniformLocation(waterShader.id, "viewPos"), 1, &camPos[0]); // getting updated in BeginRender (probably not good)

			glUniform1f(this->uniformManager->GetLocation("uJump"), drawInfo.waterInfo.uJump);
			glUniform1f(this->uniformManager->GetLocation("vJump"), drawInfo.waterInfo.vJump);
			glUniform1f(this->uniformManager->GetLocation("tiling"), drawInfo.waterInfo.tiling);
			glUniform1f(this->uniformManager->GetLocation("speed"), drawInfo.waterInfo.speed);
			glUniform1f(this->uniformManager->GetLocation("flowStrength"), drawInfo.waterInfo.flowStrength);
			glUniform1f(this->uniformManager->GetLocation("flowOffset"), drawInfo.waterInfo.flowOffset);
			glUniform1f(this->uniformManager->GetLocation("heightScale"), drawInfo.waterInfo.heightScale);
			glUniform1f(this->uniformManager->GetLocation("heightScaleModulated"), drawInfo.waterInfo.heightScaleModulated);

			// Set position, rotation and scale
			glm::mat4 matModel = glm::mat4(1.0f);

			glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
													glm::vec3(water->pos.x, water->pos.y, water->pos.z));
			matModel = matModel * matTranslate;

			glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
											water->orientation.z,
											glm::vec3(0.0f, 0.0f, 1.0f));
			matModel = matModel * rotateZ;

			glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
											water->orientation.y,
											glm::vec3(0.0f, 1.0f, 0.0f));
			matModel = matModel * rotateY;

			glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
											water->orientation.x,
											glm::vec3(1.0f, 0.0f, 0.0f));
			matModel = matModel * rotateX;

			glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
											glm::vec3(water->scale, water->scale, water->scale));

			matModel = matModel * matScale;
			glUniformMatrix4fv(watModelLoc, 1, GL_FALSE, glm::value_ptr(matModel));

			/*
			// INVERSE WAS FROM GRAPHICS CLASS
				GLint matModel_loc = glGetUniformLocation(shaderProgID, "matModel");
				GLint matModelInvTran_loc = glGetUniformLocation(shaderProgID, "matModelInvTrans");
			*/

			if (water->wireFrame)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			// Draw mesh
			glBindVertexArray(water->meshes[i].VAO);
			glDrawElements(GL_TRIANGLES, water->meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// Always good practice to set everything back to defaults once configured
			// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		return;
	}

	void Renderer::DrawDirWater(Model& water, draw_info& drawInfo)
	{
		glUseProgram(waterDirShader.id);
		glUniformMatrix4fv(this->uniformManager->GetLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(this->uniformManager->GetLocation("view"), 1, GL_FALSE, glm::value_ptr(view));

		for (unsigned int i = 0; i < water.meshes.size(); i++)
		{
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;

			for (unsigned int j = 0; j < water.meshes[i].textures.size(); ++j)
			{
				glActiveTexture(GL_TEXTURE0 + j); // activate the proper texture unit before binding
				// retrieve texture number (the N in diffuse_TextureN)
				std::string number;
				std::string name = water.meshes[i].textures[j].type;

				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);
				else if (name == "texture_normal")
					number = std::to_string(normalNr++);
				else if (name == "texture_height")
					number = std::to_string(heightNr++);

				// now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(waterDirShader.id, (name + number).c_str()), j);
				// and finally bind the texture		
				glBindTexture(GL_TEXTURE_2D, water.meshes[i].textures[j].id);
			}

			// TODO(CK): CLEAN UP
			// Water direction
			// --------------------

			glUniform1f(glGetUniformLocation(waterDirShader.id, "time"), glfwGetTime());

			// TODO(CK): Clean this up
			this->lightPos = glm::vec3(drawInfo.lightInfo.lightX, drawInfo.lightInfo.lightY, drawInfo.lightInfo.lightZ);
			glUniform3fv(glGetUniformLocation(waterDirShader.id, "lightPos"), 1, &lightPos[0]);
			glUniform3fv(glGetUniformLocation(waterDirShader.id, "viewPos"), 1, &camPos[0]); // getting updated in BeginRender (probably not good)

			glUniform1f(this->uniformManager->GetLocation("tiling2"), drawInfo.waterInfo.tiling);
			glUniform1f(this->uniformManager->GetLocation("tilingModulated"), drawInfo.waterInfo.tilingModulated);
			glUniform1f(this->uniformManager->GetLocation("speed2"), drawInfo.waterInfo.speed);
			glUniform1f(this->uniformManager->GetLocation("flowStrength2"), drawInfo.waterInfo.flowStrength);
			glUniform1f(this->uniformManager->GetLocation("heightScale2"), drawInfo.waterInfo.heightScale);
			glUniform1f(this->uniformManager->GetLocation("heightScaleModulated2"), drawInfo.waterInfo.heightScaleModulated);
			glUniform1f(this->uniformManager->GetLocation("gridResolution"), drawInfo.waterInfo.gridResolution);
			glUniform1f(this->uniformManager->GetLocation("dualGrid"), drawInfo.waterInfo.dualGrid);

			// Set position, rotation and scale
			glm::mat4 matModel = glm::mat4(1.0f);

			glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
													glm::vec3(water.pos.x, water.pos.y, water.pos.z));
			matModel = matModel * matTranslate;

			glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
											water.orientation.z,
											glm::vec3(0.0f, 0.0f, 1.0f));
			matModel = matModel * rotateZ;

			glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
											water.orientation.y,
											glm::vec3(0.0f, 1.0f, 0.0f));
			matModel = matModel * rotateY;

			glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
											water.orientation.x,
											glm::vec3(1.0f, 0.0f, 0.0f));
			matModel = matModel * rotateX;

			glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
											glm::vec3(water.scale, water.scale, water.scale));

			matModel = matModel * matScale;
			glUniformMatrix4fv(this->uniformManager->GetLocation("model"), 1, GL_FALSE, glm::value_ptr(matModel));

			/*
			// INVERSE WAS FROM GRAPHICS CLASS
				GLint matModel_loc = glGetUniformLocation(shaderProgID, "matModel");
				GLint matModelInvTran_loc = glGetUniformLocation(shaderProgID, "matModelInvTrans");
			*/

			if (water.wireFrame)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			// Draw mesh
			glBindVertexArray(water.meshes[i].VAO);
			glDrawElements(GL_TRIANGLES, water.meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// Always good practice to set everything back to defaults once configured
			// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
			// because the active texture must be 0
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

	void Renderer::Shutdown()
	{
		delete ::finnsie::g_resourceManager;
	}

	void Renderer::initShaders()
	{
		// Need a shader for models
		// TODO(CK): The game probably should pass the shaders to the renderer???
		// the renderer shouldn't have the shaders in it inside because the shaders
		// could be different...
		// or does the renderer hold a vector of shaders that will be chosen by the render functions
		this->modelShader = ::finnsie::g_resourceManager->GenerateShader(001, "shaders/model_vert.glsl", "shaders/model_frag.glsl", NULL);
		this->normalShader = ::finnsie::g_resourceManager->GenerateShader(002, "shaders/onlynormals_model_vert.glsl", "shaders/onlynormals_model_frag.glsl", "shaders/onlynormals_model_geo.glsl");
		this->waterShader = ::finnsie::g_resourceManager->GenerateShader(003, "shaders/waterdistortion_vert.glsl", "shaders/waterdistortion_frag.glsl", NULL);
		waterShader.PrintUniforms();
		
		this->waterDirShader = ::finnsie::g_resourceManager->GenerateShader(004, "shaders/waterdirection_vert.glsl", "shaders/waterdirection_frag.glsl", NULL);
		waterDirShader.PrintUniforms();
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

		// FLAW IN MANAGER... IF TWO SHADERS HAVE THE SAME NAME CAN NOT RETRIEVE

		this->watProjLoc = glGetUniformLocation(waterShader.id, "projection");
		this->watViewLoc = glGetUniformLocation(waterShader.id, "view");
		this->watModelLoc = glGetUniformLocation(waterShader.id, "model");



		this->uniformManager->CreateUniform("uJump", waterShader.id);
		this->uniformManager->CreateUniform("vJump", waterShader.id);
		this->uniformManager->CreateUniform("tiling", waterShader.id);
		this->uniformManager->CreateUniform("speed", waterShader.id);
		this->uniformManager->CreateUniform("flowStrength", waterShader.id);
		this->uniformManager->CreateUniform("flowOffset", waterShader.id);
		this->uniformManager->CreateUniform("heightScale", waterShader.id);
		this->uniformManager->CreateUniform("heightScaleModulated", waterShader.id);

		
		this->uniformManager->CreateUniform("projection",waterDirShader.id);
		this->uniformManager->CreateUniform("view", waterDirShader.id);
		this->uniformManager->CreateUniform("model", waterDirShader.id);
		this->uniformManager->CreateUniform("tiling2", waterDirShader.id);
		this->uniformManager->CreateUniform("tilingModulated", waterDirShader.id);
		this->uniformManager->CreateUniform("speed2", waterDirShader.id);
		this->uniformManager->CreateUniform("flowStrength2", waterDirShader.id);
		this->uniformManager->CreateUniform("gridResolution", waterDirShader.id);
		this->uniformManager->CreateUniform("heightScale2", waterDirShader.id);
		this->uniformManager->CreateUniform("heightScaleModulated2", waterDirShader.id);
		this->uniformManager->CreateUniform("dualGrid", waterDirShader.id);
	}

}