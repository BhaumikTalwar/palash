#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "renderer.h"
#include "compiler.h"
#include "types.h"
#include "vec.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define DYN_ARR_IMPLEMENTATION
#include "dynarr.h"


typedef struct {
    Frame           frame;
    Camera          *cam;

    Mat4            view;
    Mat4            projection;
    Mat4            view_projection;

    FragColorizer   fcl;
    ColorizerData   data;
}_RenderState;
static _RenderState ctx = {0};

static Frame newFrame(u32 width, u32 height) {
    Frame f  = {0};
    f.width  = width;
    f.height = height;

    usize elem_count = width * height;
    f.buffer = calloc(elem_count, sizeof(Pixel));
    if (f.buffer == NULL) {
        LOG("Cant Allocate a Frame : buffer");
        exit(EXIT_FAILURE);
    }

    f.depth =  malloc(elem_count * sizeof(f32));
    if (f.depth == NULL) {
        LOG("Cant Allocate a Frame : depth buffer");

        free(f.buffer);
        exit(EXIT_FAILURE);
    }
    
    for (usize i = 0; i < elem_count; i++) {
        f.depth[i] = INFINITY;
    }

    return f;
}

void InitializeRendrer(u32 width, u32 height) {
    if (ctx.frame.buffer || ctx.frame.depth) {
        return;
    }

    ctx.frame = newFrame(width, height);
    ctx.data.directional_light_dir   = DefaultLightDir;
    ctx.data.directional_light_color = DefaultLightColor;
}

void DeInitializeRendrer(void) {
    if (ctx.frame.buffer != NULL && ctx.frame.depth != NULL) {
        free(ctx.frame.buffer);
        free(ctx.frame.depth);

        ctx.frame.buffer = NULL;
        ctx.frame.depth  = NULL;
        ctx.frame.width  = 0;
        ctx.frame.height = 0;
    }
}

void ClearFrame(void) {
    if (!ctx.frame.buffer || !ctx.frame.depth) {
        return;
    }

    usize elem_count = ctx.frame.width * ctx.frame.height;
    memset(ctx.frame.buffer, 0, elem_count * sizeof(Pixel));

    for (usize i = 0; i < elem_count; i++) {
        ctx.frame.depth[i] = INFINITY;
    }
}

Frame* GetCurrentFrame() {
    return &ctx.frame;
}

int RenderFramePPM(FILE*f) {
    if (f == NULL || !ctx.frame.buffer) return -1;

    fprintf(f, "P6\n");
    fprintf(f, "%d %d\n", ctx.frame.width ,ctx.frame.height);
    fprintf(f, "255\n");

    fwrite(ctx.frame.buffer,sizeof(Pixel), ctx.frame.height * ctx.frame.width, f);
    return 0;
}

void SetLight(Vec3 dir, Vec3 amb, Color light_color) {
    ctx.data.ambient_light = amb;    
    ctx.data.directional_light_dir = dir;    
    ctx.data.directional_light_color = light_color;    
}

void BindMaterial(Material *mat) {
    ctx.data.material = mat; 
}

void BindFragColorizer(FragColorizer fcb) {
    ctx.fcl = fcb;
}

void BindCamera(Camera *cam){
    ctx.cam = cam;
    ctx.data.camera_pos = cam->position;

    ctx.view = Camera_GetView(cam);
    ctx.projection = Camera_GetProjection(cam);

    ctx.view_projection =
        MulMat4(ctx.projection, ctx.view);
}

