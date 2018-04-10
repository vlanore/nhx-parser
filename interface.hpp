#include <string>
#include <unordered_map>
#include <vector>

/*
====================================================================================================
  ~*~ Pure interfaces ~*~
==================================================================================================*/
class AnnotatedTree {
  public:
    using NodeIndex = int;

    virtual const std::vector<int>& children(NodeIndex node) const = 0;

    virtual int parent(NodeIndex node) const = 0;

    virtual std::size_t nb_nodes() const = 0;

    virtual std::string tag(NodeIndex node, std::string tag) const = 0;
};

/*
====================================================================================================
  ~*~ Implementations ~*~
==================================================================================================*/
class DoubleListAnnotatedTree : public AnnotatedTree {
    using Node = std::unordered_map<std::string, std::string>;

    std::vector<Node> nodes_;

    // (invariant: same length as nodes) element i is the index of
    // parent of i in nodes (-1 for root)
    std::vector<int> parent_;

    // (invariant: consistent with parent) element i is a vector of
    // indices corresponding to the children of node i
    std::vector<std::vector<int>> children_;

  public:
    using NodeIndex = int;

    const std::vector<int>& children(NodeIndex node) const final { return children_.at(node); }

    int parent(NodeIndex node) const final { return parent_.at(node); }

    std::size_t nb_nodes() const final { return nodes_.size(); }

    std::string tag(NodeIndex node, std::string tag) const final { return nodes_.at(node).at(tag); }
};
