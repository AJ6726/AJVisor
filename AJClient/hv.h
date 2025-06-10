#pragma once
#include <string>
#include "hypercall.h"


inline const uint64_t max_message_length = 256;
inline const uint64_t max_log_count = 256;

struct logger_message
{
	uint64_t tsc;
	char message[max_message_length];
}; 

inline const uint64_t max_logs_size = sizeof(logger_message) * max_log_count;


namespace hv
{
	uint64_t get_eprocess(uint32_t process_id);
	uint64_t get_cr3(uint64_t eprocess);

	//Attempts to hide itself, only works if the PE Headers of the Hypervisor is intact or zeroed out
	void hide(); 

	void read_virtual_memory(uint64_t cr3, uint64_t destination, uint64_t source, uint64_t size);
	void read_physical_memory(uint64_t destination, uint64_t source, uint64_t size);

	void write_virtual_memory(uint64_t cr3, uint64_t destination, uint64_t source, uint64_t size);
	void write_physical_memory(uint64_t destination, uint64_t source, uint64_t size);
	
	void hide_virtual_memory(uint64_t cr3, uint64_t address, uint64_t size); 
	void hide_physical_page(uint64_t address);

	void unhide_virtual_memory(uint64_t cr3, uint64_t address, uint64_t size);
	void unhide_physical_page(uint64_t address);

	uint64_t get_section_base(uint64_t eprocess);

	void flush_logs(); 
	uint64_t get_physical_address(uint64_t cr3, uint64_t address);

	bool is_running(); 
	void unload(); 

	hypercall_status hypercall(hypercall_code code, void* buffer, uint64_t buffer_size);
}