Mesh* load_obj(const char* filepath) {
    FILE *file = fopen(filepath, "r"); 
    if (file == NULL) {
        LOG("Cant Load the File: %s", filepath);
        return NULL;
    }

    Mesh *mesh = malloc(sizeof(Mesh));
    mesh->vertices = NULL;
    mesh->faces = NULL;

    char buffer[256] = {0};
    while (fgets(buffer, sizeof(buffer), file) != NULL) {                
        usize len = strlen(buffer);
        if (len == 0 ) continue;        

        if (strncmp(buffer, "v ", 2) == 0) {
            Vec3 pt = {0};
            sscanf(buffer + 2, "%f %f %f", &pt.x, &pt.y, &pt.z);

            arr_push(mesh->vertices, pt);
            continue;
        }

        if (strncmp(buffer, "vt ", 3) == 0) {
            Vec3 uv = {0};
            sscanf(buffer + 3, "%f %f %f", &uv.x, &uv.y, &uv.z);

            arr_push(mesh->uvs, uv);
            continue;
        }

        if (strncmp(buffer, "vn ", 3) == 0) {
            Vec3 nrml = {0};
            sscanf(buffer + 3, "%f %f %f", &nrml.x, &nrml.y, &nrml.z);

            arr_push(mesh->normals, nrml);
            continue;
        }

        if (strncmp(buffer, "f ", 2) == 0) {
            Face f = {0};

            usize n = sscanf(
                buffer + 2,
                "%zu/%zu/%zu %zu/%zu/%zu %zu/%zu/%zu",
                &f.vidxs[0], &f.tidxs[0], &f.nidxs[0],
                &f.vidxs[1], &f.tidxs[1], &f.nidxs[1],
                &f.vidxs[2], &f.tidxs[2], &f.nidxs[2]
            );

            if (n != 9)
                continue;

            for (usize i = 0; i < 3; i++) {
                f.vidxs[i]--;
                f.tidxs[i]--;
                f.nidxs[i]--;
            }

            arr_push(mesh->faces, f);
        }
    }

    fclose(file);
    return mesh;
}

void free_mesh(Mesh* mesh) {
    arr_free(mesh->vertices);
    arr_free(mesh->uvs);
    arr_free(mesh->normals);

    arr_free(mesh->faces);
    free(mesh);
}

void print_debug_obj(Mesh *mesh) {
    for (usize i = 0; i < arr_len(mesh->vertices); i++) {
        Vec3 vert = mesh->vertices[i];
        printf("v %f %f %f\n",vert.x, vert.y, vert.z);
    }

    for (usize i = 0; i < arr_len(mesh->uvs); i++) {
        Vec3 vert = mesh->uvs[i];
        printf("vt %f %f %f\n",vert.x, vert.y, vert.z);
    }

    for (usize i = 0; i < arr_len(mesh->normals); i++) {
        Vec3 vert = mesh->normals[i];
        printf("vn %f %f %f\n",vert.x, vert.y, vert.z);
    }

    for (usize i = 0; i < arr_len(mesh->faces); i++) {
        Face f = mesh->faces[i];
        printf("f %zu/%zu/%zu %zu/%zu/%zu %zu/%zu/%zu\n",
               f.vidxs[0], f.tidxs[1], f.nidxs[2],
               f.vidxs[0], f.tidxs[1], f.nidxs[2],
               f.vidxs[0], f.tidxs[1], f.nidxs[2]
            );
    }

    for (usize j=0; j < arr_len(mesh->faces); j++) {
        Face f  = mesh->faces[j];
        Vec3 v0 = mesh->vertices[f.vidxs[0]];
        Vec3 v1 = mesh->vertices[f.vidxs[1]];
        Vec3 v2 = mesh->vertices[f.vidxs[2]];

        printf("vf %f/%f/%f %f/%f/%f %f/%f/%f\n", 
            v0.x,v0.y,v0.z,
            v1.x,v1.y,v1.z,
            v2.x,v2.y,v2.z
        );    
    }
}

