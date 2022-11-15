#include "testlib.h"

#define TSTRING_IMPLEMENTATION
#include "../tstring.h"

void
test_alloc_free(void)
{
	char text[] = "Hello, my name is Mr. Computer";
	tstring_s string = {0};
	tstring_s too_short = {0};

	string = tstring(text);
	printf("text (%d) = \"%s\"\n", string.maxlen, string.c_str);
	printf("text real length = %ld\n", sizeof(text) / sizeof(text[0]));
	TL_TEST(string.c_str != NULL);
	TL_TEST(string.maxlen == sizeof(text) / sizeof(text[0]));
	printf("length = %ld, memlength = %d\n", sizeof(text) / sizeof(text[0]), string.maxlen);
	TL_TEST(string.c_str[string.maxlen-1] == TSTRING_NULLTERM);
	TL_TEST(string.is_const == 0);

	int too_short_size = 4;
	too_short = tstring_n(text, too_short_size);
	TL_TEST(too_short.c_str != NULL);
	TL_TEST(too_short.maxlen == too_short_size + 1);
	TL_TEST(too_short.c_str[too_short.maxlen-1] == TSTRING_NULLTERM);
	TL_TEST(too_short.is_const == 0);
	printf("too_short (%d) = \"%s\"\n", too_short.maxlen, too_short.c_str);
	tstring_destroy(&too_short);

	tstring_destroy(&string);
	TL_TEST(string.c_str == NULL);
	TL_TEST(string.maxlen == 0);
	TL_TEST(string.is_const == 0);
}

tstring_s global_tstring = tstring_const("GLOBAL string!");
void
test_tstring_const(void)
{
	printf("global = \"%s\"\n", global_tstring.c_str);
	TL_TEST(global_tstring.c_str != NULL);
	TL_TEST(global_tstring.is_const == 1);
	tstring_s non_global = tstring(global_tstring.c_str);
	printf("non-global = \"%s\"\n", global_tstring.c_str);

	printf("global length = %d, non-global length = %d\n",
		   global_tstring.maxlen, non_global.maxlen);
	TL_TEST(global_tstring.maxlen == non_global.maxlen);
	tstring_destroy(&non_global);
}

void
test_compare(void)
{
	char text[] = "Hello, my name is Mr. Computer";

	char is_same = 0;
	tstring_s string = {0};
	tstring_s copy = {0};
	string = tstring(text);

	tstring_copy(&string, &copy);
	TL_TEST((is_same = tstring_equal(&string, &copy)) == 1);
	printf("Was same (%d) = [%s] = [%s]\n", is_same, string.c_str, copy.c_str);
	tstring_destroy(&copy);

	copy = tstring("Something else, lol");
	TL_TEST((is_same = tstring_equal(&string, &copy)) == 0);
	printf("Was same=(%d) = [%s] = [%s]\n", is_same, string.c_str, copy.c_str);
	tstring_destroy(&copy);
	tstring_destroy(&string);

	printf("trying to copy from a const string!");
	tstring_s str_const = tstring_const("Constant string, not to be overwritten!");
	tstring_s str_new = tstring("some string i want to overwrite with.");
	tstring_copy(&str_const, &str_new);

	TL_TEST((is_same = tstring_equal(&string, &copy)) == 0);
	printf("Was same=(%d) = [%s] = [%s]\n", is_same, str_const.c_str, str_new.c_str);
	tstring_destroy(&str_const);
	tstring_destroy(&str_new);
}

