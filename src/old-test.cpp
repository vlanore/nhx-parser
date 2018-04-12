#include <chrono>
#include <fstream>
#include <iostream>
#include "nhx-parser.hpp"

using namespace std;

int main() {
    ifstream f("data/tree1.nhx");

    stringstream ss{
        "(((ADH2:0.1[&&NHX:S=human:E=1.1.1.1], ADH1:0.11[&&NHX:S=human:E=1.1.1.1]):0.05[&&NHX:S="
        "Primates:E=1.1.1.1:D=Y:B=100],ADHY:0.1[&&NHX:S=nematode:E=1.1.1.1],ADHX:0.12[&&NHX:S="
        "insect:E=1.1.1.1]):0.1[&&NHX:S=Metazoa:E==++1.1.1.1:D=N],(ADH4:0.09[&&NHX:S=yeast:E=1.1.1."
        "1],"
        "ADH3:0.13[&&NHX:S=yeast:E=1.1.1.1],ADH2:0.12[&&NHX:S=yeast:E=1.1.1.1],ADH1:0.11[&&NHX:S="
        "yeast:E=1.1.1.1]):0.1[&&NHX:S=Fungi])[&&NHX:E=1.1.1.1:D=N];"};

    NHXParser parser;

    auto begin = std::chrono::high_resolution_clock::now();
    auto& tree = parser.parse(ss);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms"
              << std::endl;

    for (size_t i = 0; i < tree.nb_nodes(); i++) {
        cout << "Node " << i << " (" << tree.tag(i, "name") << ":" << tree.tag(i, "length")
             << "), parent: " << tree.parent(i) << ", children: ";
        auto& children = tree.children(i);
        for (auto child : children) {
            cout << child << " ";
        }
        cout << "\n";
    }
}
