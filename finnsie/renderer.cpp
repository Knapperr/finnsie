#include "renderer.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <fstream>

namespace finnsie {
    
	Renderer::Renderer()
	{
		this->VBO = -1;
		this->cubeVAO = -1;
		this->lampVAO = -1;
		this->activeModelLoc = -1;
		this->activeModelShaderId = -1;
		this->camPos = glm::vec3(1.0f);
		
		this->lampPos = glm::vec3(1.2f, 1.0f, 2.0f);
		this->projection = glm::mat4(1.0f);
		this->view = glm::mat4(1.0f);
        
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
    
	void Renderer::BeginRender(Camera& cam)
	{
		this->projection = glm::perspective(glm::radians(cam.Zoom),
                                            (float)1080 / (float)720, 
											1.0f, 1000.0f); // NOTE(CK): near and far clipping distance
		this->view = cam.GetViewMatrix();
		this->camPos = cam.Position;
	}
    
	void Draw(GameObject* obj)
	{
		//glUseProgram(shader.id);

		// TODO(CK): Go through each shaders uniform and activate
		// Use the shader directly that way if a different shader 
		// is used the uniforms will be called.. could have some
		// sort of activate uniform call that can be overriden
		// and tweaked if a special shader is used
		// there has to be a way i can call like:
		/*

			IMPORTANT(ck): Oct 14 2020
			THIS MIGHT BE BETTER FOR THE SHADER TO HANDLE IT
			ACTIVATE UNIFORMS OR SOMETHING THAT YOU PASS
			THE SHADER TOO AND IT IS ALL DEALT WITH IN THERE
			INSTEAD OF DOING IT IN THIS FUNCTION

			uniform == gameobject attribute
			glUniformMatrix4fv ()
			if you have a uniform type attached
			to it you can figure out which glUniform too call
			Shader could have a function that is:
				
				Uniform(&shader);

				this call has a switch in it
				switch (uniforms[i].type)
				{
					"float" 
					"int" - glUniform1i();
					"matrix" - glUniformMatrix4fv
					"texture" - glUniform1i(glGetUniformLocation(modelShader.id, (name + number).c_str()), j);
				}
		
			There might be an easier way to do this im not sure..
			probably better to pass a matching struct from shader to c++ 
		*/
	}

	void Renderer::DrawModel(GameObject& obj, Shader modelShader)
	{
		glUseProgram(modelShader.id);
		glUniformMatrix4fv(GetLoc(&modelShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(GetLoc(&modelShader, "view"), 1, GL_FALSE, glm::value_ptr(view));

		for (unsigned int i = 0; i < obj.model->meshes.size(); i++)
		{
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
            
			for (unsigned int j = 0; j < obj.model->meshes[i].textures.size(); ++j)
			{
				glActiveTexture(GL_TEXTURE0 + j); // activate the proper texture unit before binding
				// retrieve texture number (the N in diffuse_TextureN)
				std::string number;
				std::string name = obj.model->meshes[i].textures[j].type;
                
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);
				else if (name == "texture_normal")
					number = std::to_string(normalNr++);
				else if (name == "texture_height")
					number = std::to_string(heightNr++);
                
				// now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(modelShader.id, (name + number).c_str()), j);
				// and finally bind the texture
				glBindTexture(GL_TEXTURE_2D, obj.model->meshes[i].textures[j].id);
			}
			
			// Set position, rotation and scale
			glm::mat4 matModel = glm::mat4(1.0f);
            
			glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
													glm::vec3(obj.pos.x, obj.pos.y, obj.pos.z));
			matModel = matModel * matTranslate;
            
			glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
											obj.orientation.z,
											glm::vec3(0.0f, 0.0f, 1.0f));
			matModel = matModel * rotateZ;
            
			glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
											obj.orientation.y,
											glm::vec3(0.0f, 1.0f, 0.0f));
			matModel = matModel * rotateY;
            
			glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
											obj.orientation.x,
											glm::vec3(1.0f, 0.0f, 0.0f));
			matModel = matModel * rotateX;
            
			glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
                                            glm::vec3(obj.scale, obj.scale, obj.scale));
            
			matModel = matModel * matScale;
			glUniformMatrix4fv(GetLoc(&modelShader, "model"), 1, GL_FALSE, glm::value_ptr(matModel));
            
			/*
			// INVERSE WAS FROM GRAPHICS CLASS
				GLint matModel_loc = glGetUniformLocation(shaderProgID, "matModel");
				GLint matModelInvTran_loc = glGetUniformLocation(shaderProgID, "matModelInvTrans");
			*/
            
			if (obj.wireFrame)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
            
			// Draw mesh
			glBindVertexArray(obj.model->meshes[i].VAO);
			glDrawElements(GL_TRIANGLES, obj.model->meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
            
			// Always good practice to set everything back to defaults once configured
			// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
    
    void Renderer::DrawSphere(GameObject& obj, Shader binnShader)
    {
		glUseProgram(binnShader.id);
		glUniformMatrix4fv(GetLoc(&binnShader, "projection"),
                           1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(GetLoc(&binnShader, "view"),
                           1, GL_FALSE, glm::value_ptr(view));
        
		for (unsigned int i = 0; i < obj.model->meshes.size(); i++)
		{
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
            
			for (unsigned int j = 0; j < obj.model->meshes[i].textures.size(); ++j)
			{
				glActiveTexture(GL_TEXTURE0 + j); // activate the proper texture unit before binding
				// retrieve texture number (the N in diffuse_TextureN)
				std::string number;
				std::string name = obj.model->meshes[i].textures[j].type;
                
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);
				else if (name == "texture_normal")
					number = std::to_string(normalNr++);
				else if (name == "texture_height")
					number = std::to_string(heightNr++);
                
				// now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(binnShader.id,
                                                 (name + number).c_str()), j);
				// and finally bind the texture
				glBindTexture(GL_TEXTURE_2D, obj.model->meshes[i].textures[j].id);
			}
            
            glUniform1f(glGetUniformLocation(binnShader.id, "time"), (float)glfwGetTime());
            
            glUniform3fv(glGetUniformLocation(binnShader.id, "lightPos"), 1, &g_lamp[0]);
            glUniform3fv(glGetUniformLocation(binnShader.id, "viewPos"), 1, &camPos[0]);
			
			// Set position, rotation and scale
			glm::mat4 matModel = glm::mat4(1.0f);
            
			glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
													glm::vec3(obj.pos.x, obj.pos.y, obj.pos.z));
			matModel = matModel * matTranslate;
            
			glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
											obj.orientation.z,
											glm::vec3(0.0f, 0.0f, 1.0f));
			matModel = matModel * rotateZ;
            
			glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
											obj.orientation.y,
											glm::vec3(0.0f, 1.0f, 0.0f));
			matModel = matModel * rotateY;
            
			glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
											obj.orientation.x,
											glm::vec3(1.0f, 0.0f, 0.0f));
			matModel = matModel * rotateX;
            
			glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
                                            glm::vec3(obj.scale, obj.scale, obj.scale));
            
			matModel = matModel * matScale;
			glUniformMatrix4fv(GetLoc(&binnShader, "model"),
                               1, GL_FALSE, glm::value_ptr(matModel));
            
			/*
			// INVERSE WAS FROM GRAPHICS CLASS
				GLint matModel_loc = glGetUniformLocation(shaderProgID, "matModel");
				GLint matModelInvTran_loc = glGetUniformLocation(shaderProgID, "matModelInvTrans");
			*/
            
			if (obj.wireFrame)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
            
			// Draw mesh
			glBindVertexArray(obj.model->meshes[i].VAO);
			glDrawElements(GL_TRIANGLES, obj.model->meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
            
			// Always good practice to set everything back to defaults once configured
			// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
    }
    
	void Renderer::DrawWater(WaterObject* water, Shader waterShader)
	{
		glUseProgram(waterShader.id);
		glUniformMatrix4fv(GetLoc(&waterShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(GetLoc(&waterShader, "view"), 1, GL_FALSE, glm::value_ptr(view));

		for (unsigned int i = 0; i < water->model->meshes.size(); i++)
		{
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
            
			for (unsigned int j = 0; j < water->model->meshes[i].textures.size(); ++j)
			{
				glActiveTexture(GL_TEXTURE0 + j); // activate the proper texture unit before binding
				// retrieve texture number (the N in diffuse_TextureN)
				std::string number;
				std::string name = water->model->meshes[i].textures[j].type;
                
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
				glBindTexture(GL_TEXTURE_2D, water->model->meshes[i].textures[j].id);
			}
            
            
			// loop through draw info grabbed passed to gui from game. then game passes to renderer
			// can loop through all uniforms with shader.GetUniforms();
			glUniform1f(GetLoc(&waterShader, "time"), (float)glfwGetTime());
			glUniform3fv(GetLoc(&waterShader, "lightPos"), 1, &g_lamp[0]);
			glUniform3fv(GetLoc(&waterShader, "viewPos"), 1, &camPos[0]);
			
			// TODO(CK): Draw info gets moved to model
			glUniform1f(GetLoc(&waterShader, "uJump"), water->uJump);
			glUniform1f(GetLoc(&waterShader, "vJump"), water->vJump);
			glUniform1f(GetLoc(&waterShader, "tiling"), water->tiling);
			glUniform1f(GetLoc(&waterShader, "speed"), water->speed);
			glUniform1f(GetLoc(&waterShader, "flowStrength"), water->flowStrength);
			glUniform1f(GetLoc(&waterShader, "flowOffset"), water->flowOffset);
			glUniform1f(GetLoc(&waterShader, "heightScale"), water->heightScale);
			glUniform1f(GetLoc(&waterShader, "heightScaleModulated"), water->heightScaleModulated);
			
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
			glUniformMatrix4fv(GetLoc(&waterShader, "model"),
							   1, GL_FALSE, glm::value_ptr(matModel));
            
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
			glBindVertexArray(water->model->meshes[i].VAO);
			glDrawElements(GL_TRIANGLES, water->model->meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
            
			// Always good practice to set everything back to defaults once configured
			// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		return;
	}
    
	void Renderer::DrawDirWater(WaterObject* water, Shader waterShader)
	{
		glUseProgram(waterShader.id);
		glUniformMatrix4fv(GetLoc(&waterShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(GetLoc(&waterShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        
		for (unsigned int i = 0; i < water->model->meshes.size(); i++)
		{
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
            
			for (unsigned int j = 0; j < water->model->meshes[i].textures.size(); ++j)
			{
				glActiveTexture(GL_TEXTURE0 + j); // activate the proper texture unit before binding
				// retrieve texture number (the N in diffuse_TextureN)
				std::string number;
				std::string name = water->model->meshes[i].textures[j].type;
                
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
				glBindTexture(GL_TEXTURE_2D, water->model->meshes[i].textures[j].id);
			}
                   
			glUniform1f(GetLoc(&waterShader, "time"), (float)glfwGetTime());
			glUniform3fv(GetLoc(&waterShader, "lightPos"), 1, &g_lamp[0]);
			glUniform3fv(GetLoc(&waterShader, "viewPos"), 1, &camPos[0]);
            
			glUniform1f(GetLoc(&waterShader, "tiling2"), water->tiling);
			glUniform1f(GetLoc(&waterShader,"tilingModulated"), water->tilingModulated);
			glUniform1f(GetLoc(&waterShader,"speed2"), water->speed);
			glUniform1f(GetLoc(&waterShader,"flowStrength2"), water->flowStrength);
			glUniform1f(GetLoc(&waterShader,"heightScale2"), water->heightScale);
			glUniform1f(GetLoc(&waterShader,"heightScaleModulated2"), water->heightScaleModulated);
			glUniform1f(GetLoc(&waterShader,"gridResolution"), water->gridResolution);
			glUniform1f(GetLoc(&waterShader,"dualGrid"), water->dualGrid);
            
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
			glUniformMatrix4fv(GetLoc(&waterShader, "model"),
                               1, GL_FALSE, glm::value_ptr(matModel));
            
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
			glBindVertexArray(water->model->meshes[i].VAO);
			glDrawElements(GL_TRIANGLES, water->model->meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
            
			// Always good practice to set everything back to defaults once configured
			// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
			// because the active texture must be 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		return;
	}
	
	void Renderer::drawGrass(Terrain* terr)
	{
		glUseProgram(terr->grassShader->id);
		glUniformMatrix4fv(GetLoc(terr->grassShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(GetLoc(terr->grassShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		

		// now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(terr->grassShader->id, "texture_diffuse1"), 0);
		// and finally bind the texture		
		glBindTexture(GL_TEXTURE_2D, terr->grass.textureId);
		
		for (unsigned int i = 0; i < terr->grass.model.meshes.size(); ++i)
		{
			glBindVertexArray(terr->grass.model.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, terr->grass.model.meshes[i].indices.size(),
									GL_UNSIGNED_INT, 0, terr->grass.amount);
			glBindVertexArray(0);
		}
	}

	void Renderer::DrawTerrain(Terrain* terr)
	{
		glUseProgram(terr->shader->id);

		glUniformMatrix4fv(GetLoc(terr->shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(GetLoc(terr->shader, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glUniform3fv(GetLoc(terr->shader, "lightPos"), 1, &g_lamp[0]);
		glUniform3fv(GetLoc(terr->shader, "viewPos"), 1, &camPos[0]);

		// set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(terr->shader->id, "texture_diffuse1"), 0);
		if (terr->drawTexture)
		{
			glBindTexture(GL_TEXTURE_2D, terr->textureId);
		}
		glm::mat4 matModel = glm::mat4(1.0f);

		glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
												glm::vec3(terr->x, 0.0f, terr->z));
		matModel = matModel * matTranslate;

		glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
										0.0f,
										glm::vec3(0.0f, 0.0f, 1.0f));
		matModel = matModel * rotateZ;

		glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
										0.0f,
										glm::vec3(0.0f, 1.0f, 0.0f));
		matModel = matModel * rotateY;

		glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
										0.0f,
										glm::vec3(1.0f, 0.0f, 0.0f));
		matModel = matModel * rotateX;

		glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
										glm::vec3(1.0, 1.0, 1.0));

		matModel = matModel * matScale;
		
		
		glUniformMatrix4fv(GetLoc(terr->shader, "model"), 1, GL_FALSE, glm::value_ptr(matModel));

		// Wireframe
		//if (terr->wireFrame)
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//else
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(terr->VAO);

		terr->wireFrame ? 
			glDrawElements(GL_LINES, terr->indicesLength, GL_UNSIGNED_INT, 0) 
		  : glDrawElements(GL_TRIANGLES, terr->indicesLength, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		// Always good practice to set everything back to defaults once configured
		// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);


		drawGrass(terr);
	}

	void Renderer::EndRender()
	{
		return;
	}
    
    
	void Renderer::Shutdown()
	{
		return;
	}        
}