Vec3 mesh_center(Mesh *mesh) {
    Vec3 min = mesh->vertices[0];
    Vec3 max = mesh->vertices[0];

    for (usize i = 1; i < arr_len(mesh->vertices); i++) {
        Vec3 v = mesh->vertices[i];

        if (v.x < min.x) min.x = v.x;
        if (v.y < min.y) min.y = v.y;
        if (v.z < min.z) min.z = v.z;

        if (v.x > max.x) max.x = v.x;
        if (v.y > max.y) max.y = v.y;
        if (v.z > max.z) max.z = v.z;
    }

    Vec3 center = vec3(
        (min.x + max.x) * 0.5f,
        (min.y + max.y) * 0.5f,
        (min.z + max.z) * 0.5f
    );

    return center;
}

Texture *load_texture(const char* filepath) {
    Texture *tex = malloc(sizeof(Texture)); 
    if (tex == NULL) {
        LOG("Failed to Allocate the Texture %s\n", filepath);
        return NULL;
    }

    u8 *data = stbi_load(
        filepath,
        (i32 *)&tex->width,
        (i32 *)&tex->height,
        (i32 *)&tex->channels,
        STBI_rgb_alpha
    );

    if (data == NULL) {
        LOG("Failed to Load the Texture %s\n", stbi_failure_reason());
        free(tex);

        return NULL;
    }

    usize pixel_count = (usize)tex->width * tex->height;
    tex->pixels = malloc(pixel_count * sizeof(TexturePixel));
    if (tex->pixels == NULL) {
        LOG("Failed to Allocate the Texture %s\n", filepath);
        free(tex);

        return NULL;
    }

    memcpy(tex->pixels, data, pixel_count * sizeof(TexturePixel));
    stbi_image_free(data);
    return tex;
}

void free_tex(Texture* tex) {
    free(tex->pixels);

    tex->pixels = NULL;
    tex->width = 0;
    tex->height = 0;
    tex->channels = 0;
    free(tex);
}

Color sample2D(Texture *tex, f32 u, f32 v) {
    if (!tex || !tex->pixels) return ErrorColor;

    u = MAX(0.0f, MIN(1.0f, u));
    v = MAX(0.0f, MIN(1.0f, v));

    v = 1.0f - v; 

    i32 tex_x = (i32)(u * (tex->width  - 1));
    i32 tex_y = (i32)(v * (tex->height - 1));
    
    i32 idx = (tex_y * tex->width) + tex_x;
    if (idx < 0) {
        idx = 0;
    }

    TexturePixel texel = tex->pixels[idx];
    return (Color){
        .r = texel.r,
        .g = texel.g,
        .b = texel.b
    };
}

Color unlit_tex_colorizer(Fragment *frag, ColorizerData uniforms) {
    Material *mat = uniforms.material;
    if (mat->diffuse_map) {
        return sample2D(mat->diffuse_map, frag->u, frag->v);
    }
    
    return mat->albedo_color;
}

Color barrycentric_colorizer(Fragment *frag, ColorizerData uniforms) {
    UNUSED(uniforms);
    Color clr = {0};
    clr.r = (unsigned char)((frag->alpha) * RED.r + (frag->beta) * GREEN.r + (frag->gamma) * BLUE.r);
    clr.g = (unsigned char)((frag->alpha) * RED.g + (frag->beta) * GREEN.g + (frag->gamma) * BLUE.g);
    clr.b = (unsigned char)((frag->alpha) * RED.b + (frag->beta) * GREEN.b + (frag->gamma) * BLUE.b);

    return clr;
}

