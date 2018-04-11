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
    std::vector<std::regex> tokens{
        std::regex("\\("),       std::regex("\\)"), std::regex(":"),
        std::regex(";"),         std::regex(","),   std::regex("="),
        std::regex("\\[&&NHX:"), std::regex("\\]"), std::regex("[a-zA-Z0-9._-]+")};
    //                                  std::regex("([0-9]*.)?[0-9]+"),
    using Token = std::pair<int, std::string>;  // first: index of token, second: token value

    // result to be returned
    DoubleListAnnotatedTree tree;

    // state during parsing
    std::string::const_iterator it;
    std::string::const_iterator send;

    Token next_token{-1, ""};
    std::string input{""};
    int next_node{0};

    // lexer
    void find_token() {
        int token_number{0};
        for (auto token : tokens) {
            std::smatch m;
            if (std::regex_search(it, send, m, token) and m.prefix() == "") {
                next_token = Token{token_number, m[0]};
                it += std::string(m[0]).size();
                // std::cout << "found token " << m[0] << std::endl;
                return;
            }
            token_number++;
        }
        next_token = Token{-1, ""};  // no token found in chain
    }

    // parser
    void node_nothing(int number, int parent) {
        tree.nodes_.emplace_back();
        tree.parent_.push_back(parent);
        tree.children_.emplace_back();
        if (parent != -1) {
            tree.children_.at(parent).push_back(number);
        }

        find_token();
        switch (next_token.first) {
            case 8:  // id
                node_name(number, parent);
                break;
            case 2:  //:
                node_length(number, parent);
                break;
            case 6:  // [&&NHX:
                data(number, parent);
                break;
            case 0:  // (
                next_node++;
                node_nothing(next_node, number);
                break;
            default:
                node_end(parent);
        }
    }

    void node_name(int number, int parent) {
        tree.nodes_[number]["name"] = next_token.second;

        find_token();
        switch (next_token.first) {
            case 2:  //:
                node_length(number, parent);
                break;
            case 6:  // [&&NHX:
                data(number, parent);
                break;
            default:
                node_end(parent);
        }
    }

    void node_length(int number, int parent) {
        find_token();
        if (next_token.first == 8) {
            tree.nodes_[number]["length"] = next_token.second;
        } else {
            // TODO error expected an id
        }

        find_token();
        switch (next_token.first) {
            case 6:  // [&&NHX:
                data(number, parent);
                break;
            default:
                node_end(parent);
        }
    }

    void node_end(int parent) {
        switch (next_token.first) {
            case 4:  // ,
                next_node++;
                node_nothing(next_node, parent);
                break;
            case 1: {  // )
                if (parent != -1) {
                    node_name(parent, tree.parent_.at(parent));
                }
                break;
            }
            case 3:  // ;
                break;
            default:
                std::cout << "Error while parsing token " << next_token.second << std::endl;
                exit(1);
        }
    }

    void data(int number, int parent) {
        find_token();
        if (next_token.first == 7) {  // ]
            find_token();
            node_end(parent);
        } else if (next_token.first == 8) {  // id
            std::string tag = next_token.second;
            find_token();  // TODO check it's =
            find_token();  // TODO check it's id
            tree.nodes_[number][tag] = next_token.second;
            data(number, parent);
        } else if (next_token.first == 2) {  // :
            data(number, parent);
        } else {
            // TODO error
        }
    }

  public:
    const AnnotatedTree& parse(std::istream& is) {
        std::string s(std::istreambuf_iterator<char>(is), {});  // FIXME probably not efficient
        input = s;
        it = input.begin();
        send = input.end();
        node_nothing(0, -1);
        return tree;
    }
};
