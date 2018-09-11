#include "Log.h"

#pragma once

class UnitTest {
    protected:
        uint assertionsCount;

    public:
        UnitTest () : assertionsCount (0) { currentUnitTest = this; }
        virtual ~UnitTest () {}

        static bool test (bool assertion) {
            if (currentUnitTest) {
                ++currentUnitTest->assertionsCount;
            }
           return assertion;
        }

        static UnitTest* currentUnitTest;
};

#define     TEST_ASSERTION(_x) if (not (UnitTest::test (_x))) { char* buffer = new char[128]; sprintf (buffer, "ASSERTION FAILURE (FILE: %s, LINE: %d)", __FILE__, __LINE__); throw RuntimeError (buffer); }
#define     TEST_XYOP(_x, _y, _op) { bool result_xyop = ((_x) _op (_y)); (result_xyop ? (Log::debug () << "    PASS") : (Log::error () << "    FAIL")) << ": ((" #_x ") " #_op " (" #_y ")), (" << (_x) << " " #_op " " << (_y) << ")" << endl; TEST_ASSERTION(result_xyop); }
#define     TEST_EQUALS_ULP(_x, _y) { bool result_xyop = ulpEquals((_x), (_y)); (result_xyop ? (Log::debug () << "    PASS") : (Log::error () << "    FAIL")) << ": ((" #_x ") ~= (" #_y ")), (" << (_x) << " ~= " << (_y) << "), (delta = " << abs ((_x) - (_y)) << ")" << endl; TEST_ASSERTION(result_xyop); }
#define     TEST_IN_RANGE(_x, _min, _max) { bool result_xyop = (_min <= (_x)) and ((_x) <= _max); (result_xyop ? (Log::debug () << "    PASS") : (Log::error () << "    FAIL")) << ": ((" #_min ") <= (" #_x ") <= (" #_max ")), ((" << (_min) << " <= " << (_x) << " <= (" << (_max) << ")" << endl; TEST_ASSERTION(result_xyop); }
#define     TEST_EQUALS_EPS(_x, _y, _e) { bool result_xyop = abs((_x) - (_y)) <= _e; (result_xyop ? (Log::debug () << "    PASS") : (Log::error () << "    FAIL")) << ": ((" #_x ") ~= (" #_y ")), (" << (_x) << " ~= " << (_y) << "), (delta: " << abs ((_x) - (_y)) << " < eps: " << (_e) << ")" << endl; TEST_ASSERTION(result_xyop); }
#define     TEST_EQUALS(_x, _y) TEST_XYOP(_x, _y, ==)
#define     TEST_NOT_EQUALS(_x, _y) TEST_XYOP(_x, _y, !=)
#define     TEST_TRUE(_x) TEST_XYOP(bool (_x), true, ==)
#define     TEST_FALSE(_x) TEST_XYOP(bool (_x), false, ==)
#define     EXPECT_FAIL(_x)                                                                                 \
            {                                                                                               \
                try {                                                                                       \
                    _x;                                                                                     \
                    TEST_FALSE(#_x " succeeded (UNEXPECTED)");                                              \
                } catch (RuntimeError& runtimeError) {                                                      \
                    TEST_TRUE(#_x " failed (EXPECTED)");                                                    \
                    Log::exception (runtimeError);                                                          \
                }                                                                                           \
            }
#define		TEST_CASE(name)										                                            \
			class TEST_CASE_##name : public UnitTest {  				                                    \
				public:											                                            \
					TEST_CASE_##name () {			                                                        \
						try {									                                            \
						    Log::info () << "TESTING: (" __FILE__ << ") " << #name << endl;                 \
						    {                                                                               \
						        Log::Scope scope (Log::WARN);                                               \
							    test ();				                                                    \
							}                                                                               \
							if (assertionsCount > 0) {                                                      \
							    Log::debug () << "PASSED: " #name << endl << endl;	                        \
							} else {                                                                        \
                                Log::error () << "EMPTY: " #name " (made no assertions)" << endl << endl;   \
                                exit (EXIT_FAILURE);                                                        \
							}                                                                               \
                        } catch (RuntimeError& runtimeError) {                                              \
                            Log::exception (runtimeError);                                                  \
                            exit (EXIT_FAILURE);                                                            \
                        } catch (exception& exception) {                                                    \
                            cerr << "[OTHER] " << exception.what () << endl;                                \
                            exit (EXIT_FAILURE);                                                            \
                        } catch (...) {                                                                     \
                            cerr << "[ UNKN] exiting..." << endl;                                           \
                            exit (EXIT_FAILURE);                                                            \
                        }                                                                                   \
					}											                                            \
					void test ();								                                            \
        			static TEST_CASE_##name test##name;			                                            \
			};													                                            \
        	TEST_CASE_##name TEST_CASE_##name::test##name;			                                        \
			inline void TEST_CASE_##name::test ()
