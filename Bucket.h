#ifndef BUCKET_H_INCLUDED
#define BUCKET_H_INCLUDED

#include <sstream>
#include <iostream>
#include <cstring>

const int defaultMaxKeys = 100;

class Bucket: public TextIndex {
    protected:
        Bucket(Directory& dir, int maxKeys = defaultMaxKeys);
        int Insert(char* key, int recAddr);
        int Remove(char* key);
        Bucket* Split(); ///divide el bucket y redistribuye las llaves
        int NewRange(int& newStart, int& newEnd);
        int Redistribute(Bucket& newBucket);///Redistribuir llaves
        int FindBuddy();///Encuentra el bucket contiguo
        int TryCombine();///Intenta combinar buckets
        int Combine(Bucket* buddy, int buddyIndex);///Combina dos buckets
        int Depth;///Numero de bits utilizados en comun por las llaves en el bucket
        Directory& Dir;///Directorio que contiene el bucket
        int BucketAddr;///Direccion del archivo
        ostream& Print(ostream&);
        friend class Directory;
        friend class BucketBuffer;
    };

class BucketBuffer: public TextIndexBuffer {
    public:
        BucketBuffer(int keySize, int maxKeys);
        int Pack(const Bucket& bucket);
        int Unpack(Bucket& bucket);
    };

#endif // BUCKET_H_INCLUDED
