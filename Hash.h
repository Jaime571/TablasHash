#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

int Hash(char* key); ///Crear el hash primario a partir de una cadena

int MakeAddress(char* key, int depth); ///Obtiene el hash primario, invierte los bits
                                       ///regresa la direccion en profundidad de bits

#endif // HASH_H_INCLUDED
