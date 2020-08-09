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
        
		this->drawingNormals = false;
        
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
    
    void FirstPass() 
    {
        
    }
    
	void Renderer::DrawModel(GameObject& obj, Shader modelShader, Shader normalShader)
	{
        // TODO(CK): Change this remove start shader function remove these locations
        // the locations are inside of the shaders we don't need to store them here or 
        // in the game
        // probably get rid of the normal shader.. ???? do we really need it? 
        // or can i just combine that technique inside of the modelshader and 
        // just chnage the way i draw off of a bool uniform?? ya do that
        
		
        // TODO(CK): We don't do this setting it here we do a function at the beginning
        // that does a pass and grabs the shader we are going to use
        // we could do:
        
        /*
		Shader *drawShader = BasePass(); <- returns a Shader*

		Shader* BasePass(*obj)
		{
		 if (obj.drawNormals)
		{
		 return shaders[1];
		}
		normal shader in here maybe? i still like game passing shaders
			 that was a shader can be made on the game side
		at the same time the renderer can just have its own shaders like before
		if its functional though how does it "keep" shaders is there just an array that lives
		in here and stays updated
		}


		then if we have a pointer to the shader we want to use


		also we can loop through the shader and activate all of its attributes maybe
		not the time and lighting (right now) we can also just say if (time exists then set it)

		the Shader uniforms needs a char *name .. it does have a string 
*/

		// make function or move to "First pass"
		if (!drawingNormals)
		{
			glUseProgram(modelShader.id);
			glUniformMatrix4fv(GetLoc(&modelShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(GetLoc(&modelShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
			this->activeModelShaderId = modelShader.id;
			this->activeModelLoc = GetLoc(&modelShader, "model");
		}
		else
		{
			glUseProgram(normalShader.id);
			glUniformMatrix4fv(GetLoc(&normalShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(GetLoc(&normalShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
			this->activeModelShaderId = normalShader.id;
			this->activeModelLoc = GetLoc(&normalShader, "model");

		}

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
				glUniform1i(glGetUniformLocation(activeModelShaderId, (name + number).c_str()), j);
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
			glUniformMatrix4fv(activeModelLoc, 1, GL_FALSE, glm::value_ptr(matModel));
            
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
        
		// TODO(CK): Not sure if this is proper. Look up how to do this properly
		// maybe move this logic into the game again.. if (drawNormals then call with the shader id
		if (obj.viewNormals && !drawingNormals)
		{
			// Normals are only drawn on the next draw call
			drawingNormals = true;
			DrawModel(obj, modelShader, normalShader);
			drawingNormals = false;
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
		//startShader(waterShader.id, watModelLoc, watProjLoc, watViewLoc);
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
    
	void Renderer::DrawTerrain(Terrain* terr, Shader* shader)
	{
		glUseProgram(shader->id);

		glUniformMatrix4fv(GetLoc(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(GetLoc(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glUniform3fv(GetLoc(shader, "lightPos"), 1, &g_lamp[0]);
		glUniform3fv(GetLoc(shader, "viewPos"), 1, &camPos[0]);

		// now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader->id, "texture_diffuse1"), 0);
		// and finally bind the texture		
		glBindTexture(GL_TEXTURE_2D, terr->textureId);

		glm::mat4 matModel = glm::mat4(1.0f);

		glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
												glm::vec3(terr->x - 100.0f, 0.0f, terr->z - 30.0));
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
		glUniformMatrix4fv(GetLoc(shader, "model"), 1, GL_FALSE, glm::value_ptr(matModel));

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
        
}