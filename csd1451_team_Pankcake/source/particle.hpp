#pragma once
#include "AEEngine.h"

#define MAX_PARTICLES 1000


typedef struct Particle {
    float x, y;
    float velX, velY;
    float scale;
    float life;
    float maxLife;
    float r, g, b, a;
    float endR, endG, endB, endA;
    bool active;
} Particle;

typedef struct ParticleEmitter {
    Particle particles[MAX_PARTICLES];
    AEGfxVertexList* mesh;
    float minLifeTime, maxLifeTime;
    float minVelX, maxVelX, minVelY, maxVelY;
    float minScale, maxScale;
    float startR, startG, startB, startA;
    float endR, endG, endB, endA;
    float emissionRate;
    float emissionTimer;
} ParticleEmitter;

ParticleEmitter* ParticleEmitterCreate(void);
void ParticleEmitterFree(ParticleEmitter** emitter);
void ParticleEmitterSetMesh(ParticleEmitter* emitter, AEGfxVertexList* mesh);
void ParticleEmitterSetLifeTime(ParticleEmitter* emitter, float min, float max);
void ParticleEmitterSetVelocity(ParticleEmitter* emitter, float minX, float maxX, float minY, float maxY);
void ParticleEmitterSetScale(ParticleEmitter* emitter, float min, float max);
void ParticleEmitterSetColor(ParticleEmitter* emitter, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2);
void ParticleEmitterSetEmissionRate(ParticleEmitter* emitter, float rate);
void ParticleEmitterEmit(ParticleEmitter* emitter, float x, float y, int count);
void ParticleEmitterUpdate(ParticleEmitter* emitter, float dt);
void ParticleEmitterDraw(ParticleEmitter* emitter);

float RandFloat(float min, float max);

