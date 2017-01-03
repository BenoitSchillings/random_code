#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

char *map_file = "./map.txt";

#define MAX 		32768

#define uint32 uint32_t 
#define uint16 uint16_t

uint32 *offsets;
uint16 *popularity;
char *data;

char *get_int(char *p, uint32 *result)
{
	int	sum = 0;

	while(*p >='0' && *p <='9') {
		sum *= 10;
		sum += (*p - '0');
		p++;	
	}
	*result = sum;
	return p;
}


void main()
{
	FILE	*f;
	char	line[MAX];
	int	idx;
	uint32	cur_offset;

	f = fopen(map_file, "rt");

	fgets(line, MAX, f);

	uint32 count = atoi(line);
	printf("%s %d\n", line, count);


	cur_offset = 0;

	uint32 *mapper;
	uint32 *offsets;

	//bigger than any need
	mapper = (uint32*)malloc(sizeof(uint32) * 10000000);
	offsets = (uint32*)malloc(sizeof(uint32) * 10000000);
	uint32 offset = 0;

	for (idx = 0; idx < count; idx++) {
		fgets(line, MAX, f);
		uint32 entry_count;
		uint32 value;
		char *current;

		offsets[idx] = offset;
		current = line;
	
		current = get_int(current, &entry_count);	
		current += 2;

		*(mapper + offset) = entry_count;
		offset++;
		for (int i = 0; i < entry_count ; i++) {
			current = get_int(current, &value);	
			current++;
			*(mapper + offset) = value;
			offset++;	
		}		
	}
		
	fclose(f);

        FILE *out;

        out = fopen("map.pack", "wb");

        fwrite(&count, 1, sizeof(count), out);
       	fwrite(&offset, 1, sizeof(offset), out); 
	fwrite(offsets, count, sizeof(uint32), out);
        fwrite(mapper, offset, sizeof(uint32), out);
	fclose(out);	

	free((char *)mapper);
}
