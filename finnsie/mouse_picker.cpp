#include "mouse_picker.h"

namespace finnsie {
    
    MousePicker::MousePicker()
    {
        projectionMatrix = glm::mat4(1.0f);
        currentRay = glm::vec3(0.0f);
        currentTerrainPoint = glm::vec3(0.0f);
    }

    MousePicker::MousePicker(Terrain *ter)
    {
        terrain = ter;
        projectionMatrix = glm::mat4(1.0f);
        currentRay = glm::vec3(0.0f);
        currentTerrainPoint = glm::vec3(0.0f);
    }
#define RECURSION_COUNT 200.0f
#define RAY_RANGE 100.0f
    void MousePicker::Update(float mouseX, float mouseY, glm::mat4 viewMatrix, glm::mat4 projection, glm::vec3 cameraPos)
    {
        // TODO(ck): Do this for now? the java implementation points to it 
        // ours cant cause of glm
        projectionMatrix = projection;
        currentRay = CalculateMouseRay(mouseX, mouseY, viewMatrix);
        currentTerrainPoint = binarySearch(0, 0, RAY_RANGE, currentRay, cameraPos);

    }
    
    // TODO(ck): Move to update?
    glm::vec3 MousePicker::CalculateMouseRay(float mouseX, float mouseY, glm::mat4 viewMatrix)
    {
        //float mouseX = Mouse.getX();
        //float mouseY = Mouse.getY();
        glm::vec2 normalizedCoords = GetNormalizedDeviceCoords(mouseX, mouseY);
        glm::vec4 clipCoords = glm::vec4(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);
        glm::vec4 eyeCoords = ToEyeCoords(clipCoords);
        glm::vec3 worldRay = ToWorldCoords(eyeCoords, viewMatrix);
        return worldRay;
    }

    glm::vec4 MousePicker::ToEyeCoords(glm::vec4 clipCoords)
    {
        glm::vec4 eyeCoords = glm::inverse(projectionMatrix) * clipCoords;
        return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
    }

    glm::vec3 MousePicker::ToWorldCoords(glm::vec4 eyeCoords, glm::mat4 viewMatrix)
    {
        glm::vec4 rayWorld = glm::inverse(viewMatrix) * eyeCoords;
        glm::vec3 mouseRay = glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);
        mouseRay = glm::normalize(mouseRay);
        return mouseRay;
    }

    glm::vec2 MousePicker::GetNormalizedDeviceCoords(float mouseX, float mouseY)
    {
        // TODO(ck):
        // (Display.GetWidth, Display.GetHeight) for this math
        int displayWidth = 1440;
        int displayHeight = 900;
        float x = (2.0f * mouseX) / (float)displayWidth - 1.0f;
        float y = (2.0f * mouseY) / (float)displayHeight - 1.0f;

        // NOTE(ck): Make sure to reverse the direction of y!!!!
        return glm::vec2(x, -y);
    }

    glm::vec3 MousePicker::binarySearch(int count, float start, float finish, glm::vec3 ray, glm::vec3 cameraPosition)
    {
        float half = start + ((finish - start) / 2.0f);
        if (count >= RECURSION_COUNT)
        {
            glm::vec3 endPoint = getPointOnRay(ray, half, cameraPosition);
            return endPoint;
        }

        if (intersectionInRange(start, half, ray, cameraPosition))
        {
            return binarySearch(count + 1, start, half, ray, cameraPosition);
        }
        else
        {
            return binarySearch(count + 1, half, finish, ray, cameraPosition);
        }
    }

    glm::vec3 MousePicker::getPointOnRay(glm::vec3 ray, float distance, glm::vec3 cameraPosition)
    {
        glm::vec3 scaledRay = glm::vec3(ray.x * distance, ray.y * distance, ray.z * distance);
        return cameraPosition + scaledRay;
    }

    bool MousePicker::intersectionInRange(float start, float finish, glm::vec3 ray, glm::vec3 cameraPosition)
    {
        glm::vec3 startPoint = getPointOnRay(ray, start, cameraPosition);
        glm::vec3 endPoint = getPointOnRay(ray, finish, cameraPosition);

        if (!isUnderGround(startPoint) && isUnderGround(endPoint))
            return true;
        else
            return false;
    }

    bool MousePicker::isUnderGround(glm::vec3 testPoint)
    {
        float height = terrain->GetHeight(testPoint.x, testPoint.z);
        if (testPoint.y < height)
            return true;
        else
            return false;
    }
}