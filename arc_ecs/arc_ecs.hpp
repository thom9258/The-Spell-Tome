#include <bitset>
#include <set>
#include <queue>
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <functional>
#include <cassert>
#include <memory>
#include <unordered_map>

/*NOTE: this is basically a copy-paste from:
        https://austinmorlan.com/posts/entity_component_system/
*/

#ifndef ARC_ECS_H
#define ARC_ECS_H

namespace arc { namespace ecs {

#define ASSERT_UNREACHABLE(str) assert(str && "Unmanaged execution path observed!")  
#define UNUSED(v) ((void)v)

using Entity = std::uint32_t;
const Entity MAX_ENTITIES = 5000;

using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;


class EntityManager
{
    std::queue<Entity> m_available_entities{};
    std::array<Signature, MAX_ENTITIES> m_signatures{};
    uint32_t m_living_entity_count{};

public:
    EntityManager(void);
    Entity create_entity(void);
    void destroy_entity(Entity _e);
    void set_signature(Entity _e, Signature _s);
    Signature get_signature(Entity _e);
};

class TComponentArray
{
public:
    virtual ~TComponentArray(void) = default;
    virtual void entity_destroyed(Entity _e) = 0;
};

template <typename T>
class ComponentArray : public TComponentArray
{
public:

    void
    insert_data(Entity _e, T _component)
    {
        assert(m_entity_to_index_map.find(_e) == m_entity_to_index_map.end()
               && "entity does not exist!");
        
        std::size_t newidx = m_size;
        m_entity_to_index_map[_e] = newidx;
        m_index_to_entity_map[newidx] = _e;
        m_component_array[newidx] = _component;
        ++m_size;
    }

    void
    remove_data(Entity _e)
    {
        assert(m_entity_to_index_map.find(_e) == m_entity_to_index_map.end()
               && "entity does not exist!");

        /*Copy element at end into deleted elements place to maintain density*/
        std::size_t removedidx = m_entity_to_index_map[_e];
        std::size_t last_idx = m_size - 1;
        m_component_array[removedidx] = m_component_array[last_idx];

        /*Update maps*/
        Entity last_entity = m_index_to_entity_map[last_idx];
        m_entity_to_index_map[last_idx] = removedidx;
        m_index_to_entity_map[removedidx] = last_entity;
        m_entity_to_index_map.erase(_e);
        m_index_to_entity_map.erase(last_idx);
        --m_size;
    }

    T&
    get_data(Entity _e)
    {
        assert(m_entity_to_index_map.find(_e) == m_entity_to_index_map.end()
               && "entity does not exist!");
        return m_component_array[m_entity_to_index_map[_e]];
    }

    void
    entity_destroyed(Entity _e) override
    {
        if (m_entity_to_index_map.find(_e) != m_entity_to_index_map.end())
            remove_data(_e);
    }

private:
    std::array<T, MAX_COMPONENTS> m_component_array;
    std::unordered_map<Entity, std::size_t> m_entity_to_index_map;
    std::unordered_map<std::size_t, Entity> m_index_to_entity_map;
    std::size_t m_size;
};

class ComponentManager
{
	std::unordered_map<const char*, ComponentType> m_component_types{};
	std::unordered_map<const char*, std::shared_ptr<TComponentArray>> m_component_arrays{};
	ComponentType m_next_component_type{};
    template<typename T>
	std::shared_ptr<ComponentArray<T>> get_component_array();

public:
    template<typename T>
    void register_component(void);
    template<typename T>
    ComponentType get_component_type(void);
    template<typename T>
    void add_component(Entity _e, T _component);
    template<typename T>
    void remove_component(Entity _e);
    template<typename T>
    T& get_component(Entity _e);
    void entity_destroyed(Entity _e);
};

class System
{
public:
	std::set<Entity> m_entities;
    virtual void update(float _deltatime) = 0;
}; 

class SystemManager {
    std::unordered_map<const char*, Signature> m_signatures{};
    std::unordered_map<const char*, std::shared_ptr<System>> m_systems{};
public:
    template<typename T>
    std::shared_ptr<T> register_system(void);
    template<typename T>
    void set_signature(Signature _s);
    void entity_destroyed(Entity _e);
    void entity_signature_changed(Entity _e, Signature _s);
};

class World {
    std::unique_ptr<ComponentManager> m_component_manager;
    std::unique_ptr<EntityManager> m_entity_manager;
    std::unique_ptr<SystemManager> m_system_manager;

public:
    void init(void);
    Entity create_entity(void);
    void destroy_entity(Entity _e);

    template<typename T>
    void register_component(void);
    template<typename T>
    ComponentType get_component_type(void);
    template<typename T>
    void add_component(Entity _e, T _component);
    template<typename T>
    void remove_component(Entity _e);
    template<typename T>
    T& get_component(Entity _e);

