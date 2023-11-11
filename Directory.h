#ifndef DIRECTORY_H_INCLUDED
#define DIRECTORY_H_INCLUDED

#include <sstream>
#include <iostream>
#include <cstring>

#include "Bucket.h"

class Directory {
    public:
        Directory(int maxBucketKeys = -1);
        ~Directory();
        int Open(char* name);
        int Create(char* name);
        int Close();
        int Insert(char* key, int recAddr);
        int Remove(char* key);
        int Search(char* key);///retorna el registro de una llave
        ostream& Print(ostream& stream);
    protected:
        int Depth;///profundidad del directorio
        int NumCells;///numero de entradas, = 2**Depth
        int* BucketAddr;///areglo de direcciones de buckets

        int DoubleSize();///dobla el tamanio del directorio
        int Collapse();///reduce a la mitad el directorio
        int InsertBucket(int bucketAddr, int first, int last);
        int RemoveBucket(int bucketIndex, int depth);///borrar bucket del directorio
        int Find(char* key);///regresa el BucketAddr para una llave
        int StoreBucket(Bucket* bucket);///actualiza o agrega un bucket al archivo
        int LoadBucket(Bucket* bucket, int bucketAddr);///cargar bucket de un archivo

        int MaxBucketKeys;
        BufferFile* DirectoryFile;
        LengthFieldBuffer* DirectoryBuffer;
        Bucket* CurrentBucket;///objeto para accarrear un bucket
        BucketBuffer* theBucketBuffer;///buffer para buckets
        BufferFile* BucketFile;
        int Pack() const;
        int Unpack();
        Bucket* PrintBucket;///objeto para acarrear un bucket
        friend class Bucket;
    };

#endif // DIRECTORY_H_INCLUDED