void
test_copy(void)
{
	char* text = "Hello, my name is Mr. Computer";

	tstring_s string = {0};
	tstring_s copy = {0};
	string = tstring(text);

	tstring_copy(&string, &copy);
	TL_TEST(copy.c_str != NULL);
	TL_TEST(copy.c_str[copy.maxlen-1] == TSTRING_NULLTERM);
	printf("full copy text = \"%s\"\n", copy.c_str);

	tstring_copy_fn(&string, &copy, 0, 5);
	TL_TEST(copy.c_str != NULL);
	//TL_TEST(copy.maxlen == 6);
	TL_TEST(copy.c_str[copy.maxlen-1] == TSTRING_NULLTERM);
	printf("short copy text = \"%s\"\n", copy.c_str); tstring_copy_fn(&string, &copy, 7, 10);
	TL_TEST(copy.c_str != NULL);
	TL_TEST(copy.c_str[copy.maxlen-1] == TSTRING_NULLTERM);
	printf("another short copy text = \"%s\"\n", copy.c_str);

	tstring_destroy(&string);
	tstring_destroy(&copy);
}
void
test_combine(void)
{
	tstring_s complete = tstring("Hi! My name is Alex.");
	tstring_s string = tstring("Hi! My name is ");
	tstring_s name = tstring("Alex.");
	tstring_s combined;

	combined = tstring_combine(&string, &name);
	TL_TEST(!tstring_invalid(&combined));
	TL_TEST(tstring_equal(&combined, &complete) == 1);
	printf("comparing [%s] = [%s]\n", combined.c_str, complete.c_str);
	tstring_destroy(&complete);
	tstring_destroy(&string);
	tstring_destroy(&name);
	tstring_destroy(&combined);
}

void
test_add_back(void)
{
	tstring_s complete = tstring("Hi! My name is Alex.");
	tstring_s string = tstring("Hi! My name is ");
	tstring_s name = tstring("Alex.");

	tstring_add_back(&string, &name);
	TL_TEST(!tstring_invalid(&string));
	printf("comparing [%s] = [%s]\n", string.c_str, complete.c_str);
	TL_TEST(tstring_equal(&string, &complete) == 1);

	tstring_destroy(&complete);
	tstring_destroy(&string);
	tstring_destroy(&name);
}

void
test_add_back_va(void)
{
	tstring_s full = {0};
	tstring_s filename = tstring_const("somefile.txt");
	tstring_s filepath = tstring_const("somedir");
	tstring_s cmp = {0};

	tstring_add_back_va(&full, 4,
					    &tstring_const("/home/alex/"),
					    &filepath,
					    &tstring_const("/"),
					    &filename
		);
	cmp = tstring("/home/alex/somedir/somefile.txt");
	TL_TEST(tstring_equal(&full, &cmp) == 1);
	printf("Concatenation of multible strings:\n[%s] = [%s]\n",
		   full.c_str, cmp.c_str);

	tstring_destroy(&cmp);
	tstring_destroy(&full);
}

void
test_to_upper_lower(void)
{
	tstring_s str = tstring("Leather Armor (Used)");
	tstring_s upper = {0};
	tstring_s lower = {0};
	tstring_copy(&str, &upper);
	tstring_copy(&str, &lower);

	tstring_to_upper(&upper);
	tstring_to_lower(&lower);

	TL_TEST(tstring_equal(&upper, &tstring_const("LEATHER ARMOR (USED)")));
	printf("upper [%s]\n", upper.c_str);

	TL_TEST(tstring_equal(&lower, &tstring_const("leather armor (used)")));
	printf("lower [%s]\n", lower.c_str);

	tstring_destroy(&str);
	tstring_destroy(&upper);
	tstring_destroy(&lower);
}