Color phong_tex_colorizer(Fragment *frag, ColorizerData uniforms) {
    Color base_color = unlit_tex_colorizer(frag, uniforms);
    Material *mat = uniforms.material;

    Vec3 N = NormV3(frag->normal);
    Vec3 L = NormV3(MulV3F(uniforms.directional_light_dir, -1.0f)); 
    Vec3 V = NormV3(SubV3(uniforms.camera_pos, frag->world_pos));  

    Vec3 ambient = vec3(
        base_color.r * uniforms.ambient_light.r,
        base_color.g * uniforms.ambient_light.g,
        base_color.b * uniforms.ambient_light.b
    );

    f32 diff_factor = MAX(0.0f, DotV3(N, L));
    Vec3 diffuse = vec3(
        base_color.r * diff_factor * (uniforms.directional_light_color.r / 255.0f),
        base_color.g * diff_factor * (uniforms.directional_light_color.g / 255.0f),
        base_color.b * diff_factor * (uniforms.directional_light_color.b / 255.0f)
    );

    Vec3 H = NormV3(AddV3(L, V)); 
    f32 spec_intense = powf(MAX(0.0f, DotV3(N, H)), mat->shininess > 0 ? mat->shininess : 32.0f); 
    f32 spec_factor = (mat->specular_strength > 0 ? mat->specular_strength : 0.5f) * spec_intense;
    
    Vec3 specular = vec3(
        255.0f * spec_factor * (uniforms.directional_light_color.r / 255.0f),
        255.0f * spec_factor * (uniforms.directional_light_color.g / 255.0f),
        255.0f * spec_factor * (uniforms.directional_light_color.b / 255.0f)
    );

    f32 final_r = MIN(255.0f, ambient.r + diffuse.r + specular.r);
    f32 final_g = MIN(255.0f, ambient.g + diffuse.g + specular.g);
    f32 final_b = MIN(255.0f, ambient.b + diffuse.b + specular.b);

    return (Color){
        .r = (u8)final_r,
        .g = (u8)final_g,
        .b = (u8)final_b,
    };
}

Mat4 Camera_GetView(const Camera *cam){
    return LookAtRH(
        cam->position,
        cam->target,
        cam->up
    );
}

Mat4 Camera_GetProjection(const Camera *cam){
    switch (cam->type) {

    case CAMERA_PERSPECTIVE:
        return PerspectiveRH_NO(
            cam->perspective.fov,
            cam->perspective.aspectR,
            cam->near_plane,
            cam->far_plane
        );

    case CAMERA_ORTHOGRAPHIC:
        return OrthographicRH_NO(
            cam->orthographic.left,
            cam->orthographic.right,
            cam->orthographic.bottom,
            cam->orthographic.top,
            cam->near_plane,
            cam->far_plane
        );
    }

    return Mat4Diagonal(1.0f);
}

static void putPixelOnFrame(Frame *frame, Vec2 pt, Pixel px) {
    if (!IsInFrame(
        pt.x, pt.y, 
        ctx.frame.width, ctx.frame.height)
    ) {
        return;
    }
    
    PIXEL(frame, pt.x, pt.y) = px;
}

static f32 edge_cross(Vec2 a, Vec2 b, Vec2 p) {
    Vec2 ab = SubV2(b,  a);
    Vec2 ap = SubV2(p,  a);
    return CrossV2(ab,  ap);
}

static bool is_top_left (Vec2 start, Vec2 end) {
    Vec2 edge = SubV2(end,  start);
    return ((edge.y == 0) && edge.x > 0) || (edge.y < 0);
}

