#ifndef UNIT_TEST_UTIL_H_
#define UNIT_TEST_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define FAIL(msg) do {\
		fprintf(stderr,"%s Line %d: %s", __FILE__, __LINE__, msg);\
		fflush(NULL);\
		return false;\
	} while(0)

#define ASSERT_TRUE(expression) do { \
                if(!((expression))) { \
                        FAIL("expression is false :: "); \
                } \
        } while (0)

#define ASSERT_FALSE(expression) do { \
                if((expression)) { \
                        FAIL("expression is true  ::"); \
                } \
		} while (0)

#define RUN_TEST(f) do { \
			if(f()==true){ \
				fprintf(stdout, "%s  PASSS\n",#f);\
				fflush(NULL);\
			}else{ fprintf(stderr, "%s  FAIL\n",#f);\
			} }while (0)

#ifdef __cplusplus
}
#endif

#endif /* UNIT_TEST_UTIL_H_ */
