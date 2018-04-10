#include <istream>
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
    using TagName = std::string;
    using TagValue = std::string;
    using ChildrenList = std::vector<NodeIndex>;

    virtual const ChildrenList& children(NodeIndex) const = 0;

    virtual NodeIndex parent(NodeIndex) const = 0;

    virtual NodeIndex root() const = 0;

    virtual std::size_t nb_nodes() const = 0;

    virtual TagValue tag(NodeIndex, TagName) const = 0;
};

class TreeParser {
  public:
    // result guaranteed to be valid until next parse call
    virtual const AnnotatedTree& parse(std::istream&) = 0;
};

class TreeWriter {
  public:
    virtual void write(std::ostream&, const AnnotatedTree&) = 0;
};

/*
====================================================================================================
  ~*~ Implementations ~*~
==================================================================================================*/
class DoubleListAnnotatedTree : public AnnotatedTree {
    using Node = std::unordered_map<std::string, std::string>;

    std::vector<Node> nodes_;

    // invariant: same length as nodes
    // invariant: only one node has parent -1
    // element i is the index of parent of i in nodes (-1 for root)
    std::vector<int> parent_;

    // invariant: consistent with parent
    // element i is a vector of indices corresponding to the children of node i
    std::vector<std::vector<int>> children_;

    // invariant: node with index root is only node with parent -1
    NodeIndex root_;

  public:
    const ChildrenList& children(NodeIndex node) const final { return children_.at(node); }

    NodeIndex parent(NodeIndex node) const final { return parent_.at(node); }

    NodeIndex root() const final { return root_; }

    std::size_t nb_nodes() const final { return nodes_.size(); }

    TagValue tag(NodeIndex node, TagName tag) const final { return nodes_.at(node).at(tag); }
};

/*================================================================================================*/
class NHXParser : public TreeParser {};
