#include <utils.hh>
#include <gtest/gtest.h>

namespace results {
namespace internal {
namespace  {

TEST(panic, throws_panicked) {
    EXPECT_THROW(panic("booh"), panicked);
}

}
}
}
