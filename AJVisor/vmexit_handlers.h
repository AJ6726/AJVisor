#pragma once
#include "vcore.h"

void handle_hypercall(vcore* vcore); 
bool handle_stop_hv(vcore* vcore);
void handle_msr(vcore* vcore);
void inject_exception(vcore* vcore, exception_vector vector, bool push_error, bool stop_nrip);