#include <iostream>
#include "interface.hpp"

using namespace std;

int main() {
    stringstream ss{
        "(((ADH2:0.1[&&NHX:S=human:E=1.1.1.1],ADH1:0.11[&&NHX:S=human:E=1.1.1.1]):0.05[&&NHX:S="
        "Primates:E=1.1.1.1:D=Y:B=100],ADHY:0.1[&&NHX:S=nematode:E=1.1.1.1],ADHX:0.12[&&NHX:S="
        "insect:E=1.1.1.1]):0.1[&&NHX:S=Metazoa:E=1.1.1.1:D=N],(ADH4:0.09[&&NHX:S=yeast:E=1.1.1.1],"
        "ADH3:0.13[&&NHX:S=yeast:E=1.1.1.1],ADH2:0.12[&&NHX:S=yeast:E=1.1.1.1],ADH1:0.11[&&NHX:S="
        "yeast:E=1.1.1.1]):0.1[&&NHX:S=Fungi])[&&NHX:E=1.1.1.1:D=N]; "};

    NHXParser parser;
    parser.parse(ss);
}
