#include "Directory.h"

using namespace std;

const int MaxKeySize = 12;

Directory::Directory(int maxBucketKeys) {
    Depth = 0;///Profundidad del directorio
    NumCells = 1;///numero de entradas, = 2**Depth
    BucketAddr = new int[NumCells];///arreglo de direcciones de bucket

    MaxBucketKeys = maxBucketKeys;
    DirectoryBuffer = new LengthFieldBuffer;///tamanio por defecto
    DirectoryFile = new BufferFile(*DirectoryBuffer);
    CurrentBucket = new Bucket(*this, MaxBucketKeys);
    theBucketBuffer = new BucketBuffer(MaxKeySize, MaxBucketKeys);
    BucketFile = new BufferFile(*theBucketBuffer);
    PrintBucket = new Bucket(*this, MaxBucketKeys);
    }

Directory::~Directory() {
    Close();
    }

void MakeNames(char* name, char*& dirName, char*& bktName) {
    ostrstream directoryName;
    directoryName<<name<<".dir"<<ends;
    dirName = strdup(directoryName.str());
    ostrstream bucketName;
    bucketName<<name<<".bkt"<<ends;
    bktName = strdup(bucketName.str());
    }

int Directory::Open(char* name) {
    int result;
    char* directoryName, *bucketName;
    makeNames(name, directoryName, bucketName);
    result = DirectoryFile->Open(directoryName, ios::in|ios::out);
    if(!result) return 0;
    result = DirectoryFile->Read();
    if(result == -1) return 0;
    result = Unpack();
    if(!result == -1) return 0;
    result = BucketFile->Open(bucketName, ios::in|ios::out);
    return result;
    }

int Directory::Create(char* name) {
    ///Crea los dos archivos, limpia el directorio, crea un solo bucket
    ///y lo agrega al directorio y al archivo del bucket
    int result;
    char* directoryName, *bucketName;
    MakeNames(name, directoryName, bucketName);
    result = DirectoryFile->Create(directoryName, ios::in|ios::out);
    if(!result) return 0;
    result = BucketFile->Create(bucketName, ios::in|ios::out);
    if(!result) return 0;
    ///guardar el CurrentBucket vacio en el archivo de buckets
    ///y agregarlo al directorio
    BucketAddr[0] = StoreBucket(CurrentBucket);///TODO
    return result;
    }

int Directory::Close() {
    ///Escribe en el directorio y cierra. Error cuando hay un
    ///desbordamiento de buffer
    int result;
    result = Pack();
    if(result == -1) return 0;
    DirectoryFile->Rewind();
    result = DirectoryFile->Write();
    if(result == -1) return 0;
    return DirectoryFile->Close() && BucketFile->Close();
    }

int Directory::Insert(char* key, int recAddr) {
    int found = Search(key);
    if(found == -1) return CurrentBucket->Insert(key, recAddr);
    return 0;///La llave ya existe en el directorio
    }

int Directory::Remove(char* key) {
    ///se remueve la llave y devuelve su RecAddr
    int bucketAddr = Find(key);
    LoadBucket(CurrentBucket, bucketAddr);
    return CurrentBucket->Remove(key);
    }

int Directory::Search(char* key) {
    ///regresa RecAddr para una llave, ademas de poner el bucket en una
    ///variable
    int bucketAddr = Find(key);
    LoadBucket(CurrentBucket, bucketAddr);
    return CurrentBucket->Search(key);
    }

ostream& Directory::Print(ostream& stream) {
    stream<<"Prof. del directorio: "<<Depth<<" tamanio: "
    <<NumCells<<"\n";
    for(int i = 0; i < NumCells; i++){
        stream<<"bucket "<<BucketAddr[i]
        <<" addr "<<(void*) i<<"\n";
        LoadBucket(PrintBucket, BucketAddr[i]);
        PrintBucket->Print(stream);
    }
    stream<<" fin del directorio\n";
    return stream;
    }

