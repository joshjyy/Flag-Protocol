#include "particle.hpp"
#include "utils.hpp"
#include <stdlib.h>
#include <math.h>

float RandFloat(float min, float max) {
    return min + (max - min) * ((float)rand() / RAND_MAX);
}

ParticleEmitter* ParticleEmitterCreate(void) {
    ParticleEmitter* emitter = new ParticleEmitter();
    //printf("ParticleEmitterCreate: allocated at %p, size = %zu bytes\n", emitter, sizeof(ParticleEmitter));

    emitter->emissionRate = 10.0f;
    emitter->minLifeTime = 1.0f;
    emitter->maxLifeTime = 2.0f;
    emitter->minScale = 2.0f;
    emitter->maxScale = 10.0f;
    emitter->startA = 1.0f;
    return emitter;
}


void ParticleEmitterFree(ParticleEmitter** emitter) {
    printf("ParticleEmitterFree called\n");

    if (*emitter) {
        printf("Deleting emitter at %p\n", *emitter);

        delete (*emitter);
        *emitter = nullptr;
        printf("Emitter freed\n");

    }
}

void ParticleEmitterSetMesh(ParticleEmitter* emitter, AEGfxVertexList* mesh) {
    if (!emitter) return;

    emitter->mesh = mesh;
}
// Setters for emitter properties
void ParticleEmitterSetLifeTime(ParticleEmitter* emitter, float min, float max) {
    if (!emitter) return;

    emitter->minLifeTime = min;
    emitter->maxLifeTime = max;
}

void ParticleEmitterSetVelocity(ParticleEmitter* emitter, float minX, float maxX, float minY, float maxY) {
    if (!emitter) return;

    emitter->minVelX = minX;
    emitter->maxVelX = maxX;
    emitter->minVelY = minY;
    emitter->maxVelY = maxY;
}

void ParticleEmitterSetScale(ParticleEmitter* emitter, float min, float max) {
    if (!emitter) return;

    emitter->minScale = min;
    emitter->maxScale = max;
}

void ParticleEmitterSetColor(ParticleEmitter* emitter, float r, float g, float b, float a) {
    if (!emitter) return;
    emitter->startR = r;
    emitter->startG = g;
    emitter->startB = b; 
    emitter->startA = a;

}


void ParticleEmitterSetEmissionRate(ParticleEmitter* emitter, float rate) {
    if (!emitter) return;

    emitter->emissionRate = rate;
}

void ParticleEmitterEmit(ParticleEmitter* emitter, float x, float y, int count) {
    if (!emitter) return;
	// Emit 'count' particles at (x, y)
    for (int i = 0; i < count; i++) {
		// Find an inactive particle slot

        for (int j = 0; j < MAX_PARTICLES; j++) {

			// If found inactive particle, initialize it and break to move to next particle to emit
            if (!emitter->particles[j].active) {
                Particle* p = &emitter->particles[j];
                p->x = x;
                p->y = y;
                p->velX = RandFloat(emitter->minVelX, emitter->maxVelX);
                p->velY = RandFloat(emitter->minVelY, emitter->maxVelY);
                p->scale = RandFloat(emitter->minScale, emitter->maxScale);
                p->maxLife = p->life = RandFloat(emitter->minLifeTime, emitter->maxLifeTime);
                p->r = emitter->startR; 
                p->g = emitter->startG;
                p->b = emitter->startB;
                p->a = emitter->startA;
         
                p->active = true;
                break;
            }
        }
    }
}

// Update particles: move them, and deactivate when life is over
void ParticleEmitterUpdate(ParticleEmitter* emitter, float dt) {
    if (!emitter) return;

	//loop through particles and update them
    for (int i = 0; i < MAX_PARTICLES; i++) {
		// Skip inactive particles
        Particle* p = &emitter->particles[i];
        if (!p->active) continue;
		// Update life with dt
        p->life -= dt;

		// Deactivate if life is over
        if (p->life <= 0.0f) {
            p->active = false;
            continue;
        }

		// Update position based on velocity
        p->x += p->velX * dt;
        p->y += p->velY * dt;


    }
}

void ParticleEmitterDraw(ParticleEmitter* emitter) {
    if (!emitter || !emitter->mesh) return;

	// Draw active particles
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle* p = &emitter->particles[i];
        if (!p->active) continue;

        AEGfxSetTransparency(p->a);
        AEGfxSetColorToMultiply(p->r, p->g, p->b, 1.0f);
        AEGfxSetColorToAdd(0, 0, 0, 0);

        DrawSprite(emitter->mesh, p->x, p->y, p->scale, p->scale, 0.0f);
    }
}

