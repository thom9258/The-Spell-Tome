#include "testlib.h"

#define C_MINUSMINUS_IMPLEMENTATION
#include "int_class.h"

void
test_create_classes(void)
{

    C_int_class_initialize();

    class_s* a = C_int_new(4);
    CLASS_PRINT(a);
    CLASS_DESTROY(a);
}

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    TL(test_create_classes())

    tl_summary();
    return 0;
}
