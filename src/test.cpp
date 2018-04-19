/*Copyright or Copr. Centre National de la Recherche Scientifique (CNRS) (2018)
Contributors:
- Vincent Lanore <vincent.lanore@gmail.com>

This software is a computer program whose purpose is to provide a header-only standalone parser for
NHX (New Hampshire Extended) phylogenetic trees.

This software is governed by the CeCILL-C license under French law and abiding by the rules of
distribution of free software. You can use, modify and/ or redistribute the software under the terms
of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy, modify and redistribute
granted by the license, users are provided only with a limited warranty and the software's author,
the holder of the economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated with loading, using,
modifying and/or developing or reproducing the software by the user in light of its specific status
of free software, that may mean that it is complicated to manipulate, and that also therefore means
that it is reserved for developers and experienced professionals having in-depth computer knowledge.
Users are therefore encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or data to be ensured and,
more generally, to use and operate it in the same conditions as regards security.

The fact that you are presently reading this means that you have had knowledge of the CeCILL-C
license and that you accept its terms.*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <chrono>
#include <fstream>
#include "doctest.h"
#include "nhx-parser.hpp"

using namespace std;

#define TEST_ERROR             \
    stringstream error_ss{""}; \
    try

#define TEST_ERROR_END(message) \
    catch (NHXException e) {    \
        error_ss << e.what();   \
    }                           \
    CHECK(error_ss.str() == message);

TEST_CASE("Error: invalid token") {
    stringstream ss{"aopzioei+++++)&é')\"àqspoira"};

    TEST_ERROR { NHXParser parser(ss); }
    TEST_ERROR_END(
        "Error: unexpected token +++++)&é'...\nError at position "
        "8:\n\taopzioei+++++)&é')\"àq...\n\t        ^\n");
}

TEST_CASE("Small tree.") {
    stringstream ss{
        // data copy-pasted from the NHX description pdf (see README for url)
        "(((ADH2:0.1[&&NHX:S=human:E=1.1.1.1], ADH1:0.11[&&NHX:S=human:E=1.1.1.1]):0.05[&&NHX:S="
        "Primates:E=1.1.1.1:D=Y:B=100],ADHY:0.1[&&NHX:S=nematode:E=1.1.1.1],ADHX:0.12[&&NHX:S="
        "insect:E=1.1.1.1]):0.1[&&NHX:S=Metazoa:E=1.1.1.1:D=N],(ADH4:0.09[&&NHX:S=yeast:E=1.1.1.1],"
        "ADH3:0.13[&&NHX:S=yeast:E=1.1.1.1],ADH2:0.12[&&NHX:S=yeast:E=1.1.1.1],ADH1:0.11[&&NHX:S="
        "yeast:E=1.1.1.1]):0.1[&&NHX:S=Fungi])[&&NHX:E=1.1.1.1:D=N]; "};

    NHXParser parser(ss);
    auto begin = std::chrono::high_resolution_clock::now();
    auto& tree = parser.get_tree();
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

TEST_CASE("Comments.") {
    stringstream ss{
        // data copy-pasted from the NHX description pdf (see README for url)
        "(((ADH2:0.1[&&NHX:[tralala youpi je suis un commentaire]S=human:E=1.1.1.1], "
        "ADH1:0.11[&&NHX:S=human:E=1.1.1.1]):0.05[&&NHX:S="
        "Primates:E=1.1.1.1:D=Y:B=100],ADHY:0.1[&&NHX:S=nematode:E=1.1.1.1],ADHX:0.12[&&NHX:S="
        "insect:E=1.1.1.1]):0.1[&&NHX:S=Metazoa:E=1.1.1.1:D=N],(ADH4:0.09[&&NHX:S=yeast:E=1.1.1.1],"
        "ADH3:0.13[&&NHX:S=yeast:E=1.1.1.1],ADH2:0.12[&&NHX:S=yeast:E=1.1.1.1][et en voilà un "
        "autre &éé\"çà)àç)é'],ADH1:0.11[&&NHX:S="
        "yeast:E=1.1.1.1]):0.1[&&NHX:S=Fungi])[&&NHX:E=1.1.1.1:D=N]; "};

    NHXParser parser(ss);
    auto begin = std::chrono::high_resolution_clock::now();
    auto& tree = parser.get_tree();
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

    NHXParser parser(f);
    auto begin = std::chrono::high_resolution_clock::now();
    auto& tree = parser.get_tree();
    auto end = std::chrono::high_resolution_clock::now();
    CHECK(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() < 100);
    CHECK(tree.nb_nodes() == 111);
    CHECK(tree.tag(87, "name") == "ENSSTOP00000023161");
    CHECK(tree.tag(87, "length") == "0.124829");
    CHECK(tree.parent(100) == 99);
    CHECK((tree.children(100) == std::vector<int>{101, 102}));
    CHECK(tree.root() == 0);
}
