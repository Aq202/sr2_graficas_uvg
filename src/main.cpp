#include <SDL2/SDL.h>
#include "framebuffer.h"
#include "triangle.h"
#include "loadOBJ.h"
#include <iostream>
#include "setupVertexArray.h"
#include "uniforms.h"
#include "shaders.h"
#include "fragment.h"
#include "camera.h"
#include "glm/ext.hpp"

void printVertex(glm::vec3 vertex)
{
    SDL_Log("Vértice: (%f, %f, %f)\n", vertex.x, vertex.y, vertex.z);
}

void renderBuffer(SDL_Renderer *renderer)
{
    // Create a texture
    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        FRAMEBUFFER_WIDTH,
        FRAMEBUFFER_HEIGHT);

    // Update the texture with the pixel data from the framebuffer
    SDL_UpdateTexture(
        texture,
        NULL,
        framebuffer,
        FRAMEBUFFER_WIDTH * sizeof(Color));

    // Copy the texture to the renderer
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    // Destroy the texture
    SDL_DestroyTexture(texture);
}

std::vector<std::vector<Vertex>> primitiveAssembly(
    const std::vector<Vertex> &transformedVertexes)
{
    std::vector<std::vector<Vertex>> groupedVertexes;

    for (int i = 0; i < transformedVertexes.size(); i += 3)
    {
        std::vector<Vertex> triangle;
        triangle.push_back(transformedVertexes[i]);
        triangle.push_back(transformedVertexes[i + 1]);
        triangle.push_back(transformedVertexes[i + 2]);

        groupedVertexes.push_back(triangle);
    }

    return groupedVertexes;
}

float rotation = 0;
float pi = 3.1416;
void render(std::vector<glm::vec3> &vertexes, Uniforms &uniforms)
{

    // vertex shader
    std::vector<Vertex> transformedVertexes;
    for (int i = 0; i < vertexes.size(); i+=2)
    {
        Vertex vertex = {vertexes[i], vertexes[i + 1]};
        Vertex transformedVertex = vertexShader(vertex, uniforms);
        transformedVertexes.push_back(transformedVertex);
    }

    // primitive assembly
    std::vector<std::vector<Vertex>> triangles = primitiveAssembly(transformedVertexes);

    // rasterize
    std::vector<Fragment> fragments;
    for (std::vector<Vertex> tr : triangles)
    {
        std::vector<Fragment> rasterizedTriangle = triangle(tr[0], tr[1], tr[2]);

        fragments.insert(fragments.end(), rasterizedTriangle.begin(), rasterizedTriangle.end());
    }

    // fragment shader
    for(Fragment fragment : fragments){

        const Fragment& shadedFragment = fragmentShader(fragment);
        point(shadedFragment);
    }
}

glm::mat4 createViewportMatrix(size_t screenWidth, size_t screenHeight) {
    glm::mat4 viewport = glm::mat4(1.0f);

    // Scale
    viewport = glm::scale(viewport, glm::vec3(screenWidth / 2.0f, screenHeight / 2.0f, 0.5f));

    // Translate
    viewport = glm::translate(viewport, glm::vec3(1.0f, 1.0f, 0.5f));

    return viewport;
}

Uint32 frameStart, frameTime;

int main(int argc, char *argv[])
{

    // iniciar ventana sdl
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("Spaceship", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<Face> faces;
    if (!loadOBJ("../models/model.obj", vertices, normals, faces))
    {
        return 0;
    }

    std::vector<glm::vec3> modelVertex = setupVertexArray(vertices, faces, normals);

    Uniforms uniforms;

    glm::vec3 translationVector(0.0f, 0.0f, 0.0f);
    float a = 45.0f;
    glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis
    glm::vec3 scaleFactor(1.0f, 1.0f, 1.0f);

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), translationVector);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), scaleFactor);

    // Initialize a Camera object
    Camera camera;
    camera.cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    camera.targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.upVector = glm::vec3(0.0f, -1.0f, 0.0f);

    // Projection matrix
    float fovInDegrees = 45.0f;
    float aspectRatio =  static_cast<float>(WINDOW_WIDTH) /  static_cast<float>(WINDOW_HEIGHT); 
    float nearClip = 0.1f;
    float farClip = 100.0f;
    uniforms.projection = glm::perspective(glm::radians(fovInDegrees), aspectRatio, nearClip, farClip);

    uniforms.viewport = createViewportMatrix(WINDOW_WIDTH, WINDOW_HEIGHT);


    bool running = true;
    SDL_Event event;

    while (running)
    {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        a += 2;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(a), rotationAxis);

        // Calculate the model matrix
         uniforms.model = translation * rotation * scale;

        // // Create the view matrix using the Camera object
         uniforms.view = glm::lookAt(
             camera.cameraPosition, // The position of the camera
             camera.targetPosition, // The point the camera is looking at
             camera.upVector        // The up vector defining the camera's orientation
         );

        clear();
        render(modelVertex, uniforms);

        // Render the framebuffer to the screen
        renderBuffer(renderer);

        // Present the frame buffer to the screen
        SDL_RenderPresent(renderer);

        // Delay to limit the frame rate
        SDL_Delay(1000 / 60);

        frameTime = SDL_GetTicks() - frameStart;

        if (frameTime > 0)
        {
            std::ostringstream titleStream;
            titleStream << "FPS: " << 1000.0 / frameTime; // Milliseconds to seconds
            SDL_SetWindowTitle(window, titleStream.str().c_str());
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}