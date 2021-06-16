#include <gtest/gtest.h>
#include "server.h"

int	main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
#ifdef NEVER_END
	RUN_ALL_TESTS();
	while (1);
	return (0);
#else
	return RUN_ALL_TESTS();
#endif
}