int Directory::DoubleSize() {
    int newSize = 2 * NumCells;
    int* newBucketAddr = new int[newSize];
    for(int i = 0; i < NumCells; i++) {
        newBucketAddr[2 * i] = BucketAddr[i];
        newBucketAddr[2 * i + 1] = BucketAddr[i];
        }
    delete BucketAddr;
    BucketAddr = newBucketAddr;
    Depth++;
    NumCells = newSize;
    return 1;
    }

int Directory::Collapse() {
    ///en caso de ser posible, se reduce el tamanio por mitad
    if(Depth == 0) return 0;///un solo bucket
    ///se buscan contiguos diferentes, si se encuentran regresa
    for(int i = 0; i < NumCells; i += 2)
        if(BucketAddr[i] != BucketAddr[i + 1]) return 0;
    int newSize = NumCells;
    int* newAddrs = new int[newSize];
    for(int j = 0; j < newSize; j++)
        newAddrs[j] = BucketAddr[j * 2];
    delete BucketAddr;
    BucketAddr = newAddrs;
    Depth--;
    NumCells = newSize;
    return 1;
    }

int Directory::InsertBucket(int bucketAddr, int first, int last) {
    for(int i = first, i <= last; i++)
        BucketAddr[i] = bucketAddr;
    return 1;
    }

int Directory::RemoveBucket(int bucketIndex, int depth) {
    ///asignan todas las celdas de este bucket a su contiguo
    int fillBits = Depth->bucketDepth;///numero de bits a llenar
    int buddyIndex = bucketIndex ^ (1<<(fillBits - 1));
    ///voltear el bit mas chico
    int newBucketAddr = BucketAddr[buddyIndex];
    int lowIndex = bucketIndex >> fillBits << fillBits;
    ///cero bits bajos
    int highIndex = lowIndex + (1<<fillBits) - 1;
    ///asignar bits bajos a 1
    for(int i = lowIndex; i <= highIndex; i++)
        BucketAddr[i] = newBucketAddr;
    return 0;
    }

int Directory::Find(char* key) {
    return BucketAddr[MakeAddress(key, Depth)];
    }

int Directory::StoreBucket(Bucket* bucket) {
    int result;
    result = theBucketBuffer->Pack(*bucket);
    if(result == -1) return -1;
    int addr = bucket->BucketAddr;
    if(addr != 0) return BucketFile->Write(addr);
    addr = BucketFile->Append();
    bucket->BucketAddr = addr;
    return addr;
    }

int Directory::LoadBucket(Bucket* bucket, int bucketAddr) {
    int result;
    result = BucketFile->Read(bucketAddr);
    if(result == -1) return 0;
    result = theBucketBuffer->Unpack(*bucket);
    if(result == -1) return 0;
    bucket->BucketAddr = bucketAddr;
    return 1;
    }

int Directory::Pack() const {
    ///empaqueta el buffer y regresa la cantidad de bytes empaquetados
    int result, packsize;
    DirectoryBuffer->Clear();
    packsize = DirectoryBuffer->Pack(&Depth, sizeof(int));
    if(packsize == -1) return -1;
    for(int i = 0; i < NumCells; i++) {
        result = DirectoryBuffer->Pack(&BucketAddr[i], sizeof(int));
        if(result == -1) return -1;
        packsize += result;
        }
    return packsize;
    }

int Directory::Unpack() {
    int result;
    result = DirectoryBuffer->Unpack(&Depth, sizeof(int));
    if(result == -1) return -1;
    NumCells = 1 << Depth;
    if(BucketAddr != 0) delete BucketAddr;
    BucketAddr = new int[NumCells];
    for(int i = 0; i < NumCells; i++) {
        result = DirectoryBuffer->Unpack(&BucketAddr, sizeof(int));
        if(result == -1)d return -1;
        }
    return 0;
    }
