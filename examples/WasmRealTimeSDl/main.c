#include <SDL2/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "palash.h"

#define WIDTH  1920
#define HEIGHT 1080

typedef struct {
    MeshInstance instance;
    Material model_material;
    Camera cam;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    float angle;
    float theta;
    bool running;
} AppContext;

void main_loop(void* arg) {
    AppContext *ctx = (AppContext*)arg;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            ctx->running = false;
        }
    }

    if (!ctx->running) {
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#endif
        return; 
    }

    Vec3 scale = vec3(1.0f, 1.0f, 1.0f);
    Vec3 rotate = vec3(0.0f, ctx->angle, 0.0f); 
    Vec3 translate = vec3(0.0f, 0.0f, 0.0f);
    ctx->instance.transform = TRS(translate, rotate, scale);

    DrawMeshInstance(&ctx->instance);
    
    Frame *frame = GetCurrentFrame();
    
    if (SDL_UpdateTexture(ctx->texture, NULL, frame->buffer, frame->width * 3) != 0) {
        printf("Texture Upload Failed: %s\n", SDL_GetError());
    }
    
    SDL_RenderClear(ctx->renderer);
    SDL_RenderCopy(ctx->renderer, ctx->texture, NULL, NULL);
    SDL_RenderPresent(ctx->renderer);

    ClearFrame();
    ctx->angle += ctx->theta;
}

int main() {
    InitializeRendrer(WIDTH, HEIGHT);
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Real Time SW Renderer (Cough Cough)",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WIDTH, HEIGHT,
        SDL_WINDOW_FULLSCREEN
    );
    if (!window) {
        fprintf(stderr,"%s\n",SDL_GetError());
        exit(-1);
    }

    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(window, -1, 0);
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
 
    Mesh    *mesh   = load_obj("/objs/catgirl/catgirl.obj");
    Texture *tex    = load_texture("/objs/catgirl/catgirl.png");
    if (!mesh || !tex) {
        fprintf(stderr, "Failed to load assets!\n");
        return -1;
    }

    AppContext *ctx = malloc(sizeof(AppContext));

     ctx->model_material = (Material){
        .diffuse_map       = tex,
        .normal_map        = NULL,
        .specular_map      = NULL,
        .albedo_color      = DefaultAlbedo,
        .shininess         = 6.f,   
        .specular_strength = 0.25f
    };

    ctx->cam = (Camera){
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
        .mat       = &ctx->model_material,
        .transform = Mat4Diagonal(1.0f) 
    };

    BindCamera(&ctx->cam);
    BindFragColorizer(phong_tex_colorizer);

    ctx->instance = instance;
    ctx->texture = sdl_texture;
    ctx->renderer = sdl_renderer;
    ctx->angle = 0.0f;
    ctx->theta = 0.05f;
    ctx->running = true;
 
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, ctx, 0, 1);
#else
    while (ctx->running) {
        main_loop(ctx); 
    }
#endif
    
    free_tex(tex);
    free_mesh(mesh);

    {
        SDL_DestroyTexture(sdl_texture);
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    DeInitializeRendrer();
    free(ctx);
    return 0;  
}

#include "../src/palash.c"
