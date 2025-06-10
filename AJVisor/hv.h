#pragma once
#include "page_tables.h"
#include "vcore.h"

bool check_svm_support();
void virtualize();
void unvirtualize(); 

// Callbacks
void virtualize_core(uint32_t core);
void unvirtualize_core(uint32_t core);

void setup_guest(vcore& vcore, CONTEXT& start_context);
void setup_host(vcore& vcore, volatile bool& virtualized);

void setup_resources();
void setup_host_page_table(host_page_table* host_page_table);
void setup_nested_page_table(nested_page_table* nested_page_table);

void setup_msr_pm(vcore& vcore);

