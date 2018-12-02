#pragma once

#include "agent.h"

#include <CUnit/Basic.h>

#include <stdbool.h>

// Kind of agent to create for the tests
void set_create(agent_t * (*createfunc)());

// Add agent tests to the test suite.
bool add_agent_tests(CU_pSuite suite);

