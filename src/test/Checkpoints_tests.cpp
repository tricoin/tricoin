//
// Unit tests for block-chain checkpoints
//
#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

#include "../checkpoints.h"
#include "../util.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(Checkpoints_tests)

BOOST_AUTO_TEST_CASE(sanity)
{
    uint256 p1500 = uint256("0x0d23c4067853ef5b5aab1f058e6024cb60ca488b63846a333d8b50742d67b4ef");
    uint256 p80000 = uint256("0x43694915fa334a5adb70130ec90440a71de93a18ed2d2d00e4182b700e67bf9c");
    BOOST_CHECK(Checkpoints::CheckBlock(1500, p1500));
    BOOST_CHECK(Checkpoints::CheckBlock(80000, p80000));

    
    // Wrong hashes at checkpoints should fail:
    BOOST_CHECK(!Checkpoints::CheckBlock(1500, p80000));
    BOOST_CHECK(!Checkpoints::CheckBlock(80000, p1500));

    // ... but any hash not at a checkpoint should succeed:
    BOOST_CHECK(Checkpoints::CheckBlock(1500+1, p80000));
    BOOST_CHECK(Checkpoints::CheckBlock(80000+1, p1500));

    BOOST_CHECK(Checkpoints::GetTotalBlocksEstimate() >= 80000);
}    

BOOST_AUTO_TEST_SUITE_END()
