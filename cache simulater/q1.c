#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

int no_of_misses = 0;
int no_of_hits = 0;
int check = 1;
int find_set_index(int num, int no_offbits, int set_index_bits)
{
    if (set_index_bits == 0)
        return 0;
    unsigned int x = num << (32 - (no_offbits + set_index_bits));
    x = (x >> (32 - (set_index_bits)));
    return x;
}

unsigned int find_tag(int num, int no_offbits, int set_index_bits)
{
    unsigned int temp = (unsigned int)num;
    unsigned int x = (temp >> (no_offbits + set_index_bits));
    return x;
}

int search_tag_cache(int tag, int set_index, int ways, int cache[][ways])
{
    for (int i = 0; i < ways; i++)
    {
        if (cache[set_index][i] == tag)
        {
            return i;
        }
    }
    return -1;
}

int cache_set_full(int set_index, int ways, int cache[][ways])
{
    int f = 0;

    for (int i = 0; i < ways; i++)
    {
        if (cache[set_index][i] != -1)
        {
            f++;
        }
    }
    if (f == ways)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int main()
{
    srand(time(NULL));
    char filename[100];
    FILE *file;
  printf("enter the configfile : ");
    scanf("%s", filename);
    file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    char input[5][20];
    int i = 0;
    while (fgets(input[i], sizeof(input[i]), file) != NULL && i < 5)
    {
        input[i][strcspn(input[i], "\n")] = '\0';
        i++;
    }

    fclose(file);

    int cache_size = atoi(input[0]);
    int block_size = atoi(input[1]);
    int ways = atoi(input[2]);
    char policy[20];
    char write[20];
    strcpy(policy, input[3]);
    strcpy(write, input[4]);
    int no_of_lines = cache_size / block_size;
    if (ways == 0)
    {
        ways = no_of_lines;
    }
    int no_of_sets = cache_size / (block_size * ways);
    int set_index_bits = log2(no_of_sets);
    int no_offbits = log2(block_size);

    int cache[no_of_sets][ways];
    int flag[no_of_sets][ways];
    int arr[no_of_sets];

    for (int i = 0; i < no_of_sets; i++)
    {
        for (int j = 0; j < ways; j++)
        {
            cache[i][j] = -1;
            flag[i][j] = 0;
        }
    }
    for (int i = 0; i < no_of_sets; i++)
    {
        arr[i] = 0;
    }
    char line[20];
     printf("enter the file name : ");
     scanf("%s", filename);
    file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    while (fgets(line, sizeof(line), file) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';
        int i = 0;
        char *tokens[2];
        char *token = strtok(line, ": ");

        while (token != NULL && i < 2)
        {
            tokens[i++] = token;
            token = strtok(NULL, ": ");
        }
        char address[20];
        char mode[20];
        strcpy(address, tokens[1]);
        strcpy(mode, tokens[0]);
        printf("address : %s ", address);
        unsigned int a = strtoul(address, 0, 16);
        int set_index = find_set_index(a, no_offbits, set_index_bits);
        int tag = find_tag(a, no_offbits, set_index_bits);
        if (strcmp(mode, "R") == 0 || (strcmp(mode, "W") == 0 && strcmp(write, "WB") == 0))
        {
            if (strcmp(policy, "FIFO") == 0)
            {
                if (search_tag_cache(tag, set_index, ways, cache) == -1)
                {
                    if (cache_set_full(set_index, ways, cache) == 0)
                    {
                        for (int j = 0; j < ways; j++)
                        {
                            if (cache[set_index][j] == -1)
                            {
                                cache[set_index][j] = tag;
                                flag[set_index][j]++;
                                printf("set: 0x%x", set_index);
                                printf(" miss");
                                printf(" tag: 0x%x\n", tag);
                                no_of_misses++;
                                break;
                            }
                        }
                    }
                    else
                    {
                        int min = flag[set_index][0];
                        int minindex = 0;
                        for (int l = 1; l < ways; l++)
                        {
                            if (min > flag[set_index][l])
                            {
                                min = flag[set_index][l];
                                minindex = l;
                            }
                        }
                        cache[set_index][minindex] = tag;
                        flag[set_index][minindex]++;
                        printf("set: 0x%x", set_index);
                        printf(" miss");
                        printf(" tag: 0x%x\n", tag);
                        no_of_misses++;
                    }
                }
                else
                {
                    printf("set: 0x%x ", set_index);
                    printf("Hit");
                    printf(" tag: 0x%x\n", tag);
                    no_of_hits++;
                }
            }

            if (strcmp(policy, "RANDOM") == 0)
            {
                sleep(0.5);
                int random = rand() % ways;
                if (search_tag_cache(tag, set_index, ways, cache) == -1)
                {
                    if (cache_set_full(set_index, ways, cache) == 0)
                    {
                        for (int j = 0; j < ways; j++)
                        {
                            if (cache[set_index][j] == -1)
                            {
                                cache[set_index][j] = tag;
                                no_of_misses++;
                                printf("set: 0x%x", set_index);
                                printf(" miss");
                                printf(" tag: 0x%x\n", tag);
                                break;
                            }
                        }
                    }
                    else
                    {
                        cache[set_index][random] = tag;
                        printf("set: 0x%x", set_index);
                        printf(" miss");
                        printf(" tag: 0x%x\n", tag);
                        no_of_misses++;
                    }
                }
                else
                {
                    printf("set: 0x%x ", set_index);
                    printf("Hit");
                    printf(" tag: 0x%x\n", tag);
                    no_of_hits++;
                }
            }

            if (strcmp(policy, "LRU") == 0)
            {
                if (search_tag_cache(tag, set_index, ways, cache) == -1)
                {
                    if (cache_set_full(set_index, ways, cache) == 0)
                    {
                        for (int j = 0; j < ways; j++)
                        {
                            if (cache[set_index][j] == -1)
                            {
                                cache[set_index][j] = tag;
                                flag[set_index][j] = check;
                                check++;
                                printf("set: 0x%x", set_index);
                                printf(" miss");
                                printf(" tag: 0x%x\n", tag);
                                no_of_misses++;
                                break;
                            }
                        }
                    }
                    else
                    {
                        int min = flag[set_index][0];
                        int minindex = 0;
                        for (int l = 1; l < ways; l++)
                        {
                            if (min > flag[set_index][l])
                            {
                                min = flag[set_index][l];
                                minindex = l;
                            }
                        }
                        cache[set_index][minindex] = tag;
                        flag[set_index][minindex] = check;
                        check++;
                        printf("set: 0x%x", set_index);
                        printf(" miss");
                        printf(" tag: 0x%x\n", tag);
                        no_of_misses++;
                    }
                }
                else
                {
                    int wayindex = search_tag_cache(tag, set_index, ways, cache);
                    flag[set_index][wayindex] = check;
                    check++;
                    printf("set: 0x%x ", set_index);
                    printf("Hit");
                    printf(" tag: 0x%x\n", tag);
                    no_of_hits++;
                }
            }
        }
        if (strcmp(mode, "W") == 0 && strcmp(write, "WT") == 0)
        {
            if (strcmp(policy, "FIFO") == 0)
            {
                if (search_tag_cache(tag, set_index, ways, cache) == -1)
                {

                    if (cache_set_full(set_index, ways, cache) == 0)
                    {
                        for (int j = 0; j < ways; j++)
                        {
                            if (cache[set_index][j] == -1)
                            {
                                printf("set: 0x%x", set_index);
                                printf(" miss");
                                printf(" tag: 0x%x\n", tag);
                                no_of_misses++;
                                break;
                            }
                        }
                    }
                    else
                    {
                        printf("set: 0x%x", set_index);
                        printf(" miss");
                        printf(" tag: 0x%x\n", tag);
                        no_of_misses++;
                    }
                }
                else
                {
                    printf("set: 0x%x ", set_index);
                    printf("Hit");
                    printf(" tag: 0x%x\n", tag);
                    no_of_hits++;
                }
            }
            if (strcmp(policy, "RANDOM") == 0)
            {
                sleep(0.5);
                int random = rand() % ways;
                if (search_tag_cache(tag, set_index, ways, cache) == -1)
                {

                    if (cache_set_full(set_index, ways, cache) == 0)
                    {

                        for (int j = 0; j < ways; j++)
                        {
                            if (cache[set_index][j] == -1)
                            {
                                no_of_misses++;
                                printf("set: 0x%x", set_index);
                                printf(" miss");
                                printf(" tag: 0x%x\n", tag);
                                break;
                            }
                        }
                    }
                    else
                    {
                        printf("set: 0x%x", set_index);
                        printf(" miss");
                        printf(" tag: 0x%x\n", tag);
                        no_of_misses++;
                    }
                }
                else
                {
                    printf("set: 0x%x ", set_index);
                    printf("Hit");
                    printf(" tag: 0x%x\n", tag);
                    no_of_hits++;
                }
            }

            if (strcmp(policy, "LRU") == 0)
            {
                if (search_tag_cache(tag, set_index, ways, cache) == -1)
                {
                    if (cache_set_full(set_index, ways, cache) == 0)
                    {
                        for (int j = 0; j < ways; j++)
                        {
                            if (cache[set_index][j] == -1)
                            {
                                printf("set: 0x%x", set_index);
                                printf(" miss");
                                printf(" tag: 0x%x\n", tag);
                                no_of_misses++;
                                break;
                            }
                        }
                    }
                    else
                    {
                        printf("set: 0x%x", set_index);
                        printf(" miss");
                        printf(" tag: 0x%x\n", tag);
                        no_of_misses++;
                    }
                }
                else
                {
                    int wayindex = search_tag_cache(tag, set_index, ways, cache);
                    flag[set_index][wayindex] = check;
                    check++;
                    printf("set: 0x%x ", set_index);
                    printf("Hit");
                    printf(" tag: 0x%x\n", tag);
                    no_of_hits++;
                }
            }
        }
    }
    printf("TOTAL HITS %d ", no_of_hits);
    printf(" TOTAL MISSES %d\n", no_of_misses);
}