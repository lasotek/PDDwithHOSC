#include <stdexcept>
#include "NodeMaper.h"

auto NodeMaper::number_to_string(int number)
{
    if (number >= counter)
        throw std::range_error("Wrong number");
    return node_to_name[number];
}

void NodeMaper::reset()
{
    _is_locked = false;
    counter = 0;
    node_to_name.clear();
    name_to_node.clear();
}

int NodeMaper::unique_name_to_number(const std::string &name, bool must_be_unique)
{
    auto it = name_to_node.find(name);
    if (_is_locked)
    {
        if (it == name_to_node.end())
            return -1;
        else
            return it->second;
    }
    else
    {
        if (it != name_to_node.end())
        {
            return must_be_unique ? -1 : it->second;
        }
        name_to_node.emplace(std::make_pair(name, counter));
        node_to_name.push_back(name);
        return counter++;
    }
}

void NodeMaper::remove_nodes(const std::vector<int> &internal_node)
{
    for (auto i : internal_node)
    {
        auto& name = node_to_name[i];
        name_to_node.erase(name);
        name = "";
    }
    
}
