#include <filesystem>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include <iostream>
#include <vector>

int main(int argv, const char* argc[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Pixt", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window);
    ImGui_ImplSDLRenderer_Init(renderer);
    SDL_Texture* image = SDL_CreateTexture(renderer, NULL, SDL_TEXTUREACCESS_TARGET, 32, 32);
    SDL_SetRenderTarget(renderer, image);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    int mx, my;
    SDL_SetRenderTarget(renderer, NULL);
    SDL_Rect img_dst = {284, 284, 32, 32};
    SDL_Event e;
    int zoom = 1;
    bool placing = false;
    int tool = 0;
    bool done = false;
    int color[4] = {0, 0, 0, 255};
    int brush_size = 1;
    bool pixel_grid = false;
    int w_w, w_h;
    int current_color = 0;
    std::vector<std::vector<int>> colors = {{0, 0, 0, 255}, {255, 255, 255, 255}, 
                                            {255, 0, 0, 255}, {0, 255, 0, 255}, 
                                            {0, 0, 255, 255}, {255, 255, 0, 255}, 
                                            {0, 255, 255, 255}, {255, 0, 255, 255}};

    while (!done) {
        SDL_GetMouseState(&mx, &my);
        SDL_GetWindowSize(window, &w_w, &w_h);
        SDL_Rect final_dst = {img_dst.x-((img_dst.w*zoom-img_dst.w)/2), img_dst.y-((img_dst.h*zoom-img_dst.h)/2), img_dst.w*zoom, img_dst.h*zoom};
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT) {
                done = true;
                break;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (mx > final_dst.x && mx < final_dst.w+final_dst.x && my > final_dst.y && my < final_dst.h+final_dst.y) {
                    placing = true;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (placing) {
                    placing = false;
                }
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_b) {
                    tool = 0;
                }
                else if (e.key.keysym.sym == SDLK_e) {
                    tool = 1;
                }
                else if (e.key.keysym.sym == SDLK_EQUALS) {
                    brush_size += 1;
                }
                else if (e.key.keysym.sym == SDLK_MINUS) {
                    if (brush_size > 1) {
                        brush_size -= 1;
                    }
                }
                else if (e.key.keysym.sym == SDLK_w) {
                    img_dst.y += zoom;
                }
                else if (e.key.keysym.sym == SDLK_s) {
                    img_dst.y -= zoom;
                }
                else if (e.key.keysym.sym == SDLK_a) {
                    img_dst.x += zoom;
                }
                else if (e.key.keysym.sym == SDLK_d) {
                    img_dst.x -= zoom;
                }
                else if (e.key.keysym.sym == SDLK_q) {
                    pixel_grid = !pixel_grid;
                }
            }
            else if (e.type == SDL_MOUSEWHEEL) {
                if (e.wheel.y > 0) {
                    zoom += 2;
                }
                else if (e.wheel.y < 0 && zoom > 1) {
                    zoom -= 2;
                }
            }
        }
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, image, NULL, &final_dst);
        if (placing) {
            if (tool == 0) {
                SDL_SetRenderTarget(renderer, image);
                SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
                SDL_Rect rect = {(mx-final_dst.x)/zoom, (my-final_dst.y)/zoom, brush_size, brush_size};
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderTarget(renderer, NULL);
            }
            else if (tool == 1) {
                SDL_SetRenderTarget(renderer, image);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
                SDL_Rect rect = {(mx-final_dst.x)/zoom, (my-final_dst.y)/zoom, brush_size, brush_size};
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderTarget(renderer, NULL);
            }
        }
        if (mx > final_dst.x && mx < final_dst.w+final_dst.x && my > final_dst.y && my < final_dst.h+final_dst.y && !ImGui::IsAnyItemHovered) {
            if (tool <= 1) {
                if (tool == 0)
                    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
                else
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
                SDL_Rect rect = {(mx-final_dst.x)-((mx-final_dst.x)%zoom)+final_dst.x, (my-final_dst.y)-((my-final_dst.y)%zoom)+final_dst.y, zoom*brush_size, zoom*brush_size};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &final_dst);
        if (pixel_grid) {
            for (int x = zoom+final_dst.x; x<final_dst.w+final_dst.x;x+=zoom) {
                SDL_RenderDrawLine(renderer, x, final_dst.y, x, final_dst.y+final_dst.h);
            }
            for (int y = zoom+final_dst.y; y<final_dst.h+final_dst.y;y+=zoom) {
                SDL_RenderDrawLine(renderer, final_dst.x, y, final_dst.x+final_dst.w, y);
            }
        }

        //Menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }


        //Palette Editor
        static int color_list_dif = 200;
        static int palette_width = 150;
        ImGui::SetNextWindowPos(ImVec2(0, 48));
        ImGui::SetNextWindowSize(ImVec2(palette_width, w_h-64));
        ImGui::Begin("Palette Editor", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysUseWindowPadding);
        if (ImGui::Button("+", ImVec2(20, 20))) {
            colors.push_back(std::vector<int>{255, 255, 255, 255});
        }
        ImGui::SetCursorPosX(palette_width-20);
        ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 100.0f);
        ImGui::VSliderInt("##v", ImVec2(12, w_h-64-250), &color_list_dif, 0, 200, "");
        ImGui::PopStyleVar();
        static int picker_mode = 0;
        ImGui::PushItemWidth(palette_width-10);
        ImGui::Combo("##Picker mode", &picker_mode, "Hue bar\0Hue wheel\0");
        ImVec4 imcolor = {(float)colors[current_color][0], (float)colors[current_color][1], (float)colors[current_color][2], (float)colors[current_color][3]};
        ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_None;
        picker_flags |= ImGuiColorEditFlags_AlphaBar;
        picker_flags |= ImGuiColorEditFlags_NoSidePreview;
        picker_flags |= ImGuiColorEditFlags_NoInputs;
        if (picker_mode == 0)
            picker_flags |= ImGuiColorEditFlags_PickerHueBar;
        else
            picker_flags |= ImGuiColorEditFlags_PickerHueWheel;
        ImGui::ColorPicker4("", (float*)&imcolor, picker_flags);
        ImGui::PopItemWidth();
        colors[current_color] = {(int)imcolor.x, (int)imcolor.y, (int)imcolor.z, (int)imcolor.w};
        ImGui::End();

        //Rendering
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        int i_color = 0;

        //Palette colors
        for (int y = 85+color_list_dif-200; y < 350; y+=32) {
            SDL_SetRenderDrawColor(renderer, colors[i_color][0], colors[i_color][1], colors[i_color][2], colors[i_color][3]);
            SDL_Rect c_rect = {12, y, 32, 32};
            SDL_RenderFillRect(renderer, &c_rect);
            if (mx > c_rect.x && my > c_rect.y && mx < c_rect.x+c_rect.w && my < c_rect.y+c_rect.h)
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            else
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &c_rect);
            i_color += 1;
            if (i_color == colors.size())
                break;
            SDL_SetRenderDrawColor(renderer, colors[i_color][0], colors[i_color][1], colors[i_color][2], colors[i_color][3]);
            c_rect = {44, y, 32, 32};
            SDL_RenderFillRect(renderer, &c_rect);
            if (mx > c_rect.x && my > c_rect.y && mx < c_rect.x+c_rect.w && my < c_rect.y+c_rect.h)
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            else
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &c_rect);
            i_color += 1;
            if (i_color == colors.size())
                break;
            SDL_SetRenderDrawColor(renderer, colors[i_color][0], colors[i_color][1], colors[i_color][2], colors[i_color][3]);
            c_rect = {76, y, 32, 32};
            SDL_RenderFillRect(renderer, &c_rect);
            if (mx > c_rect.x && my > c_rect.y && mx < c_rect.x+c_rect.w && my < c_rect.y+c_rect.h)
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            else
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &c_rect);
            i_color += 1;
            if (i_color == colors.size())
                break;
        }
        
        SDL_RenderPresent(renderer);
    }
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyTexture(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
