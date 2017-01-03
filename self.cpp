#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>


//----------------------------------------------------------

char *numbers[] = {
                "zero",
                "one",
                "two",
                "three",
                "four",
                "five",
                "six",
                "seven",
                "eight",
                "nine",
                "ten",
                "eleven",
                "twelve",
                "thirteen",
                "fourteen",
                "fifteen",
                "sixteen",
                "seventeen",
                "eighteen",
                "nineteen"};

//----------------------------------------------------------

char *tens[] = {"skip",
                "skip",
                "twenty",
                "thirty",
                "forty",
                "fifty",
                "sixty",
                "seventy",
                "eighty",
                "ninety"};


//----------------------------------------------------------

char *num_to_string(int value, char *dest)
{
    if (value >= 100) {
        dest = strcat(dest, numbers[value/100]);
        dest = strcat(dest, " hundred");
        value = value % 100;
        if (value != 0) {
            dest = strcat(dest, " and ");
        }
        else
            return dest;
    }

    
    if (value < 20) {
        dest = strcat(dest, numbers[value]);
        return dest;
    }

    if (value <= 99) {
        dest = strcat(dest, tens[value/10]);
        value %= 10;
        
        if (value != 0) {
	    dest = strcat(dest, " ");
            dest = strcat(dest, numbers[value%10]);
        }
    }
    return dest;
}


//----------------------------------------------------------


int test_num_to_string()
{
    char buffer[512];

    for (int i = 0; i < 900; i++) {
        buffer[0] = 0;
        num_to_string(i, buffer);
        printf("%d %s\n", i, buffer);
    }
    return 0;
}

//----------------------------------------------------------

int  count_letters[26] = {0};
int  count_letters0[26] = {0};

char cur_buffer[32768];

//----------------------------------------------------------

char count_letters_in_buffer(char *buf)
{
    for (int i = 0; i < 26; i++) count_letters0[i] = 0;
    
    char c;
    //printf("%s\n", buf);
    while (c = *buf++) {
        if (c >= 'a' && c <= 'z') {
            count_letters0[c - 'a']++;
        }
    }
    
    if (memcmp(count_letters0, count_letters, 26 * sizeof(int)) == 0) {
	return 1;
    } 
    
    while(1) {
        int r = rand()%26;
        if (count_letters0[r] != count_letters[r]) {
            count_letters[r] = count_letters0[r];
	    break;
        }
    }
    for (int i =0; i < 26; i++) {
	if (count_letters[i] == 1)
		count_letters[i] = 0;
    }
}

//------------------------------------------------------------

void build_sentence()
{
    char *tmp; 
    char letter[2] = "";
    
    cur_buffer[0] = 0;
    
    tmp = strcat(cur_buffer, "");
    int total = 0; 
    for (int i = 0; i < 26; i++) {
        total += count_letters[i]; 
	if (count_letters[i] > 1) { 
		tmp = num_to_string(count_letters[i], tmp);
        	tmp = strcat(tmp, " ");
        	letter[0] = 'a' + i;
        	letter[1] = 0;
        
        	tmp = strcat(tmp, letter);
		if (count_letters[i] > 0) {
        		tmp = strcat(tmp, "'s ");
		}
		else { 
			tmp = strcat(tmp, " ");
		}
  	} 
    }
}

int main()
{
    int gen = 0;

    srand(time(NULL));
 
    cur_buffer[0] = 0;

    while(1) { 
    	for (int i = 0; i < 26; i++) {
            count_letters[i] = 0 + rand() % 5;
        }
        build_sentence();
        count_letters_in_buffer(cur_buffer);
        memcpy(count_letters, count_letters0, 26 * sizeof(int));
        gen++;
	
        for (int i = 0; i < 1222; i++) {
            build_sentence();
        
            char result = count_letters_in_buffer(cur_buffer);
		
            if (result == 1) {
                printf("%s\n", cur_buffer);
                count_letters_in_buffer(cur_buffer);
                for (int k = 0; k < 26; k++) {
                    //printf("%d ", count_letters0[k]);
                }
                //printf("\n");
                fflush(stdout);
		        srand(time(NULL));
                return 0;
            }
    	}
	if (gen % 10000 == 0) {	
		//printf("%d\n", gen);
		srand(time(NULL));	
	}	
	//printf("%s\n%s\n", cur_buffer, cur_buffer0);
    }
}