static void drawTriangle(
    Frame *frame,
    ScreenVertex svs[3],
    Vec3 normals[3],
    Vec3 world_positions[3],
    FragColorizer  clrfn
) {

    ScreenVertex sv0 = svs[0];
    ScreenVertex sv1 = svs[1];
    ScreenVertex sv2 = svs[2];

    Vec2 v0 = {.x = sv0.pos.x, .y= sv0.pos.y};
    Vec2 v1 = {.x = sv1.pos.x, .y= sv1.pos.y};
    Vec2 v2 = {.x = sv2.pos.x, .y= sv2.pos.y};

    i32 minX = MIN(MIN(v0.x, v1.x), v2.x);
    i32 maxX = MAX(MAX(v0.x, v1.x), v2.x);
    i32 minY = MIN(MIN(v0.y, v1.y), v2.y);
    i32 maxY = MAX(MAX(v0.y, v1.y), v2.y);
    
    f32 area = edge_cross(v0, v1, v2);
    f32 inv_area = 1.0f / area;
    if (area >= 0) {
        return;
    }

    f32 inv_z0 = 1.0f / sv0.pos.z;
    f32 inv_z1 = 1.0f / sv1.pos.z;
    f32 inv_z2 = 1.0f / sv2.pos.z;

    f32 u0_over_z = sv0.uv.x * inv_z0;
    f32 v0_over_z = sv0.uv.y * inv_z0;

    f32 u1_over_z = sv1.uv.x * inv_z1;
    f32 v1_over_z = sv1.uv.y * inv_z1;

    f32 u2_over_z = sv2.uv.x * inv_z2;
    f32 v2_over_z = sv2.uv.y * inv_z2;

    f32 nx0_z = normals[0].x * inv_z0; 
    f32 ny0_z = normals[0].y * inv_z0; 
    f32 nz0_z = normals[0].z * inv_z0;

    f32 nx1_z = normals[1].x * inv_z1; 
    f32 ny1_z = normals[1].y * inv_z1; 
    f32 nz1_z = normals[1].z * inv_z1;

    f32 nx2_z = normals[2].x * inv_z2; 
    f32 ny2_z = normals[2].y * inv_z2; 
    f32 nz2_z = normals[2].z * inv_z2;

    f32 wx0_z = world_positions[0].x * inv_z0; 
    f32 wy0_z = world_positions[0].y * inv_z0; 
    f32 wz0_z = world_positions[0].z * inv_z0;

    f32 wx1_z = world_positions[1].x * inv_z1; 
    f32 wy1_z = world_positions[1].y * inv_z1; 
    f32 wz1_z = world_positions[1].z * inv_z1;

    f32 wx2_z = world_positions[2].x * inv_z2; 
    f32 wy2_z = world_positions[2].y * inv_z2; 
    f32 wz2_z = world_positions[2].z * inv_z2;

    f32 delta_w0_col = (v1.y - v2.y);
    f32 delta_w1_col = (v2.y - v0.y);
    f32 delta_w2_col = (v0.y - v1.y);

    f32 delta_w0_row = (v2.x - v1.x);
    f32 delta_w1_row = (v0.x - v2.x);
    f32 delta_w2_row = (v1.x - v0.x);

    f32 bias_w0 = is_top_left(v1, v2) ? 0 : -0.0000001f;
    f32 bias_w1 = is_top_left(v2, v0) ? 0 : -0.0000001f;
    f32 bias_w2 = is_top_left(v0, v1) ? 0 : -0.0000001f;  

    Vec2 p0 = vec2(minX, minY);

    f32 w0_row = edge_cross(v1, v2, p0) + bias_w0;
    f32 w1_row = edge_cross(v2, v0, p0) + bias_w1;
    f32 w2_row = edge_cross(v0, v1, p0) + bias_w2;
 
    Fragment frag;
    for (int y = minY; y <= maxY; y++) {
        f32 w0 = w0_row;
        f32 w1 = w1_row;
        f32 w2 = w2_row;

        for (int x = minX; x <= maxX; x++) {
            frag.screen.x = x;
            frag.screen.y = y;

            frag.alpha  = w0 * inv_area;
            frag.beta   = w1 * inv_area;
            frag.gamma  = w2 * inv_area;

            float inv_z = (frag.alpha * inv_z0) + 
                          (frag.beta  * inv_z1) + 
                          (frag.gamma * inv_z2);

            frag.depth = 1.0f / inv_z;

            if (
                (w0 >= 0 && w1 >= 0 && w2 >= 0)
                || (w0 < 0 && w1 < 0 && w2 < 0)
            ) {
                if (IsInFrame(x, y, ctx.frame.width, ctx.frame.height) && frag.depth <= DEPTH(frame, x, y)) {
                    DEPTH(frame, x, y) = frag.depth;

                    f32 u_over_z = (frag.alpha * u0_over_z) + 
                                     (frag.beta  * u1_over_z) + 
                                     (frag.gamma * u2_over_z);
                                     
                    f32 v_over_z = (frag.alpha * v0_over_z) + 
                                     (frag.beta  * v1_over_z) + 
                                     (frag.gamma * v2_over_z);

                    frag.u = u_over_z * frag.depth; 
                    frag.v = v_over_z * frag.depth;

                    f32 nx_z = (frag.alpha * nx0_z) + (frag.beta * nx1_z) + (frag.gamma * nx2_z);
                    f32 ny_z = (frag.alpha * ny0_z) + (frag.beta * ny1_z) + (frag.gamma * ny2_z);
                    f32 nz_z = (frag.alpha * nz0_z) + (frag.beta * nz1_z) + (frag.gamma * nz2_z);

                    Vec3 normal = vec3(
                            nx_z * frag.depth, 
                            ny_z * frag.depth, 
                            nz_z * frag.depth
                    );
                    frag.normal = normal;

                    f32 wx_z = (frag.alpha * wx0_z) + (frag.beta * wx1_z) + (frag.gamma * wx2_z);
                    f32 wy_z = (frag.alpha * wy0_z) + (frag.beta * wy1_z) + (frag.gamma * wy2_z);
                    f32 wz_z = (frag.alpha * wz0_z) + (frag.beta * wz1_z) + (frag.gamma * wz2_z);

                    frag.world_pos.x = wx_z * frag.depth;
                    frag.world_pos.y = wy_z * frag.depth;
                    frag.world_pos.z = wz_z * frag.depth;

                    putPixelOnFrame(
                        frame, 
                        frag.screen, 
                        clrfn(&frag, ctx.data) 
                    );
                }
            }

             w0 += delta_w0_col;
             w1 += delta_w1_col;
             w2 += delta_w2_col;
        }

         w0_row += delta_w0_row;
         w1_row += delta_w1_row;
         w2_row += delta_w2_row;

    }
}

