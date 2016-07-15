#ifndef LAMMPSBINARYDUMPIMPORTER_H
#define LAMMPSBINARYDUMPIMPORTER_H
#include <QString>
#include <QVector>
#include <QVector3D>

#include "stdio.h"
#include "string.h"

// these must match settings in src/lmptype.h which builds LAMMPS with
//   -DLAMMPS_SMALLBIG (the default), -DLAMMPS_BIGBIG, or -DLAMMPS_SMALLSMALL
// you can edit the tools/Makefile to enforce the same setting
//   for the build of binary2txt, e.g.
//   g++ -g -DLAMMPS_BIGBIG binarytxt.o -o binary2txt
//   again -DLAMMPS_SMALLBIG is the default

#include "stdint.h"
#define __STDC_FORMAT_MACROS
#include "inttypes.h"

#ifndef PRId64
#define PRId64 "ld"
#endif

#if !defined(LAMMPS_SMALLSMALL) && !defined(LAMMPS_BIGBIG) && !defined(LAMMPS_SMALLBIG)
#define LAMMPS_SMALLBIG
#endif

#if defined(LAMMPS_SMALLBIG)
typedef int tagint;
typedef int64_t bigint;
#define BIGINT_FORMAT "%" PRId64
#elif defined(LAMMPS_SMALLSMALL)
typedef int tagint;
typedef int bigint;
#define BIGINT_FORMAT "%d"
#else /* LAMMPS_BIGBIG */
typedef int64_t tagint;
typedef int64_t bigint;
#define BIGINT_FORMAT "%" PRId64
#endif

struct Atom {
    QVector3D position;
    int atomType;
    int atomId;
};

struct State {
    double xlo = 0;
    double xhi = 0;
    double ylo = 0;
    double yhi = 0;
    double zlo = 0;
    double zhi = 0;
    double xy = 0;
    double xz = 0;
    double yz = 0;

    QVector<Atom> atoms;
    int64_t timestep = 0;
    bool triclinic = false;
    int boundary[3][2];
};

class LammpsBinaryDumpImporter
{
public:
    QString fileName;
    FILE *filePtr = nullptr;
    int maxbuf = 0;
    double *buf = NULL;
    State state;
    bool hasNextTimestep = false;

    LammpsBinaryDumpImporter();
    bool open(QString fileName = "");
    void loadNextTimestep();
};

#endif // LAMMPSBINARYDUMPIMPORTER_H
