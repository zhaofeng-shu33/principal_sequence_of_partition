// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =============================================================================

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <tuple>
#include <lemon/list_graph.h>
#include <lemon/adaptors.h>

#include "set/set_stl.h"

namespace submodular {


    template <typename T>
    void make_dgraph(std::size_t n, const std::vector<std::tuple<std::size_t, std::size_t, T>>& arcs, lemon::ListDigraph& g, lemon::ListDigraph::ArcMap<T>& arc_map) {
        lemon::Tolerance<T> _tolerance;
        int m = arcs.size();
        for (std::size_t i = 0; i < n; ++i) {
            g.addNode();
        }
        for (std::size_t arc_id = 0; arc_id < m; ++arc_id) {
            std::size_t src, dst;
            T cap;
            std::tie(src, dst, cap) = arcs[arc_id];
            if (!_tolerance.positive(cap))
                continue;
            lemon::ListDigraph::Arc a = g.addArc(g.nodeFromId(src), g.nodeFromId(dst));
            arc_map[a] = cap;
        }
    }


    //! get incut value
    template <typename T>
    T get_cut_value(lemon::ListDigraph& g, lemon::ListDigraph::ArcMap<T>& arc_map, const stl::CSet _set) {
        T target_value = 0;
        for (lemon::ListDigraph::ArcIt a(g); a != lemon::INVALID; ++a) {
            int _s = g.id(g.source(a));
            int _t = g.id(g.target(a));
            if (!_set.HasElement(_s) && _set.HasElement(_t))
                target_value += arc_map[a];
        }
        return target_value;
    }

    template <typename T>
    T get_partition_value(lemon::ListDigraph& g, lemon::ListDigraph::ArcMap<T>& arc_map, const stl::Partition _partition) {
        T target_value = 0;
        for (const stl::CSet& _s : _partition)
            target_value += get_cut_value(g, arc_map, _s);
        return target_value;
    }


}

#endif
