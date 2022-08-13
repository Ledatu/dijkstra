#include <vector>
#include <memory>
#include <limits>

template <typename T>
class Node;

template <typename T>
struct Route
{
    std::weak_ptr<Node<T>> node;
    int weight;

    Route(std::weak_ptr<Node<T>> node, int weight) : node(node), weight(weight) {}
};

template <typename T>
class Node : public std::enable_shared_from_this<Node<T>>
{
private:
    std::vector<Route<T>> routes;
    std::weak_ptr<Node> source;
    int weight = std::numeric_limits<int>::max();

    T value;

public:
    Node(T &&);

    template <typename... Args>
    void add_route(Args &&...);

    bool try_set_source(std::weak_ptr<Node<T>>, int);
    void set_weight(int);

    auto &get_routes();
    auto get_source();
    int get_weight();
    T &get_value();

};

template <typename T>
bool operator < (const Node<T>& lhs, const Node<T>& rhs)
{
    return lhs.weight < rhs.weight;
}

template <typename T>
Node<T>::Node(T &&value) : value(value) {}

template <typename T>
template <typename... Args>
void Node<T>::add_route(Args &&...args)
{
    auto route = Route<T>(std::forward<Args>(args)...);
    routes.push_back(route);

    auto emplaced_node = route.node.lock();
    emplaced_node->get_routes().emplace_back(this->weak_from_this(), route.weight);
}

template <typename T>
bool Node<T>::try_set_source(std::weak_ptr<Node<T>> source, int new_weight)
{
    if (new_weight < weight)
        weight = new_weight;
    else
        return false;

    this->source = source;

    return true;
}

template <typename T>
void Node<T>::set_weight(int new_weight)
{
    weight = new_weight;
}

template <typename T>
auto &Node<T>::get_routes()
{
    return routes;
}

template <typename T>
auto Node<T>::get_source()
{
    return source;
}

template <typename T>
int Node<T>::get_weight()
{
    return weight;
}

template <typename T>
T &Node<T>::get_value()
{
    return value;
}