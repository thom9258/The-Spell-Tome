#include <stdio.h>

typedef struct {
    int age;
	char* name;
}Person;

#define t_type Person
#define arr_t_name People
#define t_operator_print(v)						\
	do {printf("name: %s age: %d\n", v.name, v.age);} while (0)
#include "../tsarray.h"

void
test_operators(void)
{
    printf("Atom test --------------\n");
    People people = {0};
    //arr = arr_atom_initn(5);

    People_push(&people, (Person){23, "john"});
    People_push(&people, (Person){58, "jane"});
    People_push(&people, (Person){14, "dohe"});
    People_push(&people, (Person){32, "mary"});

    printf("size=%d, max=%d\n", people.count, people.max);
    Person pop = People_pop(&people);
    printf("popped a person (name: %s)\n", pop.name);
    printf("size=%d, max=%d\n", people.count, people.max);

    printf("printing array using build-in operator:\n");
    People_print(&people);
    People_destroy(&people);
    printf("after destroy: ");
    printf("size=%d, max=%d\n", people.count, people.max);
}


