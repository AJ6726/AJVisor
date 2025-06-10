#pragma once
#pragma once
#include "types.h"

extern "C"
{
	void _sgdt(void*);
	void _lgdt(void* gdtr);

	uint16_t __readcs();
	uint16_t __readss();
	uint16_t __readds();
	uint16_t __reades();
	uint16_t __readfs();
	uint16_t __readgs();
	uint16_t __readtr();
	uint16_t __readldtr();

	void __writeds(uint16_t selector);
	void __writees(uint16_t selector);
	void __writefs(uint16_t selector);
	void __writegs(uint16_t selector);
	void __writetr(uint16_t selector);
	void __writeldtr(uint16_t selector);
}

#pragma pack(push, 1)

union segment_descriptor
{
	uint64_t value;

	struct
	{
		uint64_t limit_low : 16;
		uint64_t base_low : 16;
		uint64_t base_middle : 8;
		uint64_t accessed : 1;
		uint64_t write : 1;
		uint64_t direction_conforming : 1;
		uint64_t executable : 1;
		uint64_t system_bit : 1;
		uint64_t dpl : 2;
		uint64_t present : 1;
		uint64_t limit_high : 4;
		uint64_t avl : 1;
		uint64_t longmode : 1;
		uint64_t size_flag : 1;
		uint64_t granularity : 1;
		uint64_t base_high : 8;
	};

	struct
	{
		uint64_t limit_low : 16;
		uint64_t base_low : 16;
		uint64_t base_middle : 8;
		uint64_t type : 4;
		uint64_t system_bit : 1;
		uint64_t dpl : 2;
		uint64_t present : 1;
		uint64_t limit_high : 4;
		uint64_t avl : 1;
		uint64_t longmode : 1;
		uint64_t size_flag : 1;
		uint64_t granularity : 1;
		uint64_t base_high : 8;
	} system;
};

struct descriptor_table_register
{
	uint16_t limit;
	uint64_t base;
};

union segment_selector
{
	uint16_t value;
	struct
	{
		uint16_t rpl : 2;
		uint16_t table : 1;
		uint16_t index : 13;
	};
};

union segment_attribute
{
	uint16_t value;

	struct
	{
		uint16_t accessed : 1;
		uint16_t write : 1;
		uint16_t direction_conforming : 1;
		uint16_t executable : 1;
		uint16_t system_bit : 1;
		uint16_t dpl : 2;
		uint16_t present : 1;
		uint16_t avl : 1;
		uint16_t longmode : 1;
		uint16_t size_flag : 1;
		uint16_t granularity : 1;
		uint16_t reserved0 : 4;
	};

	struct
	{
		uint16_t type : 4;
		uint16_t system_bit : 1;
		uint16_t dpl : 2;
		uint16_t present : 1;
		uint16_t avl : 1;
		uint16_t longmode : 1;
		uint16_t size_flag : 1;
		uint16_t granularity : 1;
		uint16_t reserved0 : 4;
	} system;
};

struct segment
{
	segment_selector selector;
	segment_attribute attribute;

	uint32_t limit;
	uint64_t base;

	void get_attributes(descriptor_table_register& descriptor_table)
	{
		segment_descriptor descriptor = *reinterpret_cast<segment_descriptor*>(descriptor_table.base + selector.index * 8);
		//If clear (0) the descriptor defines a system segment (eg. a task state segment). If set (1) it defines a code or data segment.
		if (descriptor.system_bit)
		{
			attribute.accessed = descriptor.accessed;
			attribute.write = descriptor.write;
			attribute.direction_conforming = descriptor.direction_conforming;
			attribute.executable = descriptor.executable;
			attribute.system_bit = descriptor.system_bit;
			attribute.dpl = descriptor.dpl;
			attribute.present = descriptor.present;
			attribute.avl = descriptor.avl;
			attribute.longmode = descriptor.longmode;
			attribute.size_flag = descriptor.size_flag;
			attribute.granularity = descriptor.granularity;
			attribute.present = descriptor.present;
		}

		if (!descriptor.system_bit)
		{
			attribute.system.type = descriptor.system.type;
			attribute.system.system_bit = descriptor.system.system_bit;
			attribute.system.dpl = descriptor.system.dpl;
			attribute.system.present = descriptor.system.present;
			attribute.system.avl = descriptor.system.avl;
			attribute.system.longmode = descriptor.system.longmode;
			attribute.system.size_flag = descriptor.system.size_flag;
			attribute.system.granularity = descriptor.system.granularity;
			attribute.system.present = descriptor.system.present;
		}
	}
};

#pragma pack(pop)
