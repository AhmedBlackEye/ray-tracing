#include "texture.h"

void texture_destroy(Texture *self) {
    if (self && self->destroy) {
        self->destroy(self);
    }
}