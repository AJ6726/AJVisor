#pragma once
#include <ntstrsafe.h>
#include "types.h"
#include "spinlock.h"

inline const uint64_t max_message_length = 256;
inline const uint64_t max_log_count = 256;

struct logger_message
{
	uint64_t tsc;
	char message[max_message_length];
};

inline const uint64_t max_logs_size = sizeof(logger_message) * max_log_count;


namespace logger
{
	inline spinlock lock; 
	inline logger_message* logs = nullptr; 
	inline int64_t logs_used = 0;
	inline void* flush_buffer = nullptr; 

	inline void* flush()
	{
		scoped_spinlock scoped_lock(lock);

		memcpy(flush_buffer, logs, max_logs_size);
		memset(logs, 0, max_logs_size);
		logs_used = 0; 

		return flush_buffer; 
	}

	inline void write(const char* format, ...)
	{
		if (!logs || logs_used >= max_log_count)
			return; 

		scoped_spinlock scoped_lock(lock); 

		logger_message& log = logs[logs_used]; 

		log.tsc = __rdtsc(); 

		va_list args;
		va_start(args, format);
		_vsnprintf(log.message, max_message_length - 1, format, args);
		va_end(args);

		logs_used++; 
	}
}