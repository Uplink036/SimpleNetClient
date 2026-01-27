#include <src/server/task.h>
#include <cmath>
#include <string>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TestTaskSuit)

BOOST_AUTO_TEST_CASE(testTaskCorrectIntervalValues) {
    STask* task = getRandomTask();
    BOOST_TEST((task->opID >= 0 && task->opID < 4));
    BOOST_TEST((task->valueOne >= 0 && task->valueOne < 100));
    BOOST_TEST((task->valueTwo >= 0 && task->valueTwo < 100));
    free(task);
}

BOOST_AUTO_TEST_CASE(testTaskCorrectResultValues) {
    STask* task = getRandomTask();
    int opResult;
    double temp;
    switch (task->opID) {
    case op::ADD:
        opResult = task->valueOne + task->valueTwo;
        break;
    case op::SUB:
        opResult = task->valueOne - task->valueTwo;
        break;
    case op::MUL:
        opResult = task->valueOne * task->valueTwo;
        break;
    case op::DIV:
        temp = task->valueOne / task->valueTwo;
        opResult = round(temp);
        break;
    default:
        BOOST_FAIL("Unexpected operand in task");
        break;
    }
    BOOST_TEST(task->result == opResult);
    free(task);
}

BOOST_AUTO_TEST_CASE(testTaskRobustness) {
    for (int i = 0; i < 10; i++) {
        STask* task = getRandomTask();
        int opResult;
        double temp;
        switch (task->opID) {
        case op::ADD:
            opResult = task->valueOne + task->valueTwo;
            break;
        case op::SUB:
            opResult = task->valueOne - task->valueTwo;
            break;
        case op::MUL:
            opResult = task->valueOne * task->valueTwo;
            break;
        case op::DIV:
            temp = task->valueOne / task->valueTwo;
            opResult = round(temp);
            break;
        default:
            BOOST_FAIL("Unexpected operand in task");
            break;
        }
        BOOST_TEST(task->result == opResult);
        free(task);
    }
}


BOOST_AUTO_TEST_CASE(tesTaskToStringADD) {
    STask task;
    task.opID = op::ADD;
    task.valueOne = 1;
    task.valueTwo = 1;
    char* taskString = taskToString(&task);
    BOOST_CHECK_EQUAL(std::string("add 1 1\n"), taskString);
    free(taskString);
}

BOOST_AUTO_TEST_CASE(tesTaskToStringSUB) {
    STask task;
    task.opID = op::SUB;
    task.valueOne = 1;
    task.valueTwo = 1;
    char* taskString = taskToString(&task);
    BOOST_CHECK_EQUAL(std::string("sub 1 1\n"), taskString);
    free(taskString);
}

BOOST_AUTO_TEST_CASE(tesTaskToStringDIV) {
    STask task;
    task.opID = op::DIV;
    task.valueOne = 1;
    task.valueTwo = 1;
    char* taskString = taskToString(&task);
    BOOST_CHECK_EQUAL(std::string("div 1 1\n"), taskString);
    free(taskString);
}

BOOST_AUTO_TEST_CASE(tesTaskToStringMUL) {
    STask task;
    task.opID = op::MUL;
    task.valueOne = 1;
    task.valueTwo = 1;
    char* taskString = taskToString(&task);
    BOOST_CHECK_EQUAL(std::string("mul 1 1\n"), taskString);
    free(taskString);
}


BOOST_AUTO_TEST_SUITE_END()
