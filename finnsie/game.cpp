#include "game.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <time.h>

namespace finnsie {

    // Game inits resource manager
    ResourceManager* g_resourceManager;

    // TODO(CK): Figure out lights in game
    // keep in their own vector for awhile?
    glm::vec3 g_lamp;


    // Terrain
    // ================================================================
#define SIZE 800
#define VERTEX_COUNT 128
    Terrain::Terrain()
    {
        this->x = 0;
        this->z = 0;

        this->vao = 0;
        this->indicesLength = 0;
        this->textureId = 0;
    }

    Terrain::Terrain(int gridX, int gridZ)
    {
        this->x = gridX * SIZE;
        this->z = gridZ * SIZE;

        this->vao = 0;
        this->indicesLength = 0;
        this->textureId = 0;


        // Load a texture 
        std::string textPath = "content/textures/terr/grass.jpg";
        std::string textDir = textPath.substr(0, textPath.find_last_of('/'));
        textureId = LoadTextureFile("grass.jpg", textDir, false);
    }

    // TODO(CK):
    // Look at Brackeys: https://www.youtube.com/watch?v=64NblGkAabk&t=4s
    // this is probably a more sane way to generate and it will
    // actually teach you
    struct Vert
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

    // https://en.wikipedia.org/wiki/Perlin_noise#:~:text=Perlin%20noise%20is%20a%20procedural,details%20are%20the%20same%20size.
    float lerp(float a0, float a1, float w)
    {
        return (1.0f - w) * a0 + w * a1;
    }

    // Computes the dot product of the distance and gradient vectors.
    //float dotGridGradient(int ix, int iy, float x, float y) 
    //{

    //    // Precomputed (or otherwise) gradient vectors at each grid node
    //    //extern float Gradient[IYMAX][IXMAX][2];

    //    // Compute the distance vector
    //    float dx = x - (float)ix;
    //    float dy = y - (float)iy;

    //    // Compute the dot-product
    //    return (dx * Gradient[iy][ix][0] + dy * Gradient[iy][ix][1]);
    //}

    //float perlin(float x, float z)
    //{
    //    // Determine grid cell coordinates
    //    int x0 = (int)x;
    //    int x1 = x0 + 1;
    //    int y0 = (int)y;
    //    int y1 = y0 + 1;

    //    // Determine interpolation weights
    //    // Could also use higher order polynomial/s-curve here
    //    float sx = x - (float)x0;
    //    float sy = y - (float)y0;

    //    // Interpolate between grid point gradients
    //    float n0, n1, ix0, ix1, value;

    //    n0 = dotGridGradient(x0, y0, x, y);
    //    n1 = dotGridGradient(x1, y0, x, y);
    //    ix0 = lerp(n0, n1, sx);

    //    n0 = dotGridGradient(x0, y1, x, y);
    //    n1 = dotGridGradient(x1, y1, x, y);
    //    ix1 = lerp(n0, n1, sx);

    //    value = lerp(ix0, ix1, sy);
    //    return value;
    //}

    void Terrain::Generate()
    {
        const int count = VERTEX_COUNT * VERTEX_COUNT;
        float* vertices = new float[count * 3];
        float* normals = new float[count * 3];
        float* textureCoords = new float[count * 2];
        
        int* indices = new int[6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1)];
        
