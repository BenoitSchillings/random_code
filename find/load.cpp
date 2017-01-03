#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
char *dict_file = "./dict.pack";
char *map_file = "./map.pack";

#define uint32 uint32_t 
#define uint16 uint16_t


//----------------------------------------------------------


class Links {
	int	link_count;	
	int	data_size;
	
	uint32	*offsets;	
	uint32	*data;

public:;
	Links();
	~Links();
	int	subref_count(int i);
	int	subref(int i, int idx);

};

	Links::Links()
{
	FILE	*f;

	f = fopen(map_file, "r");

	fread(&link_count, 1, sizeof(link_count), f);
	fread(&data_size, 1, sizeof(data_size), f);	
	offsets = (uint32*)malloc(link_count * sizeof(uint32));

	fread(offsets, link_count, sizeof(uint32), f);
	
	data = (uint32*)malloc(data_size * sizeof(uint32));
	fread(data, data_size, sizeof(uint32), f);
	fclose(f);
}

//----------------------------------------------------------

	Links::~Links()
{
}

//----------------------------------------------------------

int	Links::subref_count(int idx)
{
	int	offset;

	offset = offsets[idx];

	uint32 count = *(data + offset);

	return count;
}

int	Links::subref(int i, int idx)
{
	int	offset;

	offset = offsets[i];
	
	return *(data + offset + 1 + idx);
}



//----------------------------------------------------------

class Dict {
private:;
	uint32	dict_count;
	uint32	data_size;
	
	uint32	*offsets;
	char	*data;	
public:;	
	Dict();	
	~Dict();

	uint32	idx(int i);
	char*	string(int i);
	int	count();
	int	find(char *s);
        void    prepString(char *input, char *output);
};


//----------------------------------------------------------

	Dict::Dict()
{
        FILE    *f;

        f = fopen(dict_file, "r");

        fread(&dict_count, 1, sizeof(dict_count), f);
       	fread(&data_size, 1, sizeof(data_size), f);

	offsets = (uint32*)malloc(dict_count * sizeof(uint32)*dict_count);

	fread(offsets, dict_count, sizeof(uint32), f);

	data = (char *)malloc(data_size);

	fread(data, data_size, 1, f);
 
        fclose(f);
}

//----------------------------------------------------------

int	Dict::count()
{
	return dict_count;
}

//----------------------------------------------------------

uint32	Dict::idx(int i)
{
	if (i < 0 || i >= dict_count) {
		printf("bad index %d\n", i);
		exit(-1);	
	} 

	return offsets[i];
}

//----------------------------------------------------------

char*	Dict::string(int i)
{
	uint32	offset = idx(i);

	if (offset >= data_size) {
		printf("bad offset %d\n", offset);
		exit(-1);
	}

	return data + offset;
}

//----------------------------------------------------------

int	Dict::find(char *s)
{
	int	first = 0;
	int	last = count();
	int	middle = (first+last)/2;
 
	while(first <= last) {
     		
		middle = (first + last) / 2;
		char	*current;
		current = this->string(middle);
		
		int	comp = strcmp(current, s);
   
		if (comp < 0) {
			first = middle + 1;
		}
		if (comp == 0) {
			return middle;
		}  
        	if (comp > 0) { 
			last = middle - 1;
		}
   }
   //printf("%s\n", this->string(first)); 
   return -first;
}


void Dict::prepString(char *input, char *output)
{
	char	c;

	while((c = *input++)) {
		if (c == ' ')
			c = '_';
		c = toupper(c);
		*output++ = c;
	}
	*output++ = 0;
}

//----------------------------------------------------------

	Dict::~Dict()
{
}

//----------------------------------------------------------

#define MAX_BAG	32768

class BagSearch {
public:;

	int	count;
	int	capacity;	
	uint32	*bag;
	Dict	*dict;
	Links	*links;

	BagSearch(Dict *a_dict, Links *a_linkmap);
	~BagSearch();

	void	AddWord(char *word, int level);
	void	Summary();
private:;
	
	void	AddEntry(uint32 entry);

	void	sort(int l, int r);
	int 	partition(int l, int r);
};


//----------------------------------------------------------

BagSearch::BagSearch(Dict *a_dict, Links *a_linkmap)
{
	dict = a_dict;
	links = a_linkmap;	
	count = 0;
	bag = (uint32*)malloc(512 * sizeof(uint32));
	capacity = 512;
}

//----------------------------------------------------------

BagSearch::~BagSearch()
{
	free((char *)bag);
}

//----------------------------------------------------------

void BagSearch::AddEntry(uint32 entry)
{
	if (count == capacity) {
		capacity *= 2;
		bag = (uint32*)realloc(bag, capacity * sizeof(uint32));
	}
	bag[count] = entry;
	count++;
}

//----------------------------------------------------------

void BagSearch::AddWord(char *word, int level = 2)
{
	int	found; 	

	found = dict->find(word);
	
	if (found < 0) {
		//found = -found;
		printf("%s %s\n", word, dict->string(-found));	
		return;	
	}

	for (int i = 0; i < links->subref_count(found); i++) {
		int subref = links->subref(found, i);
	
		AddEntry(subref);
		if (level > 0) {
			AddWord(dict->string(subref), level - 1);	
		}	
	}
}

//----------------------------------------------------------

void BagSearch::Summary()
{
	sort(0, count);

	int repeat = 0;
	int cur;
	int i;
 
	int n;

	int disp = 20;

	for (n = 1200; n > 1; n--) {	
		i = 1;
		cur = bag[0];

		while(i < count) {
			if (bag[i] == cur) {
				repeat++;
			}
			else {
				if (repeat == n) {
					printf("%s %d\n", dict->string(cur), repeat);
					disp--;
					if (disp == 0)
						return;	
				}
				cur = bag[i];
				repeat = 0;
			}
	
			i++;
		}
	}	
}

//----------------------------------------------------------

void BagSearch::sort(int l, int r)
{
	int j;

	if (l < r) {
		j = partition(l, r);
		sort(l, j-1);
		sort(j+1, r);
	}
}

//----------------------------------------------------------

int BagSearch::partition(int l, int r)
{
   int pivot, i, j, t;
   pivot = bag[l];
   
   i = l; j = r+1;
		
   while(1) {
   	do
		 ++i;
	while(bag[i] <= pivot && i <= r );
   	
	do	
		 --j;
	while(bag[j] > pivot );
   	
	if( i >= j ) break;
   	
	t = bag[i]; bag[i] = bag[j]; bag[j] = t;
   }
   
   t = bag[l];
   bag[l] = bag[j];
   bag[j] = t;
   return j;
}

//----------------------------------------------------------

#define MAX_SIZE	128


int xmain(int argc, char *argv[])
{
	Dict	d;	
	Links	l;
	char	cleaned_string[MAX_SIZE];

	BagSearch *bag;

	bag = new BagSearch(&d, &l);
	
	for (int i = 1; i < argc; i++) {
		d.prepString(argv[i], cleaned_string);
		bag->AddWord(cleaned_string);
	} 
	

	bag->Summary();
	
	return 0;
}


int main(int argc, char *argv[])
{
	Dict 	d;
	Links 	l;
	char	cleaned_string[MAX_SIZE];


	d.prepString(argv[1], cleaned_string);
	printf("%s %s\n", argv[1], cleaned_string);	
	int found = d.find(cleaned_string);
	if (found < 0) {
		printf("close\n");
		found = -found;
	}
	for (int i = 0; i < l.subref_count(found); i++) {
		int	subref = l.subref(found, i);
		printf("%s\n", d.string(subref));	
	}

	return 0;
}
