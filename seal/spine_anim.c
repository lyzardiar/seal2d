#include "seal.h"
#include "render.h"
#include "renders/sprite_render.h"
#include "texture.h"
#include "platform/fs.h"
#include "math/geo.h"

#include "spine/spine.h"
#include "spine_anim.h"

EXTERN_GAME;

void _spAtlasPage_createTexture (spAtlasPage* self, const char* path)
{
    struct texture* tex = texture_cache_load(GAME->texture_cache, path);
    self->rendererObject = tex;
    self->width = tex->width;
    self->height = tex->height;
}

void _spAtlasPage_disposeTexture (spAtlasPage* self)
{
    texture_unload(self->rendererObject);
}

char* _spUtil_readFile (const char* path, int* length)
{
    size_t len = 0;
    char* data = (char*)fs_read(path, &len, 0);
    *length = len;
    return data;
}

struct spine_anim* spine_anim_new(const char* atlas_path,
                                  const char* spine_data_path,
                                  float scale)
{
    struct spine_anim* self = STRUCT_NEW(spine_anim);
    memset(self->vertices, 0, sizeof(self->vertices));
    memset(self->uvs, 0, sizeof(self->uvs));

    // step 1: load atlas
    spAtlas* atlas = spAtlas_createFromFile(atlas_path, NULL);
    if (!atlas) {
        fprintf(stderr, "error loading spine atlas file = %s.", atlas_path);
        return NULL;
    }

    // step 2: load skeleton data
    spSkeletonJson* json = spSkeletonJson_create(atlas);
    json->scale = scale;

    spSkeletonData* skeletonData = spSkeletonJson_readSkeletonDataFile(json, spine_data_path);
    if (!skeletonData || json->error) {
        fprintf(stderr, "error reading skeleton json, error = %s", json->error);
        spSkeletonJson_dispose(json);
        return NULL;
    }
    spSkeletonJson_dispose(json);

    // step 3: load skeleton
    spSkeleton* skeleton = spSkeleton_create(skeletonData);

    // step 4: load animation state and init
    spAnimationStateData* state_data = spAnimationStateData_create(skeletonData);
    spAnimationState* state = spAnimationState_create(state_data);

    self->atlas = atlas;
    self->skeleton = skeleton;
    self->anim_state = state;

    return self;
}

void spine_anim_free(struct spine_anim* self)
{
    spAtlas_dispose(self->atlas);
}

void spine_anim_update(struct spine_anim* self, float dt)
{
    spAnimationState_update(self->anim_state, dt);
    spAnimationState_apply(self->anim_state, self->skeleton);
    spSkeleton_updateWorldTransform(self->skeleton);
}

static void spine_anim_render_region(struct spine_anim* self,
                                     struct render* R,
                                     struct sprite* spr,
                                     spSlot* slot,
                                     spAttachment* attachment)
{
    spRegionAttachment* region = (spRegionAttachment*)slot->attachment;
    spRegionAttachment_computeWorldVertices(region, slot->bone, self->vertices);
    struct texture* texture =(struct texture*)((spAtlasRegion*)region->rendererObject)->page->rendererObject;
    memcpy(self->uvs, region->uvs, sizeof(self->uvs));

    // coord of spine
    // (x2, y2)------(x1, y1)
    //  |               |
    //  |               |
    //  |               |
    // (x3, y3)------(x4, y4)
    float* vert = self->vertices;
    float *uvs = region->uvs;

    struct glyph* g = &spr->glyph;

    g->tr.position[0] = vert[0];
    g->tr.position[1] = vert[1];
    g->tl.position[0] = vert[2];
    g->tl.position[1] = vert[3];
    g->bl.position[0] = vert[4];
    g->bl.position[1] = vert[5];
    g->br.position[0] = vert[6];
    g->br.position[1] = vert[7];

    g->tr.uv[0] = uvs[0];
    g->tr.uv[1] = uvs[1];
    g->tl.uv[0] = uvs[2];
    g->tl.uv[1] = uvs[3];
    g->bl.uv[0] = uvs[4];
    g->bl.uv[1] = uvs[5];
    g->br.uv[0] = uvs[6];
    g->br.uv[1] = uvs[7];

    SET_VERTEX_COLOR(g->bl, 255, 255, 255, 255);
    SET_VERTEX_COLOR(g->tr, 255, 255, 255, 255);
    SET_VERTEX_COLOR(g->tl, 255, 255, 255, 255);
    SET_VERTEX_COLOR(g->br, 255, 255, 255, 255);
    for (int i = 0; i < 8; i+=2) {
        printf("vertice[%d] = (%.2f, %.2f) ", i, self->vertices[i], self->vertices[i+1]);
        printf("uv[%d] = (%.2f, %.2f) \n", i, uvs[i], uvs[i+1]);
    }

    render_switch(R, RENDER_TYPE_SPRITE);
    sprite_render_func_draw(R, spr);
}

void spine_anim_draw(struct spine_anim* self, struct render* R, struct sprite* spr)
{
    spSkeleton* skeleton = self->skeleton;
    int i = 0;
    int n_slots = skeleton->slotsCount;
    for (; i < n_slots ; ++i) {
        spSlot* slot = skeleton->drawOrder[i];
        spAttachment* attachment = slot->attachment;
        if (attachment) {
            switch (attachment->type) {
                case SP_ATTACHMENT_REGION: {
                    spine_anim_render_region(self, R, spr, slot, attachment);
                    break;
                }

                default:
                    break;
            }
        }
    }
}

void spine_anim_set_anim(struct spine_anim* self, const char* anim_name)
{

}

void spine_get_boundingbox(struct spine_anim* self, struct rect* r)
{

}