#include "cheat.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

CHEAT_TEST(mathematics_still_work,
	cheat_assert(2 + 2 == 4);
	cheat_assert_not(2 + 2 == 5);
)