    template<typename T>
    std::shared_ptr<T> register_system(void);
    template<typename T>
    void set_system_signature(Signature _s);
};

#define ARC_ECS_IMPLEMENTATION
#ifdef ARC_ECS_IMPLEMENTATION

EntityManager::EntityManager(void)
{
    for (Entity e = 0; e < MAX_ENTITIES; ++e)
        m_available_entities.push(e);
}    

Entity
EntityManager::create_entity(void)
{
    assert(m_living_entity_count < MAX_ENTITIES &&
           "too many entities!");
    Entity id = m_available_entities.front();
    m_available_entities.pop();
    ++m_living_entity_count;
    return id;
}

void
EntityManager::destroy_entity(Entity _e)
{
    assert(_e < MAX_ENTITIES &&
           "entity out of range!");
    m_signatures[_e].reset();
    m_available_entities.push(_e);
    --m_living_entity_count;
}

void
EntityManager::set_signature(Entity _e, Signature _s)
{
    assert(_e < MAX_ENTITIES &&
           "entity out of range!");
    m_signatures[_e] = _s;
}

Signature
EntityManager::get_signature(Entity _e)
{
    assert(_e < MAX_ENTITIES &&
           "entity out of range!");
    return m_signatures[_e];
}

template<typename T>
std::shared_ptr<ComponentArray<T>>
ComponentManager::get_component_array()
{
    // get the statically casted pointer to the ComponentArray of type T.
	const char* typeName = typeid(T).name();
	assert(m_component_types.find(typeName) != m_component_types.end() &&
           "Component not registered before use.");
	return std::static_pointer_cast<ComponentArray<T>>(m_component_arrays[typeName]);
}

template<typename T>
void
ComponentManager::register_component(void)
{
	const char* typeName = typeid(T).name();
    assert(m_component_types.find(typeName) == m_component_types.end() &&
           "Registering component type more than once.");
    m_component_types.insert({typeName, m_next_component_type});
    m_component_arrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
    ++m_next_component_type;
}

template<typename T>
ComponentType
ComponentManager::get_component_type(void)
{
 	const char* typeName = typeid(T).name();
    assert(m_component_types.find(typeName) != m_component_types.end() &&
           "Component not registered before use.");
    return m_next_component_type[typeName];
}

template<typename T>
void
ComponentManager::add_component(Entity _e, T _component)
{
    get_component_array<T>()->insert_data(_e, _component);
}

template<typename T>
void
ComponentManager::remove_component(Entity _e)
{
    get_component_array<T>()->remove_data(_e);
}

template<typename T>
T&
ComponentManager::get_component(Entity _e)
{
    return get_component_array<T>()->get_data(_e);
}

void
ComponentManager::entity_destroyed(Entity _e)
{
    /*remove all components owned by entity*/
	for (auto const& pair : m_component_arrays) {
		auto const& component = pair.second;
		component->entity_destroyed(_e);
	}
}

template<typename T>
std::shared_ptr<T>
SystemManager::register_system(void)
{
    const char* typeName = typeid(T).name();
    assert(m_systems.find(typeName) == m_systems.end() &&
           "Registering system more than once.");
    auto system = std::make_shared<T>();
    m_systems.insert({typeName, system});
    return system;
}

template<typename T>
void
SystemManager::set_signature(Signature _s)
{
 		const char* typeName = typeid(T).name();
		assert(m_systems.find(typeName) != m_systems.end() &&
               "System used before registered.");
		m_signatures.insert({typeName, _s});
}

void
SystemManager::entity_destroyed(Entity _e)
{
	// Erase a destroyed entity from all system lists
	// mEntities is a set so no check needed
	for (auto const& pair : m_systems) {
		auto const& system = pair.second;
		system->m_entities.erase(_e);
	}
}

void
SystemManager::entity_signature_changed(Entity _e, Signature _s)
{
	// Notify each system that an entity's signature changed
	for (auto const& pair : m_systems) {
		auto const& type = pair.first;
		auto const& system = pair.second;
		auto const& system_signature = m_signatures[type];
		if ((_s & system_signature) == system_signature) {
            // Entity signature matches system signature - insert into set
			system->m_entities.insert(_e);
        } else {
            // Entity signature does not match system signature - erase from set  
			system->m_entities.erase(_e);
        } 
	}
}

Entity
World::create_entity()
{
	return m_entity_manager->create_entity();
}

void
World::destroy_entity(Entity _e)
{
	m_entity_manager->destroy_entity(_e);
	m_component_manager->entity_destroyed(_e);
	m_system_manager->entity_destroyed(_e);
}

template<typename T>
void
World::register_component()
{
	m_component_manager->register_component<T>();
}

template<typename T>
void
World::add_component(Entity _e, T _component)
{
	m_component_manager->add_component<T>(_e, _component);

	auto signature = m_entity_manager->get_signature(_e);
	signature.set(m_component_manager->get_component<T>(), true);
	m_entity_manager->set_signature(_e, signature);
	m_system_manager->entity_signature_changed(_e, signature);
}

template<typename T>
void
World::remove_component(Entity _e)
{
	m_component_manager->remove_component<T>(_e);

	auto signature = m_entity_manager->get_signature(_e);
	signature.set(m_component_manager->get_component_type<T>(), false);
	m_entity_manager->set_signature(_e, signature);
	m_system_manager->entity_signature_changed(_e, signature);
}

template<typename T>
T&
World::get_component(Entity _e)
{
	return m_component_manager->get_component<T>(_e);
}

template<typename T>
ComponentType
World::get_component_type()
{
	return m_component_manager->get_component_type<T>();
}

template<typename T>
std::shared_ptr<T>
World::register_system()
{
	return m_system_manager->register_system<T>();
}

template<typename T>
void
World::set_system_signature(Signature _s)
{
	m_system_manager->set_signature<T>(_s);
}

#endif /*ARC_ECS_IMPLEMENTATION*/
#endif /*ARC_ECS_H*/

}; /*namespace ecs*/ }; /*namespace arc*/
