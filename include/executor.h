#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "ir.h"

// Execute the workflow in dependency order
void execute_workflow(IRWorkflow *ir);

#endif // EXECUTOR_H
