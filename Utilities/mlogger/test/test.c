#define LOG_IMPLEMENTATION
#define LOG_SHOW_DATETIME
#include "../mlogger.h"

void
test_display_log_messages(void)
{
	char* log_a = "Logfile_A.txt";
	char* log_b = "Logfile_B.txt";
	LOG_CLEAN(log_a);
	LOG_CLEAN(log_b);
	LOG_INFO(log_a, "Remember to drink lots of water! (%d liters)", 2);

	LOG_WARNING(log_a, "Detected something bad.");
	LOG_WARNING(log_a, "Kernel error count: (%d), ram usage: (%f%%).", 15, 68.4);
	LOG_ERROR(log_a, "Stopping execution.");
	LOG_LOG(log_a, "CUSTOM TYPE", "info about this type.");

	char* complicated_msg = "hello this is a multiline msg \n\n"
	                        "This message has no applied formatting other than"
	                        "standard arguments like a printf()\n"
	                        "this is the end of it \n";
	LOG_PURE(log_b, complicated_msg);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
	test_display_log_messages();
	return 0;
}
