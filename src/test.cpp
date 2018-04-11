#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <chrono>
#include <fstream>
#include "doctest.h"
#include "nhx-parser.hpp"

using namespace std;

TEST_CASE("Small tree.") {
    stringstream ss{
        // data copy-pasted from the NHX description pdf (see README for url)
        "(((ADH2:0.1[&&NHX:S=human:E=1.1.1.1], ADH1:0.11[&&NHX:S=human:E=1.1.1.1]):0.05[&&NHX:S="
        "Primates:E=1.1.1.1:D=Y:B=100],ADHY:0.1[&&NHX:S=nematode:E=1.1.1.1],ADHX:0.12[&&NHX:S="
        "insect:E=1.1.1.1]):0.1[&&NHX:S=Metazoa:E=1.1.1.1:D=N],(ADH4:0.09[&&NHX:S=yeast:E=1.1.1.1],"
        "ADH3:0.13[&&NHX:S=yeast:E=1.1.1.1],ADH2:0.12[&&NHX:S=yeast:E=1.1.1.1],ADH1:0.11[&&NHX:S="
        "yeast:E=1.1.1.1]):0.1[&&NHX:S=Fungi])[&&NHX:E=1.1.1.1:D=N]; "};

    NHXParser parser;
    auto begin = std::chrono::high_resolution_clock::now();
    auto& tree = parser.parse(ss);
    auto end = std::chrono::high_resolution_clock::now();
    CHECK(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() < 25);
    CHECK(tree.nb_nodes() == 12);
    CHECK(tree.tag(3, "name") == "ADH2");
    CHECK(tree.tag(3, "length") == "0.1");
    CHECK(tree.tag(3, "S") == "human");
    CHECK(tree.parent(6) == 1);
    CHECK((tree.children(7) == std::vector<int>{8, 9, 10, 11}));
    CHECK(tree.root() == 0);
}

TEST_CASE("Medium-sized tree (110 nodes).") {
    ifstream f("data/tree1.nhx");

    NHXParser parser;
    auto begin = std::chrono::high_resolution_clock::now();
    auto& tree = parser.parse(f);
    auto end = std::chrono::high_resolution_clock::now();
    CHECK(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() < 100);
    CHECK(tree.nb_nodes() == 111);
    CHECK(tree.tag(87, "name") == "ENSSTOP00000023161");
    CHECK(tree.tag(87, "length") == "0.124829");
    CHECK(tree.parent(100) == 99);
    CHECK((tree.children(100) == std::vector<int>{101, 102}));
    CHECK(tree.root() == 0);
}
