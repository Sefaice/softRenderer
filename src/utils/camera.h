#pragma once

#include "math.h"

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
//const float YAW         =  0.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;
    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    //// Euler Angles
    //float Yaw;
    //float Pitch;

    // Constructor with vectors
    Camera(vec3 _position, vec3 _worldUp, vec3 _front)
        : movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
        // Yaw(YAW), Pitch(PITCH),
    {
        position = _position;
        worldUp = _worldUp;
        front = _front;

        updateCameraVectors();
    }

    //// Constructor with scalar values
    //Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    //{
    //    Position = vec3(posX, posY, posZ);
    //    WorldUp = vec3(upX, upY, upZ);
    //    Yaw = yaw;
    //    Pitch = pitch;
    //    updateCameraVectors();
    //}

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    mat4 GetViewMatrix()
    {
       mat4 view1 = mat4(right.x, up.x, -front.x, 0,
            right.y, up.y, -front.y, 0,
            right.z, up.z, -front.z, 0,
            0, 0, 0, 1);
        mat4 view2 = mat4(1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            -position.x, -position.y, -position.z, 1);
        mat4 view = view1 * view2;

        return view;
    }

    //// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    //void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    //{
    //    float velocity = MovementSpeed * deltaTime;
    //    if (direction == FORWARD)
    //        Position += Front * velocity;
    //    if (direction == BACKWARD)
    //        Position -= Front * velocity;
    //    if (direction == LEFT)
    //        Position -= Right * velocity;
    //    if (direction == RIGHT)
    //        Position += Right * velocity;
    //}

    //// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    //void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
    //{
    //    xoffset *= mouseSensitivity;
    //    yoffset *= mouseSensitivity;

    //    Yaw += xoffset;
    //    Pitch += yoffset;
    //    /*if(Up.y < 0)
    //        Yaw += xoffset;
    //    else
    //        Yaw -= xoffset;
    //    Pitch += yoffset;*/

    //    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    //    if (constrainPitch)
    //    {
    //        if (Pitch > 89.0f)
    //            Pitch = 89.0f;
    //        if (Pitch < -89.0f)
    //            Pitch = -89.0f;
    //    }

    //    // Update Front, Right and Up Vectors using the updated Euler angles
    //    updateCameraVectors();
    //}

    //// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    //void ProcessMouseScroll(float yoffset)
    //{
    //    if (Zoom >= 1.0f && Zoom <= 45.0f)
    //        Zoom -= yoffset;
    //    if (Zoom <= 1.0f)
    //        Zoom = 1.0f;
    //    if (Zoom >= 45.0f)
    //        Zoom = 45.0f;
    //}

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
       /* vec3 front;
        front.x = cos(radians(Yaw)) * cos(radians(Pitch));
        front.y = sin(radians(Pitch));
        front.z = sin(radians(Yaw)) * cos(radians(Pitch));*/
        front = normalize(front);
        // Also re-calculate the Right and Up vector
        right = cross(front, worldUp);  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        right = normalize(right);
        up = cross(right, front);
        up = normalize(up);

        /*
        quat key_quat = quat(vec3(radians(Pitch), radians(Yaw), 0));
        ////std::cout << pitch << " " << yaw << std::endl;
        ////std::cout << key_quat.x <<" "<< key_quat.y <<" "<< key_quat.z << std::endl;

        vec3 front = rotate(key_quat, vec3(0, 0, -1));
        Front = normalize(front);

        Up = rotate(key_quat, WorldUp);
        normalize(Up);
        Right = cross(Front, Up);
        normalize(Right);
        */
    }
};