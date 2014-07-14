#include "esUtil.h"
#include "read.hpp"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{

    ESMatrix testM, testM2;
    
    // esMatrixLoadIdentity(&testM);
    // cout << "identity" << endl;
    // sanshichuan::printMatrix(testM);

    // esMatrixLoadIdentity(&testM);
    // esTranslate(&testM, 0, 0, -5);
    // cout << "translate z:-5" << endl;
    // sanshichuan::printMatrix(testM);

    // esMatrixLoadIdentity(&testM);
    // esRotate(&testM, 90, 0, 0, 1);
    // cout << "rotate z 90" << endl;
    // sanshichuan::printMatrix(testM);

    // esMatrixLoadIdentity(&testM);
    // esScale(&testM, 2, 3, 4);
    // cout << "scale x-2 y-3 z-4" << endl;
    // sanshichuan::printMatrix(testM);

    // esMatrixLoadIdentity(&testM);
    // esScale(&testM, 0, 0, -5);
    // esTranslate(&testM, 0, 0, -5);
    // cout << "I want scale z-5, than translate z-5" <<endl;
    // sanshichuan::printMatrix(testM);


    ESMatrix fru;
    esMatrixLoadIdentity(&fru);
    esFrustum(&fru, -0.6, 0.6, -1, 1, 3, 30);
    sanshichuan::printMatrix(fru);

    esMatrixLoadIdentity(&fru);
    esFrustum(&fru, -1.5, 1.5, -1, 1, 3, 30);
    sanshichuan::printMatrix(fru);

    // esMatrixLoadIdentity(&mvp);
    // esMatrixMultiply(&mvp, &testM, &fru);
    // sanshichuan::printMatrix(mvp);
	
    return 0;

}
