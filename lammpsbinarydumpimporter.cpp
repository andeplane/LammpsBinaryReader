#include "lammpsbinarydumpimporter.h"
#include <QDebug>
LammpsBinaryDumpImporter::LammpsBinaryDumpImporter()
{

}

bool LammpsBinaryDumpImporter::open(QString fileName)
{
    this->fileName = fileName;
    filePtr = fopen(fileName.toLocal8Bit().constData(),"rb");
    if (!filePtr) {
        qDebug() << "ERROR: Could not open file " << fileName;
        return false;
    }
    hasNextTimestep = true;
    return true;
}

void LammpsBinaryDumpImporter::loadNextTimestep()
{

    fread(&state.timestep,sizeof(bigint),1,filePtr);
    // detect end-of-file
    if (feof(filePtr)) {
        fclose(filePtr);
        hasNextTimestep = false;
        return;
    }

    bigint natoms;
    int size_one, nchunk;
    int boundary[3][2];
    char boundstr[9];

    fread(&natoms,sizeof(bigint),1,filePtr);
    fread(&state.triclinic,sizeof(int),1,filePtr);

    fread(&state.boundary[0][0],6*sizeof(int),1,filePtr);
    fread(&state.xlo,sizeof(double),1,filePtr);
    fread(&state.xhi,sizeof(double),1,filePtr);
    fread(&state.ylo,sizeof(double),1,filePtr);
    fread(&state.yhi,sizeof(double),1,filePtr);
    fread(&state.zlo,sizeof(double),1,filePtr);
    fread(&state.zhi,sizeof(double),1,filePtr);
    if (state.triclinic) {
        fread(&state.xy,sizeof(double),1,filePtr);
        fread(&state.xz,sizeof(double),1,filePtr);
        fread(&state.yz,sizeof(double),1,filePtr);
    }
    fread(&size_one,sizeof(int),1,filePtr);
    fread(&nchunk,sizeof(int),1,filePtr);
    state.atoms.resize(natoms);
    int atomIndex = 0;
    // loop over processor chunks in file
    for (int i = 0; i < nchunk; i++) {
        int chunkSize;
        fread(&chunkSize,sizeof(int),1,filePtr);
        // extend buffer to fit chunk size

        if (chunkSize > maxbuf) {
            if (buf) delete [] buf;
            buf = new double[chunkSize];
            maxbuf = chunkSize;
        }

        // read chunk and write as size_one values per line

        fread(buf,sizeof(double),chunkSize,filePtr);
        int numAtomsInChunk = chunkSize /= size_one;
        int m = 0;
        for (int chunk = 0; chunk < numAtomsInChunk; chunk++) {
            state.atoms[atomIndex].atomId = buf[m++];
            state.atoms[atomIndex].atomType = buf[m++];
            state.atoms[atomIndex].position[0] = buf[m++];
            state.atoms[atomIndex].position[1] = buf[m++];
            state.atoms[atomIndex].position[2] = buf[m++];
            atomIndex++;
            if(size_one > 5) {
                qDebug() << "Error, this binary file has more columns than we support";
                exit(1);
            }
        }
    }
}
