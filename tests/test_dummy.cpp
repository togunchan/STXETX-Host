#include <catch2/catch_all.hpp>

TEST_CASE("dummy test always passes", "[dummy]")
{
    SUCCEED("This dummy test always succeeds so that CMake has at least one test source.");
}