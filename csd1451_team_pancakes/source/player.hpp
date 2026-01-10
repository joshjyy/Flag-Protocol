#ifndef BULLET_H
#define BULLET_H
// Bullet structure
struct Bullet {
    float x;
    float y;
    float velocityX;
    float velocityY;
    bool active; 
    Bullet* next;  // Pointer to next bullet in linked list
};

Bullet* CreateBullet(float x, float y, float targetX, float targetY, float speed);

void NormalizeVector(float* x, float* y);
void UpdateBullets(Bullet** bulletHead, float dt);
void RenderBullets(Bullet* bulletHead, void* mesh);
void FreeBullets(Bullet** bulletHead);
int GetBulletCount(Bullet* bulletHead);

#endif 