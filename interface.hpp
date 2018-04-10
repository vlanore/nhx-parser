#include <iostream>
#include <regex>
#include <sstream>
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
  public:
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
class NHXParser : public TreeParser {
    // list of token for lexer
    std::vector<std::regex> tokens{std::regex("\\("),
                                   std::regex("\\)"),
                                   std::regex(":"),
                                   std::regex(";"),
                                   std::regex(","),
                                   std::regex("="),
                                   std::regex("\\[&&NHX:"),
                                   std::regex("\\]"),
                                   std::regex("([0-9]*.)?[0-9]+"),
                                   std::regex("[a-zA-Z0-9._-]+")};
    using Token = std::pair<int, std::string>;  // first: index of token, second: token value

    // result to be returned
    DoubleListAnnotatedTree tree;

    // state during parsing
    int next_node{0};
    std::string::const_iterator it;
    Token next_token{-1, ""};

    void find_token(const std::string& s) {
        int token_number{0};
        for (auto token : tokens) {
            std::smatch m;
            if (std::regex_search(it, s.end(), m, token) and m.prefix() == "") {
                next_token = Token{token_number, m[0]};
                it += std::string(m[0]).size();
                return;
            }
            token_number++;
        }
        next_token = Token{-1, ""};  // no token found in chain
    }

  public:
    const AnnotatedTree& parse(std::istream& is) {
        std::string s(std::istreambuf_iterator<char>(is), {});  // FIXME probably not efficient
        it = s.begin();
        do {
            find_token(s);
            std::cout << next_token.second << std::endl;
        } while (next_token.first != -1);
        return tree;
    }
};
