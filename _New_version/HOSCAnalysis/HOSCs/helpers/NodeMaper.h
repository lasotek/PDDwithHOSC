#ifndef __NODEMAPER_H__
#define __NODEMAPER_H__

#include <vector>
#include <unordered_map>
#include <string>

class NodeMaper
{
private:
    /* data */
    std::vector<std::string> node_to_name;
    std::unordered_map<std::string, int> name_to_node;
    int unique_name_to_number(const std::string &name, bool must_be_unique = false);
    int counter = 0;
    bool _is_locked = false;

public:
    inline auto name_to_number(const std::string &name) { return unique_name_to_number(name); }
    /**
     * @brief register node name. If already exists return -1
     * 
     * @param name name of the node
     * @return auto returns number
     */
    bool register_unique_to_number(const std::string &name) { return unique_name_to_number(name, true) >= 0; }
    auto number_to_string(int number);

    NodeMaper(/* args */) noexcept {}
    NodeMaper(const NodeMaper &Source) = default;
    ~NodeMaper() {}
    inline bool is_locked() const { return _is_locked; }
    inline void lock() { _is_locked = true; }
    inline size_t size() const { return name_to_node.size(); }
    void reset();
    const std::vector<std::string> &Node2Name() const { return node_to_name; }
    void remove_nodes(const std::vector<int> &internal_node);
};

#endif // __NODEMAPER_H__