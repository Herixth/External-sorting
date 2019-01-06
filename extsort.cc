
#include "extsort_init.h"

using namespace std;

int main(int argc, char *argv[]) {
    Extsort_Manager extsort(argv);
    extsort.init();
    extsort.create_runfile_RS();
    extsort.merge();


    //system("pause");
    return 0;
}