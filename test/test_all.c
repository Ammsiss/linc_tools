#include "unity_fixture.h"

void run_all(void) {
    RUN_TEST_GROUP(llog);
    RUN_TEST_GROUP(args);
    RUN_TEST_GROUP(dstr);
    RUN_TEST_GROUP(darr);
    RUN_TEST_GROUP(termsplit);
    RUN_TEST_GROUP(partty);
    RUN_TEST_GROUP(hda);
    RUN_TEST_GROUP(kval);
    RUN_TEST_GROUP(trie);
}

int main(int argc, const char **argv) {
    return UnityMain(argc, argv, run_all);
}
