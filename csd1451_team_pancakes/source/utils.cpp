
#include "utils.hpp"
#include "AEEngine.h"

int AreCirclesIntersecting(float circle_center1_x, float circle_center1_y, float radius_1, float circle_center2_x, float circle_center2_y, float radius_2) {

    float dist_between_pt = sqrtf((circle_center2_x - circle_center1_x) * (circle_center2_x - circle_center1_x) +
        (circle_center2_y - circle_center1_y) * (circle_center2_y - circle_center1_y)); // distance between circle centers

    float sum_of_radii = radius_1 + radius_2; //since both circles have same diameter, radius1 + radius2 = diameter

    float diff_of_radii = radius_1 - radius_2; //difference of radii is zero since both circles have same diameter


    if (dist_between_pt <= diff_of_radii) {
        return 2; //one circle is inside the other
    }

    else if (dist_between_pt <= sum_of_radii) {
        return 1; //circles are colliding
    }

    else {
        return 0; //circles are not colliding
    }

}

//Uncomment for box collision
int AreBoxesIntersecting(float box1_center_x, float box1_center_y, float box1_width, float box1_height,
    float box2_center_x, float box2_center_y, float box2_width, float box2_height) {
    float halfW1 = box1_width / 2.0f;
    float halfH1 = box1_height / 2.0f;
    float halfW2 = box2_width / 2.0f;
    float halfH2 = box2_height / 2.0f;

    int overlapX = (box1_center_x - halfW1 < box2_center_x + halfW2) &&
        (box1_center_x + halfW1 > box2_center_x - halfW2);

    int overlapY = (box1_center_y - halfH1 < box2_center_y + halfH2) &&
        (box1_center_y + halfH1 > box2_center_y - halfH2);

    return (overlapX && overlapY) ? 1 : 0;
}

void DrawSprite(AEGfxVertexList* mesh, float x, float y, float scaleX, float scaleY, float rotation) {
    AEMtx33 scale = { 0 };
    AEMtx33Scale(&scale, scaleX, scaleY);

    AEMtx33 rotate = { 0 };
    AEMtx33Rot(&rotate, rotation);

    AEMtx33 translate = { 0 };
    AEMtx33Trans(&translate, x, y);

    AEMtx33 transform = { 0 };
    AEMtx33Concat(&transform, &rotate, &scale);
    AEMtx33Concat(&transform, &translate, &transform);

    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}