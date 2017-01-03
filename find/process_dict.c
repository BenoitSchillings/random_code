#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

char *dict_file = "./dict.txt";
char *map_file = "./map.txt";

#define MAX 		32768
#define OVERSIZE_BUFFER 50*1000*1000

#define uint32 uint32_t 
#define uint16 uint16_t

uint32 *offsets;
uint16 *popularity;
char *data;

void main()
{
	FILE	*f;
	char	line[MAX];
	char	pop[MAX];	
	int	idx;
	uint32	cur_offset;

	f = fopen(dict_file, "rt");

	fgets(line, MAX, f);

	uint32 count = atoi(line);
	printf("%s %d\n", line, count);

	offsets = (uint32*)malloc(count * sizeof(uint32));	
	popularity = (uint16*)malloc(count * sizeof(uint16));

	data = (char*)malloc(OVERSIZE_BUFFER);	
	cur_offset = 0;

	for (idx = 0; idx < count; idx++) {
		fgets(line, MAX, f);
		fgets(pop, MAX, f);	
		
		uint16 pop_count = atoi(pop);	
		int len = strlen(line);
	
		offsets[idx] = cur_offset;	
		memcpy(data + cur_offset, line, len);	
		*(data + cur_offset + len - 1) = 0;
		popularity[idx] = pop_count;	
		cur_offset += strlen(line);	
	}
		
	fclose(f);

	FILE *out;

	out = fopen("dict.pack", "wb");

	fwrite(&count, 1, sizeof(count), out);
	fwrite(&cur_offset, 1, sizeof(cur_offset), out);
	fwrite(offsets, count, sizeof(uint32), out);
	fwrite(data, cur_offset, 1, out);	
	fwrite(popularity, count, sizeof(uint16), out);
	fclose(out);
	free((char*)data);
}
