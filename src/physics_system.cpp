// internal
#include <iostream>
#include "physics_system.hpp"
#include "world_init.hpp"

const float COLLISION_THRESHOLD = 0.0f;

void PhysicsSystem::init(RenderSystem* renderer_arg) {
    this->renderer = renderer_arg;
}

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion &motion)
{
    // abs is to avoid negative scale due to the facing direction.
    return {abs(motion.scale.x), abs(motion.scale.y)};
}

bool check_collision_conditions(Entity entity_i, Entity entity_j) {
    if (registry.players.has(entity_i)) {
        if (registry.enemies.has(entity_j) ||
            registry.spitterBullets.has(entity_j) ||
            registry.collectables.has(entity_j) ||
            registry.weaponHitBoxes.has(entity_j))
        {
            return true;
        }
    } else if (registry.weaponHitBoxes.has(entity_i)) {
        if (registry.enemies.has(entity_j) || registry.blocks.has(entity_j) || registry.spitterBullets.has(entity_j))
        {
            return true;
        }
    } else if (registry.blocks.has(entity_i) && (registry.solids.has(entity_j) || registry.projectiles.has(entity_j))) {
        return true;
    } else if (registry.parallaxBackgrounds.has(entity_i) || registry.blocks.has(entity_i)) {
        if (registry.bullets.has(entity_j) ||
            registry.rockets.has(entity_j) ||
            registry.grenades.has(entity_j) ||
            registry.spitterBullets.has(entity_j) ||
            registry.collectables.has(entity_j) ||
            registry.players.has(entity_j) ||
            registry.boulders.has(entity_j)) 
        {
            return true;
        }
    }
    return false;
}

vec2 get_parametrics(vec2 p1, vec2 c1, vec2 p2, vec2 c2) {
    vec2 t;
    if (c1.x == 0) {
        t.y = (p1.x - p2.x + c1.x * p2.y / c1.y - c1.x * p1.y / c1.y) / (c2.x - c1.x * c2.y / c1.y);
        t.x = (p2.y - p1.y + t.y*c2.y) / c1.y;
    } else {
        t.y = (p1.y - p2.y + c1.y * p2.x / c1.x - c1.y * p1.x / c1.x) / (c2.y - c1.y * c2.x / c1.x);
        t.x = (p2.x - p1.x + t.y*c2.x) / c1.x;
    }
    return t;
}

bool check_intersection(vec2 p1, vec2 q1, vec2 p2, vec2 q2) {
    vec2 c1 = q1 - p1;
    vec2 c2 = q2 - p2;
    if (c1.x * c2.y == c1.y * c2.x)
        return false;

    vec2 t = get_parametrics(p1, c1, p2, c2);

    if (t.x < 0 || t.x > 1 || t.y < 0 || t.y > 1)
        return false;
    return true;
}

bool check_inside(vec2 p1, vec2 q1, vec2 p2, vec2 q2) {
    vec2 c1 = q1 - p1;
    vec2 c2 = q2 - p2;
    if (c2.x == 0 && c2.y == 0)
        return true;
    
    if (c1.x * c2.y == c1.y * c2.x)
        return false;

    vec2 t = get_parametrics(p1, c1, p2, c2);

    if (t.x >= 0 && t.x <= 1 && t.y > 1)
        return true;
    return false;
}

