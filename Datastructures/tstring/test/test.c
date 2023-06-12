#include "testlib.h"

#define TSTR_IMPLEMENTATION
#include "../tstr.h"

void
test_alloc_free(void)
{
	char text[] = "Hello, my name is Mr. Computer";
	tstr string = {0};
	tstr too_short = {0};

	string = tstr_(text);
	printf("text (%d) = \"%s\"\n", string.maxlen, string.c_str);
	printf("text real length = %ld\n", sizeof(text) / sizeof(text[0]));
	TL_TEST(string.c_str != NULL);
	TL_TEST(string.maxlen == sizeof(text) / sizeof(text[0]));
	printf("length = %ld, memlength = %d\n", sizeof(text) / sizeof(text[0]), string.maxlen);
	TL_TEST(string.c_str[string.maxlen-1] == TSTR_NULLTERM);
	TL_TEST(string.is_view == 0);

	int too_short_size = 4;
	too_short = tstr_n(text, too_short_size);
	TL_TEST(too_short.c_str != NULL);
	TL_TEST(too_short.maxlen == too_short_size+1);
	TL_TEST(too_short.c_str[too_short.maxlen-1] == TSTR_NULLTERM);
	TL_TEST(too_short.is_view == 0);
	printf("too_short (%d) = \"%s\"\n", too_short.maxlen, too_short.c_str);
	tstr_destroy(&too_short);

	tstr_destroy(&string);
	TL_TEST(string.c_str == NULL);
	TL_TEST(string.maxlen == 0);
	TL_TEST(string.is_view == 0);
}

char* global_c_str = "GLOBAL string!";

void
test_tstr_view(void)
{
    tstr global_tstr = tstr_view(global_c_str);
	printf("global = \"%s\"\n", global_tstr.c_str);
	TL_TEST(global_tstr.c_str != NULL);
	TL_TEST(global_tstr.is_view == 1);
	tstr non_global = tstr_(global_tstr.c_str);
	printf("non-global = \"%s\"\n", global_tstr.c_str);
	printf("global length = %d, non-global length = %d\n",
		   global_tstr.maxlen, non_global.maxlen);
	TL_TEST(global_tstr.maxlen + 1 == non_global.maxlen);
	tstr_destroy(&non_global);
}

void
test_compare(void)
{
	char text[] = "Hello, my name is Mr. Computer";

	char is_same = 0;
	tstr string = {0};
	tstr copy = {0};
	string = tstr_(text);

	tstr_copy(&string, &copy);
	TL_TEST(tstr_equal(&string, &copy));
    is_same = tstr_equal(&string, &copy);
	printf("Was same (%d) = [%s] = [%s]\n", is_same, string.c_str, copy.c_str);
	tstr_destroy(&copy);

	copy = tstr_("Something else, lol");
	TL_TEST(!tstr_equal(&string, &copy));
    is_same = tstr_equal(&string, &copy);
	printf("Was same=(%d) = [%s] = [%s]\n", is_same, string.c_str, copy.c_str);
	tstr_destroy(&copy);
	tstr_destroy(&string);

	printf("trying to copy from a const string!\n");
	tstr str_const = tstr_view("Constant string, not to be overwritten!");
	tstr str_new = tstr_("some string i want to overwrite with.");
	tstr_copy(&str_const, &str_new);

    is_same = tstr_equal(&string, &copy);
	printf("Was same=(%d) = [%s] = [%s]\n", is_same, str_const.c_str, str_new.c_str);
	tstr_destroy(&str_const);
	tstr_destroy(&str_new);
}

void
test_copy(void)
{
	char* text = "Hello, my name is Mr. Computer";

	tstr string = {0};
	tstr copy = {0};
	string = tstr_(text);

	tstr_copy(&string, &copy);
	TL_TEST(copy.c_str != NULL);
	TL_TEST(copy.c_str[copy.maxlen-1] == TSTR_NULLTERM);
	printf("full copy text = \"%s\"\n", copy.c_str);

	tstr_copy(&string, &copy);
    tstr_cut( &copy, 0, 5);
	TL_TEST(copy.c_str != NULL);
	//TL_TEST(copy.maxlen == 6);
	TL_TEST(copy.c_str[copy.maxlen-1] == TSTR_NULLTERM);
	printf("short copy text = \"%s\"\n", copy.c_str);

    tstr_destroy(&copy);
	tstr_copy(&string, &copy);
    tstr_cut( &copy, 7, 10);
	TL_TEST(copy.c_str != NULL);
	TL_TEST(copy.c_str[copy.maxlen-1] == TSTR_NULLTERM);
	printf("another short copy text = \"%s\"\n", copy.c_str);

	tstr_destroy(&string);
	tstr_destroy(&copy);
}

void
test_concat(void)
{
	tstr complete = tstr_("Hi! My name is Alex.");
	tstr string = tstr_("Hi! My name is ");
	tstr name = tstr_("Alex.");

	tstr_concat(&string, &name);
	TL_TEST(tstr_ok(&string));
	printf("comparing [%s] = [%s]\n", string.c_str, complete.c_str);
	TL_TEST(tstr_equal(&string, &complete) == 1);

	tstr_destroy(&complete);
	tstr_destroy(&string);
	tstr_destroy(&name);
}