// The points need to be screen points btw!!!!!
void DrawLine(Vec2 p1, Vec2 p2, Color color) {
    i32 dx = abs((i32)(p2.x - p1.x));
    i32 dy = abs((i32)(p2.y - p1.y));

    i32 sx = (p2.x > p1.x) ? 1 : -1;
    i32 sy = (p2.y > p1.y) ? 1 : -1;

    i32 p = 0;
    i32 x = p1.x;
    i32 y = p1.y;

    if (dx > dy) {
        p = 2 * dy - dx;
        for (int i = 0; i <= dx; i++) {
            if (IsInFrame(x, y, ctx.frame.width, ctx.frame.height)) {
                PIXEL(&ctx.frame, x, y) = color;
            }

            if (p < 0) {
                p = p + 2 * dy;
            } else {
                y += sy;
                p += 2 * (dy - dx);
            }

            x += sx;
        }
    } else {
        p = 2 * dx - dy;
        for (int i = 0; i <= dy; i++) {
            if (IsInFrame(x, y, ctx.frame.width, ctx.frame.height)) {
                PIXEL(&ctx.frame, x, y) = color;
            }

            if (p < 0) {
                p = p + 2 * dx;
            } else {
                x += sx;
                p += 2 * (dx - dy);
            }

            y += sy;
        }
    }
}

void drawTriangle_wf(ScreenVertex svs[3],Color clr) { 
    Vec2 p1 = {.x = svs[0].pos.x, .y= svs[0].pos.y};
    Vec2 p2 = {.x = svs[1].pos.x, .y= svs[1].pos.y};
    Vec2 p3 = {.x = svs[2].pos.x, .y= svs[2].pos.y};

    DrawLine(p2, p3, clr);
    DrawLine(p3, p1, clr);
    DrawLine(p1, p2, clr);
}

