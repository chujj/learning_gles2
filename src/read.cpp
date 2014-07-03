#include "read.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using std::cout;
using std::endl;
using std::cin;

namespace sanshichuan {


long readFileInStr(char * absFilename, char ** data)
{
    FILE * f = fopen(absFilename, "r");
    if (f == NULL) {
	cout << "open file fail of" << absFilename << endl;
	return -1l;
    }

    // get the size
    long size = 0;
    fseek(f, 0l, SEEK_END);
    size = ftell(f);
    // cout << "size of file: " << size <<endl;
    fseek(f, 0l, SEEK_SET); // reset to head

    char * buffer = (char *) malloc(sizeof(char) * size);
    long retval = fread(buffer, sizeof(char), size, f);
    if (retval != size) {
	cout << "read fail ";
	cout << "read " << retval << " total: " << size <<endl;
    } else {
	// cout << "read success" << endl;
    }

    *data = buffer;
    return size;
}

// int main(int argc, char *argv[])
// {
//     if (argc < 2) {
// 	cout<<" use me like: ./read file need to read" << endl;
// 	exit(255);
//     }

//     char * pdata;
//     printf("addr: %x\n" , pdata);
    
//     long readsize = readFileInStr(argv[1], &pdata);
//     printf("addr: %x\n" , pdata);
//     cout << readsize << endl;
//     cout << pdata;
//     cout << std::ends;
    
//     return 0;
// }



}
