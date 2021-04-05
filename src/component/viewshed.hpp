#pragma once
#include <concepts>
#include <algorithm>
#include <execution>
#include <unordered_set>
#include <vector>
#include "component/position.hpp"
#include "core/map.hpp"
#include "entt/entt.hpp"

#include "boost/container/flat_set.hpp"


// namespace std {
// template <>
// struct hash<radl::position_t> {
//     std::size_t operator()(const radl::position_t& pos) const {
//         return (hash<int>()(pos.first) ^ ((hash<int>()(pos.second) << 1)) >>
//         1);
//     }
// };
// }  // namespace std
extern entt::registry radl::engine::reg;

namespace radl::world {

struct viewshed_t {
    static constexpr int default_range = 8;

    int range = default_range;  // range of visibility
    // the flat_set is the set API with vector as a base instead of a tree
    boost::container::flat_set<position_t> visible_coordinates;
    bool dirty = true;
};

/**
 * @brief
 *
 * @param player_pos
 * @param viewshed
 * @param map
 */
void viewshed_update(position_t& player_pos, viewshed_t& viewshed, Map& map);

/**
 * @brief
 *
 * @param reg
 * @param ent
 */
void fov_update(entt::registry& reg, entt::entity ent);


template <typename T>
concept EntIter = std::random_access_iterator<T>&& std::indirectly_readable<T>;

/**
 * @brief
 *
 * @tparam C Collection of entities with methods begin/end
 * @param e_iterable
 */
template <EntIter C>
void fov_update_parallel(C e_iterable) {
    std::for_each(e_iterable->begin(), e_iterable->end(), [](entity ent) {
        fov_update(engine::reg, ent);
    });
}

}  // namespace radl::world
