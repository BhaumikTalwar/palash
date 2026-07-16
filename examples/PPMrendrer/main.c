#include "renderer.h"

#define WIDTH  800
#define HEIGHT 600


int main() {
    InitializeRendrer(WIDTH, HEIGHT);

    Vec3 light_dir  = vec3(1.0f, 1.0f, -1.0f); 
    Vec3 ambient    = vec3(0.2f, 0.2f, 0.2f);   
    Color light_col = WHITE;
    SetLight(light_dir, ambient, light_col);
    
    Mesh    *mesh   = load_obj("../objs/head.obj");
    Texture *tex    = load_texture("../objs/head_diffuse.tga");
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
        .position = vec3(0.0f, 0.0f, 2.0f),  
        .target   = vec3(0.0f, 0.0f, 0.0f),  
        .up       = vec3(0.0f, 1.0f, 0.0f),  

        .perspective = {
            .fov = 60.0f * DegToRad,        
            .aspectR = (f32)WIDTH / (f32)HEIGHT
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

    char file_pattern[40]; 
    int duration   = 20;
    int num_frames = FPS * duration;
    float theta    = 6.28 / num_frames;
    float angle    = 0;

    for (int i = 0; i < num_frames; i++) {
        sprintf(file_pattern, "./frames/output-%06d.ppm", i); 
        FILE *f = fopen(file_pattern, "wb");

        if (f == NULL) {
            fprintf(stderr, "Cant Open the File at: %s\n", file_pattern);
            exit(EXIT_FAILURE);    
        }

        Vec3 scale         = vec3(1.0f, 1.0f, 1.0f);
        Vec3 rotate        = vec3(0.0f, angle, 0.0f); 
        Vec3 translate     = vec3(0.0f, 0.0f, 0.0f);
        instance.transform = TRS(translate, rotate, scale);

        DrawMeshInstance(&instance);
        RenderFramePPM(f);
        LOG("Generated:- %s", file_pattern);

        ClearFrame();
        fclose(f);
        angle += theta;
    }
    
    free_tex(tex);
    free_mesh(mesh);

    DeInitializeRendrer();
    return 0;  
}


#include "../../src/renderer.c"
