#include "Camera.hpp"
#include <GLFW\glfw3.h>
#include <iostream>

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget);
        this->cameraRightDirection = glm::cross(cameraUp, this->cameraFrontDirection);
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);

    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //std::cout << "Camera position: " << this->cameraPosition.x << ", " << this->cameraPosition.y << ", " << this->cameraPosition.z << "\n";
        glm::vec3 newpos;
        switch (direction) {
        case MOVE_FORWARD:
            newpos = this->cameraPosition + speed * this->cameraFrontDirection;
            if (newpos.y >= -1.0f)
                this->cameraPosition = newpos;
            break;
        case MOVE_BACKWARD:
            newpos = this->cameraPosition - speed * this->cameraFrontDirection;
            if (newpos.y >= -1.0f)
                this->cameraPosition = newpos;
            break;
        case MOVE_LEFT:
            this->cameraPosition += speed * this->cameraRightDirection;
            break;
        case MOVE_RIGHT:
            this->cameraPosition -= speed * this->cameraRightDirection;
            break;
        case MOVE_UPWARD:
            newpos = this->cameraPosition + speed * this->cameraUpDirection;
            if (newpos.y >= -1.0f)
                this->cameraPosition = newpos;
            break;
        case MOVE_DOWNWARD:
            newpos = this->cameraPosition - speed * this->cameraUpDirection;
            if (newpos.y >= -1.0f)
                this->cameraPosition = newpos;
            break;
        }
        this->cameraPosition.y = 0.1f;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->cameraFrontDirection = glm::normalize(front);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        this->cameraUpDirection = glm::normalize(glm::cross(this->cameraRightDirection, this->cameraFrontDirection));
    }
}