void
test_find(void)
{
	tstring_s string = tstring("hello, Hello, Hello, Hi, my name is Mr. Computer");
	int idx;

	TL_TEST(tstring_find_char(&string, ',') == 5);
	TL_TEST(string.c_str[tstring_find_char(&string, ',')] == ',');

	idx = tstring_find(&string, &tstring_const(","));
	TL_TEST(idx == 5);
	TL_TEST(string.c_str[tstring_find(&string, &tstring_const(","))] == ',');
	printf("found comma (idx %d=%s)\n", idx, string.c_str + idx);

	idx = tstring_find(&string, &tstring_const("name"));
	printf("found name (idx %d=%s)\n", idx, string.c_str + idx);

	idx = tstring_find(&string, &tstring_const("NAME"));
	TL_TEST(idx == TSTRING_INVALID);

	if (idx == TSTRING_INVALID)
		printf("did not find NAME :(\n");
	else
		printf("FOUND NAME (idx %d=%c)\n", idx, string.c_str[idx]);

	idx = tstring_find(&string, &tstring_const("Computer"));
	TL_TEST(idx == 40);
		printf("found Computer (idx %d=%s)\n", idx, string.c_str + idx);


	idx = tstring_find(&string, &tstring_const("Computerman"));
	TL_TEST(idx == TSTRING_INVALID);
	if (idx == TSTRING_INVALID)
		printf("Did not find Computerman :(\n");

	tstring_destroy(&string);
}

void
test_find_reverse(void)
{
	int idx = 0;
	tstring_s string = tstring("Hello, hello, Hello, Hi, my name is Mr. Computer");
	printf("String: %s\n", string.c_str);

	idx = tstring_findlast_char(&string, 'e');
	TL_TEST(string.c_str[idx] == 'e');
	printf("found last e (idx %d=%c)\n", idx, string.c_str[idx]);

	idx = tstring_findlast(&string, &tstring_const("Hello"));
	TL_TEST(string.c_str[idx] == 'H' && idx == 14);
	printf("found last Hello (idx %d=%s)\n", idx, string.c_str + idx);
	tstring_destroy(&string);
}

void
test_split(void)
{
	int idx;
	tstring_s full = tstring("/home/somedir/somefile.txt");
	tstring_s filename = {0};
	tstring_s filepath = {0};

	idx = 1 + tstring_findlast_char(&full, '/');

	tstring_split(&full, &filepath, &filename, idx);
	TL_TEST(tstring_equal(&filepath, &tstring_const("/home/somedir/")));
	TL_TEST(tstring_equal(&filename, &tstring_const("somefile.txt")));
	printf("split succesfull:\nsplit [%s]\ninto:\n[%s]\n[%s]\n",
		   full.c_str, filepath.c_str, filename.c_str);

	tstring_destroy(&full);
	tstring_destroy(&filename);
	tstring_destroy(&filepath);
}

void
test_cut(void)
{
	int start;
	int end;
	tstring_s full = tstring("/home/somedir/somefile.txt");
	tstring_s cut = {0};

	start = 5;
	end = tstring_findlast_char(&full, '/');

	tstring_copy(&full, &cut);
	tstring_cut(&cut, start, end);
	TL_TEST(tstring_equal(&cut, &tstring_const("/home/somefile.txt")));
	printf("cut [%s] -> [%s]\n", full.c_str, cut.c_str);

	tstring_destroy(&full);
	tstring_destroy(&cut);
}

void
test_tstring_fileread()
{
	tstring_s filepath = tstring_const("./resources/testfile.txt");
	tstring_s fdata = tstring_load_FILE(&filepath);
	TL_TEST(!tstring_invalid(&fdata));
	printf("file contents:\n%s\n", fdata.c_str);
	tstring_destroy(&fdata);
}

void
test_tstring_format()
{
}

void
test_tstring_charcast(void)
{
    tstring_s tchar = tstring_char('t');
    printf("tchar=[%s]\n", tchar.c_str);
    tstring_destroy(&tchar);
}


int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	TL(test_alloc_free());
	TL(test_tstring_const());
	TL(test_compare());
	TL(test_copy());
	TL(test_to_upper_lower());
	TL(test_find());
	TL(test_find_reverse());
	TL(test_combine());
	TL(test_add_back());
	TL(test_add_back_va());
	TL(test_split());
	TL(test_cut());
	TL(test_tstring_fileread());
	TL(test_tstring_format());
	TL(test_tstring_charcast());

	tl_summary();
	return 0;
}
