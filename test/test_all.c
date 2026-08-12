#include "unity_fixture.h"

void run_all(void) {
    RUN_TEST_GROUP(llog);
}

int main(int argc, const char **argv) {
    return UnityMain(argc, argv, run_all);
}
