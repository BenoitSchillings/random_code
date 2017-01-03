#include <stdio.h>
#include <stdlib.h>
#include "ip.c"

//-----------------------------------------

int find_ip(ulong ip1, ulong ip2, ulong ip3, ulong ip4)
{
	int	low;
	int	high;
	int	mid;

	ulong	ip = (ip1<<24) | (ip2<<16) | (ip3<<8) | ip4;

	low = 1;
    	high = COUNT/2;
    	do {
        	mid = (low + high) / 2;
        	
		if (ip < map[mid * 2])
           		high = mid - 1;
        	
		if (ip > map[mid * 2])
            		low = mid + 1;
    	} while (ip != map[mid * 2] && low <= high);

	//printf("match is %lx for ip %lx region is %d, %s\n", map[mid * 2], ip, map[mid * 2 + 1], region_to_str(map[mid * 2 + 1]));
	return map[mid * 2 + 1];
}

//------------------------------------------

int main()
{

	for (int i = 0; i < 10000; i++) {	
		find_ip(rand()%255, rand()%255 , rand()%255, 1);
	}
}

//------------------------------------------
