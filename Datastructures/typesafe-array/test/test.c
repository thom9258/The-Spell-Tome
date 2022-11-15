#include "test_ptr_type.h"
#include "test_double.h"
#include "test_struct.h"
#include "test_namespace_override.h"

int main(void) {

    test_double();
    test_atom();
    test_ptr_type();
    test_namespace_override();

    return 0;
}

