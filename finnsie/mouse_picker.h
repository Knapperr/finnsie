#ifndef MOUSE_PICKER_H
#define MOUSE_PICKER_H

#include "terrain.h"

/*
    public static Matrix4f createViewMatrix(Camera camera) {
    Matrix4f viewMatrix = new Matrix4f();
    viewMatrix.setIdentity();
    Matrix4f.rotate((float) Math.toRadians(camera.getPitch()), new Vector3f(1, 0, 0), viewMatrix,
            viewMatrix);
    Matrix4f.rotate((float) Math.toRadians(camera.getYaw()), new Vector3f(0, 1, 0), viewMatrix, viewMatrix);
    Vector3f cameraPos = camera.getPosition();
    Vector3f negativeCameraPos = new Vector3f(-cameraPos.x,-cameraPos.y,-cameraPos.z);
    Matrix4f.translate(negativeCameraPos, viewMatrix, viewMatrix);
    return viewMatrix;

*/

// already have this in the camera
//glm::mat4 createViewMatrix(Camera camera)
//{
//    glm::mat4 viewMatrix = glm::mat4(1.0f);
//    viewMatrix.rotate 
//}

namespace finnsie {

#define RECURSION_COUNT 200.0f
#define RAY_RANGE 400.0f

    // NOTE(ck): References
    //  https://github.com/munro98/LandscapeWorld/blob/master/src/MousePicker.cpp
    //  https://www.youtube.com/watch?v=DLKN0jExRIM

    class MousePicker
    {
    public:
        glm::vec3 currentTerrainPoint;
        glm::vec3 currentRay;
        glm::mat4 projectionMatrix;
        Terrain *terrain;

        MousePicker();
        MousePicker(Terrain *terrain);

        void Update(float mouseX, float mouseY, glm::mat4 viewMatrix, glm::mat4 projection, glm::vec3 cameraPos);
        glm::vec3 CalculateMouseRay(float mouseX, float mouseY, glm::mat4 viewMatrix);
        glm::vec4 ToEyeCoords(glm::vec4 clipCoords);
        glm::vec3 ToWorldCoords(glm::vec4 eyeCoords, glm::mat4 viewMatrix);
        glm::vec2 GetNormalizedDeviceCoords(float mouseX, float mouseY);

    private:
        glm::vec3 binarySearch(int count, float start, float finish, glm::vec3 ray, glm::vec3 cameraPosition);
        glm::vec3 getPointOnRay(glm::vec3 ray, float distance, glm::vec3 cameraPosition);
        bool intersectionInRange(float start, float finish, glm::vec3 ray, glm::vec3 cameraPosition);
        bool isUnderGround(glm::vec3 testPoint);
    };
}
#endif