#include <iostream>

#define ARC_ECS_IMPLEMENTATION
#include "../arc_ecs.hpp"

#include "testlib.h"


struct Pos {
    float x, y, z;
};

struct Color {
    float r,g,b,a;
};

arc::ecs::World world;

class GravitySystem : arc::ecs::System {
public:
    void update(float _deltatime) override {
        std::size_t i = 0;
        for (auto const& entity : m_entities) {
            auto& pos = world.get_component<Pos>(entity);
            pos.z -= 9.82 * _deltatime;
            std::cout << "pos(" << i++ << ") " << pos.x << " " << pos.y << " "<< pos.z
                      << std::endl;
        }
    }
};

void
test_move_pos(void)
{
    world.register_component<Pos>();

    /*Setup gravity system*/
    auto gravity_system = world.register_system<GravitySystem>();
    arc::ecs::Signature gravity_signature;
    gravity_signature.set(world.get_component_type<Pos>());

    /*create entities*/
    std::vector<arc::ecs::Entity> entities(3);
    int i = 0;
    for (auto& e : entities) {
        e = world.create_entity();
        world.add_component(e, Pos{(float)i, (float)i, (float)i++});
    }

    /*run system simulation*/
    int nmax = 10;
    float dt = 0; 
    for (int n = 0; n < nmax; n++) {
        gravity_system->update(dt);
        dt += 0.16;
    }
}

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

    TL(test_move_pos());

    tl_summary();
}
