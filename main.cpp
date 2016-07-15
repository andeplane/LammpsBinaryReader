#include "lammpsbinarydumpimporter.h"
#include <iostream>
#include <QDebug>
using namespace std;

int main(int argc, char *argv[])
{
    LammpsBinaryDumpImporter importer;
    QString filename = QString("/Users/anderhaf/Desktop/ssz13_3.bin");
    if(!importer.open(filename)) {
        qDebug() << "Could not open file " << filename;
        return 0;
    }
    if(importer.hasNextTimestep) {
        importer.loadNextTimestep();
        qDebug() << "Read timestep with " << importer.state.atoms.size() << " atoms.";
        for(Atom &atom : importer.state.atoms) {
            qDebug() << atom.atomId << " " << atom.atomType << " " << atom.position;
        }
    }
    return 0;
}
