#include <iostream>
#include "Hash.h"
#include "Directory.h"

using namespace std;

int main() {
    int result;
    Directory Dir(4);
    result = Dir.Create("hashfile");
    if(result == 0) {
        cout<<"Borrar tsthash.dir y tsthash.bkt\n";
        return 0;
        }
    char* keys[] = {"bill", "lee", "pauline", "alan", "julie", "mike", "elizabeth", "mark",
                    "ashley", "peter", "joan", "john", "charles", "mary", "emily"
                   };
    const int numKeys = 15;
    for(int i = 0; i < numKeys; i++) {
            cout<<keys[i]<<" "<<(void*)Hash(keys[i])
                <<" "<<(void*)MakeAddress(keys[i], 16)<<"\n";
            result = Dir.Insert(keys[i], 100 + i);
            if(result == 0)
                cout<<"Insertar "<<keys[i]<<" ha fallado.\n";
            Dir.Print(cout);
        }
    return 1;
    }