bool precise_collision(const Entity& entity1, const Entity& entity2) {
    Motion& motion1 = registry.motions.get(entity1);
    Motion& motion2 = registry.motions.get(entity2);
    CollisionMesh* mesh1 = registry.collisionMeshPtrs.get(entity1);
    CollisionMesh* mesh2 = registry.collisionMeshPtrs.get(entity2);
    
    std::vector<ColoredVertex> vertices1 = mesh1->vertices;
    mat2 rotation_matrix1 = mat2({cos(motion1.angle), -sin(motion1.angle)}, {sin(motion1.angle), cos(motion1.angle)});
    for (ColoredVertex& vertex: vertices1) {
        vertex.position = vec3(motion1.position + (motion1.positionOffset + vec2(vertex.position.x * motion1.scale.x, vertex.position.y * motion1.scale.y)) * rotation_matrix1, 0);
    }
    
    std::vector<ColoredVertex> vertices2 = mesh2->vertices;
    mat2 rotation_matrix2 = mat2({cos(motion2.angle), -sin(motion2.angle)}, {sin(motion2.angle), cos(motion2.angle)});
    for (ColoredVertex& vertex: vertices2) {
        vertex.position = vec3(motion2.position + (motion2.positionOffset + vec2(vertex.position.x * motion2.scale.x, vertex.position.y * motion2.scale.y)) * rotation_matrix2, 0);
    }

    for (std::pair<int, int> edge1: mesh1->edges) {
        for (std::pair<int, int> edge2: mesh2->edges) {
            if (check_intersection(vertices1[edge1.first - 1].position, vertices1[edge1.second - 1].position, vertices2[edge2.first - 1].position, vertices2[edge2.second - 1].position))
                return true;
        }
        if (check_inside(vertices1[edge1.first - 1].position, vertices1[edge1.second - 1].position, motion1.position + motion1.positionOffset * rotation_matrix1, motion2.position + motion2.positionOffset * rotation_matrix2))
            return true;
    }

    for (std::pair<int, int> edge2: mesh2->edges) {
        if (check_inside(vertices2[edge2.first - 1].position, vertices2[edge2.second - 1].position, motion2.position + motion2.positionOffset * rotation_matrix2, motion1.position + motion1.positionOffset * rotation_matrix1))
            return true;
    }

    return false;
}

bool PhysicsSystem::collides(const Entity &entity1, const Entity &entity2)
{
    Motion& motion1 = registry.motions.get(entity1);
    Motion& motion2 = registry.motions.get(entity2);
    if (registry.lasers.has(entity1) || registry.lasers.has(entity2)) {
        return precise_collision(entity1, entity2);
    }
    vec2 scale1 = get_bounding_box(motion1) / 2.0f;
    vec2 scale2 = get_bounding_box(motion2) / 2.0f;
    if (abs(motion1.position.x - motion2.position.x) < (scale1.x + scale2.x) &&
        abs(motion1.position.y - motion2.position.y) < (scale1.y + scale2.y))
    {
        if (!registry.collisionMeshPtrs.get(entity1)->is_sprite || !registry.collisionMeshPtrs.get(entity2)->is_sprite)
            return precise_collision(entity1, entity2);
        else
            return true;
    }
    return false;
}

void PhysicsSystem::step(float elapsed_ms, int dialogue)
{
    // Move fish based on how much time has passed, this is to (partially) avoid
    // having entities move at different speed based on the machine.
    auto &motion_container = registry.motions;
    for (uint i = 0; i < motion_container.size(); i++)
    {
        Motion &motion = motion_container.components[i];
        Entity entity = motion_container.entities[i];
        float step_seconds = elapsed_ms / 1000.f;
        if (registry.dialogues.has(entity) || registry.dialogueTexts.has(entity)) {
            // move dialogue only if it's not centered
            if (motion.position.x > window_width_px / 2) {
                motion.position += motion.velocity * step_seconds;
            }
        } else {
            // move only if no dialogues are shown
            if (dialogue == 0) {
                if (registry.gravities.has(entity)) {
                    Gravity& gravity = registry.gravities.get(entity);
                    if (!gravity.lodged.test(0) && !gravity.lodged.test(1) && !gravity.dashing)
                        motion.velocity[1] += GRAVITY_ACCELERATION_FACTOR * elapsed_ms;
                }
                motion.position += motion.velocity * step_seconds;
            }
        }
    }

    // Check for collisions between all entities with meshes
    for (uint i = 0; i < registry.collisionMeshPtrs.size(); i++) {
        for (uint j = i + 1; j < registry.collisionMeshPtrs.size(); j++) {
            Entity entity_i = registry.collisionMeshPtrs.entities[i];
            Entity entity_j = registry.collisionMeshPtrs.entities[j];            
            if ((check_collision_conditions(entity_i, entity_j) || check_collision_conditions(entity_j, entity_i)) && PhysicsSystem::collides(entity_i, entity_j)) {
                registry.collisions.emplace_with_duplicates(entity_i, entity_j);
                registry.collisions.emplace_with_duplicates(entity_j, entity_i);
            }
        }
    }
}