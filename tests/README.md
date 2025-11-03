Tests (Unity)

Build (example):
gcc -Itest_vendor/Unity/src tests/test_main.c tests/test_exceptions.c tests/support/mock_exceptions.c test_vendor/Unity/src/Unity.c -o test_runner

Run:
./test_runner

Notes:
- test_vendor/Unity should contain the Unity checkout (headers and Unity.c).
- tests/support contains host mocks to avoid kernel halts during unit tests.
- Add new tests under tests/ and document them in-file.