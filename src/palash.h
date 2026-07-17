#ifndef _RENDERER_
#define _RENDERER_

#include "compiler.h"
#include "types.h"
#include "vec.h"

#define CENTER_X(w)  (u32)(((w)  / 2) - 1)
#define CENTER_Y(h)  (u32)(((h)  / 2) - 1)
#define CENTER(w, h) vec2(CENTER_X((w)), CENTER_Y((h)))

#define FPS 60

typedef struct PACKED{
    u8 r,g,b;
} Color;
STATIC_ASSERT((sizeof(Color) == 3), "Color must be 3 bytes");

typedef struct PACKED {
    u8 r,g,b,a;
} TexturePixel;
STATIC_ASSERT((sizeof(TexturePixel) == 4), "Texture Pixel must be 4 bytes");

typedef struct {
    u32 width;
    u32 height;
    u8 channels;

    TexturePixel *pixels;
} Texture;

typedef Color Pixel;

typedef struct {
    u32     width;
    u32     height;

    Pixel   *buffer;
    f32     *depth;
}Frame;
#define PIXEL(f, x, y) (f)->buffer[(i32)((y) * (f)->width + (x))]
#define DEPTH(f, x, y) (f)->depth [(i32)((y) * (f)->width + (x))]

static FORCE_INLINE bool IsInFrame(f32 x, f32 y, f32 w, f32 h) {
    return  ((x >= 0 && x < w)
        &&  (y >= 0 && y < h));
}

typedef struct {
    usize vidxs[3];
    usize tidxs[3];
    usize nidxs[3];
} Face;

typedef struct {
    Vec3 *vertices;
    Vec3 *uvs;
    Vec3 *normals;
    Face *faces;
} Mesh;

typedef struct {
    Vec3 normal;
    Vec3 world_pos;
    Vec2 screen;
    f32 depth;

    f32 alpha;
    f32 beta;
    f32 gamma;

    f32 u, v;
} Fragment;

typedef struct{ 
    Texture *diffuse_map; 
    Texture *normal_map; 
    Texture *specular_map; 

    Color albedo_color;

    f32 shininess;
    f32 specular_strength;
} Material;

typedef struct {
    Material *material;   

    Vec3 camera_pos;
    Vec3 ambient_light;;
    Vec3 directional_light_dir;
    Color directional_light_color; 
} ColorizerData;

typedef Color (*FragColorizer)(Fragment*, ColorizerData);

typedef enum  {
    CAMERA_PERSPECTIVE,
    CAMERA_ORTHOGRAPHIC,
} CameraProjection;

typedef struct {
    CameraProjection type;

    Vec3 position;
    Vec3 target;
    Vec3 up;

    union {
        struct {
            float fov;
            float aspectR;
        } perspective;

        struct {
            float left;
            float right;
            float bottom;
            float top;
        } orthographic;
    };

    float near_plane;
    float far_plane;
} Camera;

typedef struct {
    Vec3 pos;
    Vec3 uv;
} ScreenVertex;

typedef struct {
    Mat4 transform;

    Mesh *mesh;
    Material *mat;
} MeshInstance;

#define COLOR_HEX(hex) ((Color){ \
    .r = ((hex >> 16) & 0xFF),   \
    .g = ((hex >> 8) & 0xFF),    \
    .b = ((hex) & 0xFF)          \
})

static const Color RED        = COLOR_HEX(0xFF0000);
static const Color GREEN      = COLOR_HEX(0x00FF00);
static const Color BLUE       = COLOR_HEX(0x0000FF);
static const Color WHITE      = COLOR_HEX(0xFFFFFF);
static const Color BLACK      = COLOR_HEX(0x000000);

static const Color ErrorColor    = COLOR_HEX(0xFF00FF);
static const Color DefaultAlbedo = COLOR_HEX(0xA2A4A4);

static const Material DefaultMaterial = (Material){
    .diffuse_map = NULL,
    .normal_map = NULL,
    .specular_map = NULL,
    .albedo_color = COLOR_HEX(0xA2A4A4), 
};

static const Vec3 DefaultLightDir    = vec3(1, 1, -1);
static const Color DefaultLightColor = COLOR_HEX(0xFFFFFF);

void InitializeRendrer(u32 width, u32 height);
void DeInitializeRendrer(void);

Frame* GetCurrentFrame();
void ClearFrame(void);
int RenderFramePPM(FILE*f);

void SetLight(Vec3 dir, Vec3 amb,Color light_color);
void BindMaterial(Material *mat);
void BindFragColorizer(FragColorizer fcb);

Mesh* load_obj(const char* filepath);
void free_mesh(Mesh* mesh);

void print_debug_obj(Mesh *mesh);
Vec3 mesh_center(Mesh *mesh);

Texture *load_texture(const char* filepath);
Texture *load_texture_max_dim(const char* filepath, u32 max_dim);
void free_tex(Texture* tex);

Color sample2D(Texture *tex, f32 u, f32 v);
Color unlit_tex_colorizer(Fragment *frag, ColorizerData uniforms);
Color barrycentric_colorizer(Fragment *frag, ColorizerData uniforms);
Color phong_tex_colorizer(Fragment *frag, ColorizerData uniforms);

void BindCamera          (Camera *cam);
Mat4 Camera_GetView      (const Camera *cam);
Mat4 Camera_GetProjection(const Camera *cam);

void DrawLine(Vec2 p1, Vec2 p2, Color color);
void DrawMesh(const Mesh *mesh, Material *mat, Mat4 model, Mat4 view, Mat4 proj);
void DrawMeshWireFrame( const Mesh *mesh, Material *mat, Mat4 model, Mat4 view, Mat4 proj);

void DrawMeshInstance(const MeshInstance *instance);
void DrawMeshInstanceWF(const MeshInstance *instance);

#endif // !_RENDERER_
