#ifndef DMI_H
#define DMI_H
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>

struct __attribute__((packed)) entry_point {
	uint8_t	anchor[5]; // Anchor string
	uint8_t checksum;
	uint8_t len;
	uint8_t major;
	uint8_t minor;
	uint8_t docrev;
	uint8_t entry_point_revision;
	uint8_t reserved;
	uint16_t structure_table_length;
	uint32_t structure_table_addr;
};

struct __attribute__((packed)) structure_header {
	uint8_t type;
	uint8_t len;
	uint16_t handle;
};


struct __attribute__((packed)) memory_device {
	uint16_t physical_array_handle;
	uint16_t error_info_handle;
	uint16_t total_width;
	uint16_t data_width;
	uint16_t size;
	uint8_t form_factor;
	uint8_t device_set;
	uint8_t device_locator;
	uint8_t bank_locator;
	uint8_t memory_type;
	uint16_t type_detail;
	uint16_t speed;
	uint8_t manufacturer;
	uint8_t serial;
	uint8_t asset_tag;
	uint8_t part_num;
	uint8_t attributes;
	uint32_t extended_size;
	uint16_t configured_memory_speed;
	uint16_t min_voltage;
	uint16_t max_voltage;
	uint16_t configured_voltage;
	uint8_t memory_technology;
	uint16_t operating_mode_capability;
	uint8_t firmware_version;
	uint16_t manufacturer_id;
	uint16_t product_id;
	uint16_t subsystem_controller_mfgid;
	uint16_t subsystem_controller_prodid;
	uint64_t nonvolatile_size;
	uint64_t volatile_size;
	uint64_t cache_size;
	uint64_t logical_size;
	uint32_t extended_speed;
	uint32_t extended_configured_speed;
	uint16_t pmic0_mfgid;
	uint16_t pmic0_revnum;
	uint16_t rcd_mfg_id;
	uint16_t rcd_revnum;
};



void skip_table(struct structure_header *,  FILE * );
void skip_strings(FILE *);
const char * get_mem_tech();
uint16_t get_mem_speed();

extern char * memtypes[];
extern uint16_t MemorySpeed;
extern char * MemoryTechnology;

#endif

