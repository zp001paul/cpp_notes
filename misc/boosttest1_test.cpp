
// g++ -o xxx xxx.cpp -std=c++11 -lboost_unit_test_framework
// -lboost_test_exec_monitor
#define BOOST_TEST_MAIN // 必须定义主测试套件，必须位于头文件之前
#include "boosttest1.hpp"
#include <boost/smart_ptr.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp> // test的头文件

// 测试套件的开始
BOOST_AUTO_TEST_SUITE(s_smart_ptr)

// // 测试用例1
// BOOST_AUTO_TEST_CASE(t_scoped_ptr)      // 测试用例1 t_scoped_ptr
// {
//     boost::scoped_ptr<int> p(new int(874));
//     BOOST_CHECK(p);
//     BOOST_CHECK_EQUAL(*p, 874);
// }
//
// // 测试用例2
// BOOST_AUTO_TEST_CASE(t_shared_ptr)      // 测试用例2 t_shared_ptr
// {
//     boost::shared_ptr<int> p(new int(100));
//
//     BOOST_CHECK(p);
//     BOOST_CHECK_EQUAL(*p, 100);
//     BOOST_CHECK_EQUAL(p.use_count(), 1);
//
//     boost::shared_ptr<int> p2 = p;
//     BOOST_CHECK_EQUAL(p2, p);
//     BOOST_CHECK_EQUAL(p.use_count(), 3);    // 预设一个错误
// }

BOOST_AUTO_TEST_CASE(myfunctest) { BOOST_CHECK_EQUAL(myfunc(), 5); }

BOOST_AUTO_TEST_SUITE_END()
