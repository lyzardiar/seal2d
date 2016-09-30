#include "seal.h"
#include "render.h"
#include "renders/spine_render.h"
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
                                     float x, float y,
                                     spSlot* slot,
                                     spAttachment* attachment)
{
    spRegionAttachment* region = (spRegionAttachment*)slot->attachment;
    spRegionAttachment_computeWorldVertices(region, slot->bone, self->vertices);
    struct texture* texture =(struct texture*)((spAtlasRegion*)region->rendererObject)->page->rendererObject;

    struct spine_render_vertex_desc d = {
        self->vertices,
        region->uvs,
        NULL,
        texture->id,
        x, y
    };

    spine_render_func_draw(R, &d);
}

void spine_anim_draw(struct spine_anim* self,
                     struct render* R,
                     float x, float y)
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
                    spine_anim_render_region(self, R, x, y, slot, attachment);
                    break;
                }

                default:
                    break;
            }
        }
    }
}

void spine_anim_set_anim(struct spine_anim* self, const char* anim_name, int track, bool loop)
{
    spAnimationState_setAnimationByName(self->anim_state, track, anim_name, true);
}

void spine_get_boundingbox(struct spine_anim* self, struct rect* r)
{

}