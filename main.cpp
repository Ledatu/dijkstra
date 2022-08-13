#include <iostream>
#include <vector>
#include <memory>
#include <limits>
#include <string>
#include <random>
#include <algorithm>
#include <chrono>
#include <thread>

#include "Node.hpp"

template <typename NodeType>
void Dijkstra(NodeType from, NodeType to)
{
    if (from == to)
        return;

    for (auto route : from->get_routes())
    {
        auto route_node = route.node.lock();
        if (!route_node->try_set_source(from, from->get_weight() + route.weight))
            continue;
        Dijkstra(route_node, to);
    }
}

int main()
{
    using char_node = Node<char>;

    auto num_of_routes = 10;
    std::vector<std::shared_ptr<char_node>> nodes(num_of_routes);

    auto name = 'A';
    for (auto &&node : nodes)
    {
        node = std::make_shared<char_node>(name++);
    }

    std::random_device engine;
    std::uniform_int_distribution num_of_routes_distr(1, 3);
    std::uniform_int_distribution route_source_distr(0, num_of_routes - 1);
    std::uniform_int_distribution weight_of_routes_distr(1, 10);
    for (auto &&node : nodes)
    {
        for (auto i = 0; i < num_of_routes_distr(engine); i++)
        {
            auto source_index = route_source_distr(engine);
            if (std::find_if(node->get_routes().begin(), node->get_routes().end(), [&](auto &&route)
                             { 
                                    auto _node = route.node.lock();
                                    return node == nodes[source_index] || _node == nodes[source_index]; }) != node->get_routes().end())
            {
                break;
            }
            node->add_route(nodes[source_index], weight_of_routes_distr(engine));
        }
    }

    for (auto &&node : nodes)
    {
        for (auto &&route : node->get_routes())
        {
            std::cout << node->get_value() << " -" << route.weight << "- ";
            std::cout << route.node.lock()->get_value() << "\t\t";
        }
        std::cout << '\n';
    }

    auto from = nodes[0];
    auto to = nodes[9];
    from->set_weight(0);
    Dijkstra(from, to);
    
    std::cout << "\nTotal weight: " << to->get_weight() << "\n";
    do {
        std::cout << to->get_value() << "-";
    }
    while(to = to->get_source().lock());
    std::cout << "\b \n";
}