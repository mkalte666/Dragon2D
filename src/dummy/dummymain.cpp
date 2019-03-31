/*
    main.c: dummymain entry point
    Copyright (C) 2018 Malte Kießling
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <SDL.h>
#include <SDL_image.h>

#include <GL/gl3w.h>
#include <GL/glcorearb.h>

#include "tinyxml2.h"
#include <iostream>
// clang-format off
#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_sdl.h"
// clang-format on

#include <pybind11/embed.h>

namespace py = pybind11;
using namespace py::literals;


int callFromPython(std::string pytext)
{
    ImGui::Begin("window created by a function that is called from python");
    ImGui::Text("The test behind this is sent from python %s", pytext.c_str());
    ImGui::End();

    return 123;
}

PYBIND11_EMBEDDED_MODULE(dummymain, m) {
    m.def("someFunc", &callFromPython);
}

const char* testPython = "import dummymain\n"
                         "dummymain.someFunc(\"heya im in the python code\")";

int main(int argc, char* argv[])
{
#ifdef _DEBUG
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);
#endif
    int rc = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_VIDEO);
    if (rc < 0) {
        std::cerr << "Cannot Init SDL! " << SDL_GetError() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "INIT ERROR", "Cannot init SDL!", nullptr);
        exit(1);
    }

    rc = IMG_Init(IMG_INIT_JPG || IMG_INIT_PNG);
    if (rc < 0) {
        std::cerr << "Cannot Init SDL_image" << IMG_GetError() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "INIT ERROR", "Cannot init SDL_image!", nullptr);
        exit(1);
    }

    // init python
    py::scoped_interpreter pythonInterpreter{};

    SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");

    // we are runnable. put in own scope so we run some destructors of stack objects before everything explodes
    {
        // sdl window and gles
        const char* glsl_version = "#version 100";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);

        SDL_Window* window = SDL_CreateWindow("Brackwasser", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        if (!window) {
            std::cerr << "Cannot create window" << SDL_GetError() << std::endl;
            exit(3);
        }

        SDL_Renderer* renderer;
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        if (!renderer) {
            std::cerr << "Cannot create renderer" << SDL_GetError() << std::endl;
            exit(3);
        }
        SDL_RenderClear(renderer); // makes renderer active

        // path. why here?
        auto basePathP = SDL_GetBasePath();
        SDL_Log("Base Path: %s", basePathP);
        SDL_free(basePathP);

        // gl3w
        auto gl3wres = gl3wInit();
        bool err = gl3wres != 0;
        if (err) {
            std::cerr << "Failed to init gl3w:" << gl3wres << std::endl;
            if (gl3wres == GL3W_ERROR_OPENGL_VERSION) {
                std::cerr << "trying to continue with broken gl3w cause its a version mismatch error that can happen with gles contexts" << std::endl
                          << "this might crash" << std::endl;
            } else {
                exit(3);
            }
        }

        // imgui setup
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        // we are supposed to give a context here, but what for?
        ImGui_ImplSDL2_InitForOpenGL(window, nullptr);
        ImGui_ImplOpenGL3_Init(glsl_version);
        (void)io;
        ImGui::StyleColorsDark();

        bool running = true;
        double deltaTime = 0.01;

        uint64_t now = SDL_GetPerformanceCounter();
        uint64_t last = 0;
        while (running) {
            // poll events
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                ImGui_ImplSDL2_ProcessEvent(&e);

                if (e.type == SDL_QUIT) {
                    running = false;
                }
            }

            if (!running) {
                break;
            }

            // clear screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            ImGui::Begin("Heya");
            ImGui::Text("If you can read this, then the library tester (aka dummymain) is working.");
            ImGui::End();

            py::exec(testPython);

            ImGui::ShowDemoWindow();
            // this flushes the renderer. after this only imgui
            SDL_RenderFlush(renderer);

            // now render imgui
            ImGui::Render(); // thos does not actually render (as in: draw calls)
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // this does, though
            // and swap
            SDL_GL_SwapWindow(window);

            // now do rendering until next frame!

            deltaTime = (double)((now - last) / (double)SDL_GetPerformanceFrequency());
            last = now;
            now = SDL_GetPerformanceCounter();
            if (deltaTime < 0.008) {
                SDL_Delay(static_cast<int>((0.008 - deltaTime) * 1000));
            }
        }

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }
    Py_Finalize();
    SDL_Quit();

    return 0;
}