#include "dmi.h"

uint16_t MemorySpeed = 0;
char * MemoryTechnology = NULL;

char * memtypes[] = {
	"",
	"Other",
	"Unkown",
	"DRAM",
	"EDRAM",
	"VRAM",
	"SRAM",
	"RAM",
	"ROM",
	"FLASH",
	"EEPROM",
	"FEPROM",
	"EPROM",
	"CDRAM",
	"3DRAM",
	"SDRAM",
	"SGRAM",
	"RDRAM",
	"DDR",
	"DDR2",
	"DDR2 FB-DIMM",
	"Reserved",
	"Reserved",
	"Reserved",
	"DDR3",
	"FBD2",
	"DDR4",
	"LPDDR",
	"LPDDR2",
	"LPDDR3",
	"LPDDR4",
	"non-volatile",
	"HBM",
	"HBM2",
	"DDR5",
	"LPDDR5",
	"HBM3",
	NULL
};

void skip_table(struct structure_header * head,  FILE * fp){

	/* data section */
	uint8_t c; 
	for (int i = 0; i < head->len - 4; i++){
		c = (uint8_t)fgetc(fp);
		if(ferror(fp)){
			perror("Failed to read DMI Tables:");
			exit(1);
		}
	}

	/* strings section 
	   strings are null terminated, and the section is double-null-terminated.
	   have to scan character by character to determine the size of the table
	*/
	uint8_t last = 0xff;
	while(1){
		c = (uint8_t)fgetc(fp);
		if(ferror(fp)){
			perror("Failed to read DMI Tables:");
			exit(1);
		}

		if( c == 0x00 && last == 0x00)
			break;
		last = c;
	}

}

void skip_strings(FILE * fp)
{
	uint8_t c; 
	uint8_t last = 0xff;
	while(1){
		c = (uint8_t)fgetc(fp);
		if(ferror(fp)){
			perror("Failed to read DMI Tables:");
			exit(1);
		}

		if( c == 0x00 && last == 0x00)
			break;
		last = c;
	}
}
// returns a statically allocated const string, no free() needed.
const char * get_mem_tech(){
	if (MemoryTechnology != NULL)
		return MemoryTechnology;


	struct structure_header head;
	struct memory_device md;
	FILE * fp = fopen("/sys/firmware/dmi/tables/DMI", "r");
	if (!fp){
		// fail silently if no root permissions.
		return NULL;
	}

	while(1) {
		size_t res = fread(&head, sizeof(head), 1, fp);
		if (res == 0 && ferror(fp)){
			perror("Failed to read DMI Tables:");
			exit(1);
		}
		if (feof(fp))
			break;

		// look for memory device type
		if(head.type != 17){
			skip_table(&head, fp);
			continue;
		}

		fread(&md, sizeof(md), 1, fp);
		if(ferror(fp)){
			perror("Failed to read memory table: ");
			exit(1);
		}

		// skip if not real RAM
		if(md.memory_type <3 ||
		   md.memory_type == 5 ||
		   md.memory_type == 6 ||
		   md.memory_type == 8 ||
		   md.memory_type == 9 ||
		   md.memory_type == 10 ||
		   md.memory_type == 11 ||
		   md.memory_type == 12 ||
		   md.memory_type == 21 ||
		   md.memory_type == 22 ||
		   md.memory_type == 23 ||
		   md.memory_type == 31 
		   ){
			skip_strings(fp);
			continue;
		}
	
		// just going to return the first memory module that shows up for now
		// This will inevitably hit weird edge cases, but for now this should
		// be sufficient	
		fclose(fp);
		MemoryTechnology = memtypes[md.memory_type]; 
		return MemoryTechnology;
	}

	fclose(fp);
	return NULL;
}

uint16_t get_mem_speed(){
	if (MemorySpeed != 0 ) 
		return MemorySpeed;

	struct structure_header head;
	struct memory_device md;
	FILE * fp = fopen("/sys/firmware/dmi/tables/DMI", "r");
	if (!fp){
		// fail silently if no root permissions
		return 0;
	}
	while(1) {
		size_t res = fread(&head, sizeof(head), 1, fp);
		if (res == 0 && ferror(fp)){
			perror("Failed to read DMI Tables:");
			exit(1);
		}
		if (feof(fp))
			break;

		if(head.type != 17){
			skip_table(&head, fp);
			continue;
		}

		fread(&md, sizeof(md), 1, fp);
		if(ferror(fp)){
			perror("Failed to read memory table: ");
			exit(1);
		}

		//same as above, return the first module seen
		fclose(fp);
		MemorySpeed = md.configured_memory_speed;
		return MemorySpeed;

	}

	fclose(fp);
	return 0;
}