        // Get array sizes for rendering
        int verticesLength = count * 3;
        int textureLength = count * 2;
        this->indicesLength = 6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1);
        int index = 0;
        for (int i = 0; i < VERTEX_COUNT; ++i)
        {
            for (int j = 0; j < VERTEX_COUNT; ++j)
            {
                vertices[index * 3] = (float)j / ((float)VERTEX_COUNT - 1) * SIZE; // x 
                //vertices[index * 3 + 1] = 0);
                int rnum = rand() % 50 + 10;
                vertices[index * 3 + 1] = cosf(rnum) * 2.0f;
                vertices[index * 3 + 2] = (float)i / ((float)VERTEX_COUNT - 1) * SIZE; // z

                normals[index * 3] = 0;
                normals[index * 3 + 1] = 1;
                normals[index * 3 + 2] = 0;

                textureCoords[index * 2] = (float)j / ((float)VERTEX_COUNT - 1);
                textureCoords[index * 2 + 1] = (float)i / ((float)VERTEX_COUNT - 1);
                ++index;
            }
        }
        index = 0;
        for (int gz = 0; gz < VERTEX_COUNT - 1; ++gz)
        {
            for (int gx = 0; gx < VERTEX_COUNT - 1; ++gx)
            {
                int topLeft = (gz * VERTEX_COUNT) + gx;
                int topRight = topLeft + 1;
                int bottomLeft = ((gz + 1) * VERTEX_COUNT) + gx;
                int bottomRight = bottomLeft + 1;

                indices[index++] = topLeft;
                indices[index++] = bottomLeft;
                indices[index++] = topRight;
                indices[index++] = topRight;
                indices[index++] = bottomLeft;
                indices[index++] = bottomRight;
            }
        }
        unsigned int vbo;
        unsigned int nbo; // normal buffer
        unsigned int tbo; // texture buffer
        unsigned int ebo;

        glGenVertexArrays(1, &this->vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glGenBuffers(1, &nbo);
        glGenBuffers(1, &tbo);

        // TODO(CK):
        // Having all of these buffers is probably bad
        // I am sure you could easily just use 1 array and fill it up with all of this data
        // or just use a vector with the Vert structure for the data that is the proper way...
        // REFACTOR

        glBindVertexArray(this->vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verticesLength * sizeof(float), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(int), indices, GL_STATIC_DRAW);


        // TODO(CK) & STUDY(CK): I forgot 3 * sizeof(float) in the last parameter 
        // I need to make sure that I study glBufferData and glVertexAttribPointer I think because 
        // it was missing the stride it wasn't formating the data correctly which is why it looked
        // like a mountain
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


        // THIS IS ALL WRONG ... get this working properly
        // need to figure out the strides...
        // ---------------------------------------------------
        // Vertex normals
        glBindBuffer(GL_ARRAY_BUFFER, nbo);
        glBufferData(GL_ARRAY_BUFFER, verticesLength * sizeof(float), normals, GL_STATIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(3 * sizeof(float)));
        // -------------------------

        // Vertex texture coords
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, textureLength * sizeof(float), textureCoords, GL_STATIC_DRAW);

        glEnableVertexAttribArray(2);
        // STUDY(CK): For some reason at the very end of this (void*)(6 * sizeof(float))); 
        // was causing the texcoords to distort.. this is probably why you are meant to have
        // ONE BUFFER bound instead 
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        // --------------------------

        // unbind & clean up
        glBindVertexArray(0);

        delete[] vertices;
        delete[] normals;
        delete[] textureCoords;
        delete[] indices;
    }

    void Terrain::Render(Shader *modelShader, Camera *cam)
    {
        glUseProgram(modelShader->id);
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)1080 / (float)720, 1.0f, 1000.0f);
        glUniformMatrix4fv(GetLoc(modelShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(GetLoc(modelShader, "view"), 1, GL_FALSE, glm::value_ptr(cam->GetViewMatrix()));

        glUniform3fv(GetLoc(modelShader, "lightPos"), 1, &g_lamp[0]);
        glUniform3fv(GetLoc(modelShader, "viewPos"), 1, &cam->Position[0]);

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(modelShader->id, "texture_diffuse1"), 0);
        // and finally bind the texture		
        glBindTexture(GL_TEXTURE_2D, this->textureId);

        glm::mat4 matModel = glm::mat4(1.0f);

        glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
                                                glm::vec3(this->x - 100.0f, -35.0f, this->z - 30.0));
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
                                        glm::vec3(0.1, 0.1, 0.1));

        matModel = matModel * matScale;
        glUniformMatrix4fv(GetLoc(modelShader, "model"), 1, GL_FALSE, glm::value_ptr(matModel));

         
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(this->vao);
        glDrawElements(GL_TRIANGLES, this->indicesLength, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Always good practice to set everything back to defaults once configured
        // NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // ================================================================

	Game::Game(GLFWwindow& wnd)
	{
		LOG("Game Init");
        
		::finnsie::g_resourceManager = new ResourceManager();
        g_lamp = glm::vec3(1.0f);
        g_lamp.x = -148.0f;
        g_lamp.y = -13.38f;

		this->mode = Mode::EDIT;
		this->window = &wnd;
		this->gui = new Gui();
		this->renderer = new Renderer();
		this->camera = new Camera();

		this->leftMousePressed = false;
        this->debugRightMousePressed = false;
		this->debugSphereStopped = false;
		
        // Water objects
		distortWater = new WaterObject("water",
                                       glm::vec3(-100.0f, -30.0f, 0.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f),
                                       40.0f,
                                       "content/objects/quad/basic_quad.obj");
        distortWater->tiling = 5.0f;
        distortWater->speed = 0.3f;
        distortWater->flowStrength = 0.05f;
        distortWater->flowOffset = -0.23f;
        distortWater->heightScale = 0.1f;
        distortWater->heightScaleModulated = 8.0f;
		LoadDistortedWater(distortWater->model);
        
		dirWater = new WaterObject("water",
                                   glm::vec3(-100.0f, -30.0f, 80.0f),
                                   glm::vec3(0.0f, glm::radians(180.0f), 0.0f),
                                   40.0f,
                                   "content/objects/quad/basic_quad.obj");
        dirWater->tiling = 10.0f;
        dirWater->speed = 0.2f;
        dirWater->flowStrength = 0.07f;
        dirWater->heightScale = 0.92f;
        dirWater->heightScaleModulated = 9.0f;
        dirWater->gridResolution = 30.0f;
        dirWater->tilingModulated = 50.0f;
        dirWater->dualGrid = true;
		LoadDirectionalWater(dirWater->model);
        
        // Test Sphere for shader learning 
        testSphere = new GameObject("sphere",
                                    glm::vec3(-100.0f, 100.0f, 80.0),
                                    glm::vec3(0.0f, 0.0f, 0.0f),
                                    10.0f,
                                    "content/objects/sphere/sphere.obj");
        std::string diffusePath = "content/textures/wall.jpg";
        std::string diffuseDir = diffusePath.substr(0, diffusePath.find_last_of('/'));
        Texture text = {};
        text.id = LoadTextureFile("wall.jpg", diffuseDir, false);
        text.type = "texture_diffuse";
        text.path = diffusePath;
        testSphere->model->meshes[0].textures.push_back(text);
        
        // TODO(CK): Remove book... put into legacy folder or something?
        //book = new GameObject("book",
        //                      glm::vec3(-100.0f, -25.0f, 40.0),
        //                      glm::vec3(0.0f, glm::radians(180.0f), 0.0f),
        //                      8.0f,
        //                      "content/objects/thickpage/thick_page.obj");
        //std::string diffusePath1 = "content/textures/jeff/page1.jpg";
        //std::string diffuseDir1 = diffusePath1.substr(0, diffusePath1.find_last_of('/'));
        //Texture text1 = {};
        //text1.id = LoadTextureFile("page2.jpg", diffuseDir1, false);
        //text1.type = "texture_diffuse";
        //text1.path = diffusePath1;
        //book->model->meshes[0].textures.push_back(text1);
        
        
        
        
        // Shaders 
		this->modelShader = ::finnsie::g_resourceManager->GenerateShader(001, "shaders/model_vert.glsl", "shaders/model_frag.glsl", NULL);
		this->normalShader = ::finnsie::g_resourceManager->GenerateShader(002, "shaders/onlynormals_model_vert.glsl", "shaders/onlynormals_model_frag.glsl", "shaders/onlynormals_model_geo.glsl");
		this->waterShader = ::finnsie::g_resourceManager->GenerateShader(003, "shaders/waterdistortion_vert.glsl", "shaders/waterdistortion_frag.glsl", NULL);
		this->waterDirShader = ::finnsie::g_resourceManager->GenerateShader(004, "shaders/waterdirection_vert.glsl", "shaders/waterdirection_frag.glsl", NULL);
        
        this->binnShader = 
            ::finnsie::g_resourceManager->GenerateShader(005,
                                                         "shaders/blinnphong_vert.glsl",
                                                         "shaders/blinnphong_frag.glsl",
                                                         NULL);
        
        
        
        this->gui->Init(*this->window, camera->MovementSpeed);

        // TODO(CK): Remove this seeding rand
        srand(time(NULL));
        this->terrain = Terrain(0, 0);
        this->terrain.Generate();

	}
    
    // TODO(CK): pass the input from main to here
	void Game::Update(float dt)
	{
        // TODO(CK): Pass water for now... find a way to update
        // the game data directly... get lighting info directly too
		gui->Update(*distortWater, *dirWater);
        
		if (leftMousePressed)
		{
			processCamera(dt);
		}
        

        // TODO(CK): Move this out
        // apply gravity to sphere
        if (debugRightMousePressed && !debugSphereStopped)
        {
            /*glm::vec2 force = glm::vec2(0, testSphere->mass * -9.81f);
            glm::vec2 acceleration = glm::vec2(force.x / testSphere->mass, force.y / testSphere->mass);
            testSphere->velocity.x += acceleration.x * dt;
            testSphere->velocity.y += acceleration.y * dt;
            testSphere->pos.x += testSphere->velocity.x * dt;
            testSphere->pos.y += testSphere->velocity.y * dt;*/
            
            AddAcc(&testSphere->rb, dt);
            testSphere->pos.x += testSphere->rb.velocity.x * dt;
            testSphere->pos.y += testSphere->rb.velocity.y * dt;
            
            
        }
        
        // stop the sphere so it doesn't fall forever...
        if (testSphere->pos.y < -30.0f)
        {
            debugSphereStopped = true;
        }
        if (debugSphereStopped)
        {
            // stop gravity now allow ball to roll on this imaginary plane?
            //testSphere->rb.velocity += 
            //testSphere->pos.y = -testSphere->pos.y;
            //testSphere->rb.velocity.y = -testSphere->rb.velocity.y;
        }
	}
    
	void Game::Render()
	{
		// TODO(CK): Use game camera (can switch to gameCamera here)
		renderer->BeginRender(*camera); 

        for (unsigned int i = 0; i < g_objects.size(); i++)
        {
            if (g_objects[i]->model != NULL)
            {
                renderer->DrawModel(*g_objects[i], modelShader, normalShader);
            }
        }
        // draw above so you can see underneath
        this->terrain.Render(&this->binnShader, this->camera);
		
        
        renderer->DrawWater(distortWater, waterShader);
		renderer->DrawDirWater(dirWater, waterDirShader);
        renderer->DrawSphere(*testSphere, binnShader);
        
        // TODO(CK): Remove the book object..
        //renderer->DrawSphere(*book, binnShader);


		renderer->EndRender();
		gui->Render();
	}
    
	void Game::ProcessInput(int key, int action, int scancode, int mods, float dt)
	{
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            Reload(&binnShader);
        }
        
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
        { 
            testSphere->pos.x -= 28 * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) 
        { 
            testSphere->pos.x += 28 * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) 
        { 
            testSphere->pos.z += 28 * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) 
        { 
            testSphere->pos.z -= 28 * dt;
        }
        
		/*
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		{
			// Change to (!cameraMode) - camera mode else guimode
			if (mode == Mode::CAMERA)
			{
				mode = Mode::GUI;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				std::cout << "normal\n";

			}
			else
			{
				mode = Mode::CAMERA;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				std::cout << "disabled\n";
			}
		}
		*/
	}
    
	void Game::ProcessMouseButtons(int button, int action, int mods)
	{
		// NOTE(CK): (hack?)
        // TODO(CK): Need to not allow changing camera speed while gui is active
		// This interacts with the gui well because if the mouse button is held down
		// the gui wont register to the mouse
		if (!gui->Active() && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			leftMousePressed = true;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			leftMousePressed = false;
		}
        
        // TODO(CK): ! DEBUG TEST
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            debugRightMousePressed = true;
        }
	}
    
	void Game::Shutdown()
	{
		renderer->Shutdown();
		delete renderer;
        
		delete distortWater;
		delete dirWater;
        delete testSphere;
        
		delete camera;
		gui->Shutdown();
		delete gui;
	}
	
	void Game::processCamera(float dt)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::FORWARD, dt); }
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::BACKWARD, dt); }
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::LEFT, dt); }
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::RIGHT, dt); }
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::UP, dt); }
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { camera->ProcessKeyboard(Camera_Movement::DOWN, dt); }
	}
}