void
test_to_upper_lower(void)
{
	tstr str = tstr_("Leather Armor (Used)");
	tstr upper = {0};
	tstr lower = {0};
    tstr upper_res = tstr_view("LEATHER ARMOR (USED)");
    tstr lower_res = tstr_view("leather armor (used)");
	tstr_copy(&str, &upper);
	tstr_copy(&str, &lower);

	tstr_to_upper(&upper);
	tstr_to_lower(&lower);


	TL_TEST(tstr_equal(&upper, &upper_res));
	printf("upper [%s]\n", upper.c_str);

	TL_TEST(tstr_equal(&lower, &lower_res));
	printf("lower [%s]\n", lower.c_str);

	tstr_destroy(&str);
	tstr_destroy(&upper);
	tstr_destroy(&lower);
}

void
test_find(void)
{
	tstr string = tstr_("hello, Hello, Hello, Hi, my name is Mr. Computer");
	int idx;
    tstr fnd = tstr_view(",");

	TL_TEST(tstr_find(&string, &fnd) == 5);
	TL_TEST(string.c_str[tstr_find(&string, &fnd)] == ',');

	idx = tstr_find(&string, &fnd);
	TL_TEST(idx == 5);
	TL_TEST(string.c_str[tstr_find(&string, &fnd)] == ',');
	printf("found comma (idx %d=%s)\n", idx, string.c_str + idx);

    fnd = tstr_view("name");
	idx = tstr_find(&string, &fnd);
	printf("found name (idx %d=%s)\n", idx, string.c_str + idx);

    fnd = tstr_view("NAME");
	idx = tstr_find(&string, &fnd);
	TL_TEST(idx == TSTR_INVALID);

	if (idx == TSTR_INVALID)
		printf("did not find NAME :(\n");
	else
		printf("FOUND NAME (idx %d=%c)\n", idx, string.c_str[idx]);

    fnd = tstr_view("Computer");
	idx = tstr_find(&string, &fnd);
	TL_TEST(idx == 40);
		printf("found Computer (idx %d=%s)\n", idx, string.c_str + idx);


    fnd = tstr_view("Computerman");
	idx = tstr_find(&string, &fnd);
	TL_TEST(idx == TSTR_INVALID);
	if (idx == TSTR_INVALID)
		printf("Did not find Computerman :(\n");

	tstr_destroy(&string);
}

void
test_find_reverse(void)
{
	int idx = 0;
	tstr string = tstr_("Hello, hello, Hello, Hi, my name is Mr. Computer");
	printf("String: %s\n", string.c_str);

    tstr fnd = tstr_view("e");
	idx = tstr_findlast(&string, &fnd);
	TL_TEST(string.c_str[idx] == 'e');
	printf("found last e (idx %d=%c)\n", idx, string.c_str[idx]);

    fnd = tstr_view("Hello");
	idx = tstr_findlast(&string, &fnd);
	TL_TEST(string.c_str[idx] == 'H' && idx == 14);
	printf("found last Hello (idx %d=%s)\n", idx, string.c_str + idx);
	tstr_destroy(&string);
}

void
test_split(void)
{
	int idx;
	tstr full = tstr_("/home/somedir/somefile.txt");
	tstr filename = {0};
	tstr filepath = {0};


    tstr fnd = tstr_view("/");
	idx = 1 + tstr_findlast(&full, &fnd);

	tstr_split(&full, &filepath, &filename, idx);

    fnd = tstr_view("/home/somedir/");
	TL_TEST(tstr_equal(&filepath, &fnd));

    fnd = tstr_view("somefile.txt");

	TL_TEST(tstr_equal(&filename, &fnd));
	printf("split succesfull:\nsplit [%s]\ninto:\n[%s]\n[%s]\n",
		   full.c_str, filepath.c_str, filename.c_str);

	tstr_destroy(&full);
	tstr_destroy(&filename);
	tstr_destroy(&filepath);
}

void
test_cut(void)
{
	int start;
	int end;
	tstr full = tstr_("/home/somedir/somefile.txt");
	tstr cut = {0};

	start = 5;
    tstr fnd = tstr_view("/");
	end = tstr_findlast(&full, &fnd);

	tstr_copy(&full, &cut);
	tstr_cut(&cut, start, end);
    fnd = tstr_view("/home/somefile.txt");
	TL_TEST(tstr_equal(&cut, &fnd));
	printf("cut [%s] -> [%s]\n", full.c_str, cut.c_str);

	tstr_destroy(&full);
	tstr_destroy(&cut);
}

void
test_tstr_fileread()
{
	tstr filepath = tstr_view("./resources/testfile.txt");
	tstr fdata = tstr_file(&filepath);
	TL_TEST(tstr_ok(&fdata));
	printf("file contents:\n%s\n", fdata.c_str);
	tstr_destroy(&fdata);
}

void
test_tstr_charcast(void)
{
    tstr tchar = tstr_fmt("%c", 't');
    printf("tchar=[%s]\n", tchar.c_str);
    tstr_destroy(&tchar);
}

void
test_tstr_fmt(void)
{
    tstr s;

    s = tstr_fmt("  > %d, my string: %s\n", 2, "THIS STRING");
    tstr res = tstr_view("  > 2, my string: THIS STRING\n");
    printf("formatted string: [%s]\n", s.c_str);
    TL_TEST(tstr_equal(&s, &res));
    tstr_destroy(&s);
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	TL(test_alloc_free());
	TL(test_tstr_view());
	TL(test_compare());
	TL(test_copy());
	TL(test_to_upper_lower());
	TL(test_find());
	TL(test_find_reverse());
	TL(test_concat());
	TL(test_split());
	TL(test_cut());
	TL(test_tstr_fileread());
	TL(test_tstr_charcast());
    TL(test_tstr_fmt());

	tl_summary();
	return 0;
}
