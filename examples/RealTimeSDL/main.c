#include <SDL3/SDL.h>

#include "renderer.h"

#define WIDTH  1920
#define HEIGHT 1080


int main() {
    InitializeRendrer(WIDTH, HEIGHT);
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Real Time SW Renderer (Cough Cough)",
        WIDTH, HEIGHT,
        SDL_WINDOW_FULLSCREEN
    );
    if (!window) {
        fprintf(stderr,"%s\n",SDL_GetError());
        exit(-1);
    }

    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(window, NULL);
    if (!sdl_renderer) {
        fprintf(stderr,"%s\n",SDL_GetError());
        exit(-1);
    }

    SDL_Texture *sdl_texture =
        SDL_CreateTexture(
            sdl_renderer,
            SDL_PIXELFORMAT_RGB24,
            SDL_TEXTUREACCESS_STREAMING,
            WIDTH, HEIGHT
        );
    if (!sdl_texture) {
        fprintf(stderr,"%s\n",SDL_GetError());
        exit(-1);
    }
 
    Vec3 light_dir  = vec3(1.0f, 1.0f, -1.0f); 
    Vec3 ambient    = vec3(0.2f, 0.2f, 0.2f);   
    Color light_col = COLOR_HEX(0x663FD6);
    SetLight(light_dir, ambient, light_col);
 
    Mesh    *mesh   = load_obj("../objs/catgirl/catgirl.obj");
    Texture *tex    = load_texture("../objs/catgirl/catgirl.png");
    if (!mesh || !tex) {
        fprintf(stderr, "Failed to load assets!\n");
        return -1;
    }

    Material model_material = {
        .diffuse_map       = tex,
        .normal_map        = NULL,
        .specular_map      = NULL,
        .albedo_color      = DefaultAlbedo,
        .shininess         = 6.f,   
        .specular_strength = 0.25f
    };

    Camera cam = {
        .type     = CAMERA_PERSPECTIVE,
        .position = vec3(0.5f, 1.0f, 1.0f),  
        .target   = vec3(0.0f, 0.0f, 0.0f),  
        .up       = vec3(0.0f, 1.0f, 0.0f),  

        .perspective = {
            .fov = 60.0f * DegToRad,        
            .aspectR = (float)WIDTH / (float)HEIGHT
        },

        .near_plane = 0.1f,
        .far_plane  = 100.0f
    };

    MeshInstance instance = {
        .mesh      = mesh,
        .mat       = &model_material,
        .transform = Mat4Diagonal(1.0f) 
    };

    BindCamera(&cam);
    BindFragColorizer(phong_tex_colorizer);

    bool  running  = true;
    float theta    = 0.05f;
    float angle    = 0;

    while (running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
        }

        Vec3 scale         = vec3(1.0f, 1.0f, 1.0f);
        Vec3 rotate        = vec3(0.0f, angle, 0.0f); 
        Vec3 translate     = vec3(0.0f, 0.0f, 0.0f);
        instance.transform = TRS(translate, rotate, scale);

        DrawMeshInstance(&instance);
        
        // To render frame to sdl_texture
        {
            Frame *frame = GetCurrentFrame();
            SDL_UpdateTexture( sdl_texture, NULL, frame->buffer, frame->width*3);
            SDL_RenderClear( sdl_renderer);
            SDL_RenderTexture( sdl_renderer, sdl_texture, NULL, NULL);
            SDL_RenderPresent(sdl_renderer);
        }

        ClearFrame();
        angle += theta;
    }
    
    free_tex(tex);
    free_mesh(mesh);

    {
        SDL_DestroyTexture(sdl_texture);
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow( window);
        SDL_Quit();
    }

    DeInitializeRendrer();
    return 0;  
}


#include "../../src/renderer.c"