void DrawMeshWireFrame(
    const Mesh *mesh, 
    Material *mat, 
    Mat4 model, 
    Mat4 view, 
    Mat4 proj
) {
    if(mesh == NULL || mat == NULL) return;     
   
    ctx.data.material = mat;
    Mat4 mvp = MVP(model,  view,  proj);

    for (usize i = 0; i < arr_len(mesh->faces); i++) {
        Face f = mesh->faces[i];
        ScreenVertex screen_verts[3];

        for (usize j = 0; j < 3; j++) {
            Vec3 v_local = mesh->vertices[f.vidxs[j]];
            
            Vec4 v_clip = MulMat4Vec4(mvp, vec4V(v_local, 1.0f));
            if (v_clip.w == 0.0f) v_clip.w = F32_EPSILON; 
            Vec3 v_ndc = DivV3F(v_clip.xyz, v_clip.w);
            
            f32 half_w = ctx.frame.width * 0.5f;
            f32 half_h = ctx.frame.height * 0.5f;
            
            screen_verts[j].pos.x = (v_ndc.x + 1.0f) * half_w;
            screen_verts[j].pos.y = (1.0f - v_ndc.y) * half_h;
            screen_verts[j].pos.z = v_clip.w; 
        }
        
        drawTriangle_wf(screen_verts,mat->albedo_color);
    }
}

void DrawMesh(
    const Mesh *mesh, 
    Material *mat, 
    Mat4 model, 
    Mat4 view, 
    Mat4 proj
) {
    if(mesh == NULL || mat == NULL) return;     
   
    ctx.data.material = mat;
    Mat4 mvp = MVP(model,  view,  proj);
    Mat4 normal_mat = Mat4Transpose(Mat4InvGen(model));

    for (usize i = 0; i < arr_len(mesh->faces); i++) {
        Face f = mesh->faces[i];
        
        ScreenVertex screen_verts[3];
        Vec3 face_normals[3];
        Vec3 face_world_pos[3];

        for (usize j = 0; j < 3; j++) {
            Vec3 v_local = mesh->vertices[f.vidxs[j]];
            Vec3 uv_local = mesh->uvs[f.tidxs[j]];
            Vec3 n_local = mesh->normals[f.nidxs[j]];
            
            Vec4 v_clip = MulMat4Vec4(mvp, vec4V(v_local, 1.0f));
            if (v_clip.w == 0.0f) v_clip.w = F32_EPSILON; 

            Vec3 v_ndc = DivV3F(v_clip.xyz, v_clip.w);
            
            f32 half_w = ctx.frame.width * 0.5f;
            f32 half_h = ctx.frame.height * 0.5f;
            
            screen_verts[j].pos.x = (v_ndc.x + 1.0f) * half_w;
            screen_verts[j].pos.y = (1.0f - v_ndc.y) * half_h;
            screen_verts[j].pos.z = v_clip.w; 
            screen_verts[j].uv = uv_local;

            Vec4 v_world4 = MulMat4Vec4(model, vec4V(v_local, 1.0f));
            face_world_pos[j] = v_world4.xyz;
            
            Vec4 n_world4 = MulMat4Vec4(normal_mat, vec4V(n_local, 0.0f));
            face_normals[j] = NormV3(n_world4.xyz);
        }
        
        drawTriangle(&ctx.frame, screen_verts, face_normals,face_world_pos, ctx.fcl);
    }

}

void DrawMeshInstance(const MeshInstance *instance) {
    if (!instance || !instance->mesh || !instance->mat) return;
    DrawMesh(
        instance->mesh, 
        instance->mat, 
        instance->transform, 
        ctx.view, 
        ctx.projection
    );  
}

void DrawMeshInstanceWF(const MeshInstance *instance) {
    if (!instance || !instance->mesh || !instance->mat) return;
    DrawMeshWireFrame(
        instance->mesh, 
        instance->mat, 
        instance->transform, 
        ctx.view, 
        ctx.projection
    );  
}
