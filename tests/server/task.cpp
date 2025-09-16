#include <task.h>
#include <boost/test/unit_test.hpp>
#include <cmath>

BOOST_AUTO_TEST_SUITE( TestTaskSuit )

BOOST_AUTO_TEST_CASE( testTaskCorrectIntervalValues)
{
    STask task = getTask();
    BOOST_TEST((task.opID >= 0 && task.opID < 4));
    BOOST_TEST((task.valueOne >= 0 && task.valueOne < 100));
    BOOST_TEST((task.valueTwo >= 0 && task.valueTwo < 100));
}

BOOST_AUTO_TEST_CASE( testTaskCorrectResultValues)
{
    STask task = getTask();
    int opResult;
    double temp;
    switch (task.opID)
    {
    case op::ADD:
        opResult = task.valueOne + task.valueTwo;
        break;
    case op::SUB:
        opResult = task.valueOne - task.valueTwo;
        break;
    case op::MUL:
        opResult = task.valueOne * task.valueTwo;
        break;
    case op::DIV:
        temp = task.valueOne / task.valueTwo;
        opResult = round(temp);
        break;
    default:
        BOOST_FAIL("Unexpected operand in task");
        break;
    }
    BOOST_TEST(task.result == opResult);
}

BOOST_AUTO_TEST_SUITE_END()