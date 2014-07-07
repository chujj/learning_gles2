#include "esUtil.h"
#include "read.hpp"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{

    ESMatrix testM, fru, mvp;

    
    esMatrixLoadIdentity(&testM);
    cout << "identity" << endl;
    sanshichuan::printMatrix(testM);

    esMatrixLoadIdentity(&testM);
    esTranslate(&testM, 0, 0, -5);
    cout << "translate z:-5" << endl;
    sanshichuan::printMatrix(testM);

    esMatrixLoadIdentity(&testM);
    esRotate(&testM, 90, 0, 0, 1);
    cout << "rotate z 90" << endl;
    sanshichuan::printMatrix(testM);

    // esMatrixLoadIdentity(&fru);
    // esFrustum(&fru, -1, 1, -1, 1, 3, 30);
    // sanshichuan::printMatrix(fru);

    // esMatrixLoadIdentity(&mvp);
    // esMatrixMultiply(&mvp, &testM, &fru);
    // sanshichuan::printMatrix(mvp);
	
    return 0;

}
