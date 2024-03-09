#include <types.h>
#include <assert.h>
#include <string.h>

void testDateConversions() {
	static const char* testCases[] = {"01-09-2020", "12-12-7800"};
	for (unsigned i = 0; i < sizeof testCases / sizeof(char*); i++) {
		Date date = stringToDate(testCases[i]);
		char* dateStr = dateToString(date);
		assert(strcmp(testCases[i], dateStr) == 0);
		free(dateStr);
	}
}

int main(void) {
    testDateConversions();
}
