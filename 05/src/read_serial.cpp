#include <Arduino.h>
#include "utils.h"

#define MAX_INPUT_LINES 5000

char serial_buffer[256];
int iserial_buffer = 0;
int line_num = 0;
uint32_t t_start = 0;
uint32_t char_count = 0;

String lines[MAX_INPUT_LINES];

int line_index[MAX_INPUT_LINES];

bool state_bof = false;
bool state_eof = false;
bool state_eol = false;

class crate_stack
{
public:
    void load(const char * src)
    {
        strcat(crates,src);
        n = strlen(src);
    }

    char top()
    {
        if (n==0) return '\0';
        return crates[n-1];
    }

    char * topn( int num )
    {
        if (num > n) return NULL;
        return(&crates[n-num]);
    }

    void chopn( int num )
    {
        if (num > n) return;
        n -= num;
        crates[n] = '\0';
    }


    void push(char c)
    {
        strncat(crates,&c,1);
        n++;
    }

    void pushn(const char * c, int num )
    {
        strncat(crates,c,num);
        n += num;
    }

    char pop()
    {
        if (n==0) return '\0';
        n--;
        char c = crates[n];
        crates[n] = '\0';
        return c; 
    }

    void dump(int i)
    {
        Serial.printf("%i(%i) : %s\n", i, n, crates );
    }

protected:
    char crates[80] = "";
    int n = 0;
};

#define NSTACKS 9 

/*
[B]                     [N]     [H]
[V]         [P] [T]     [V]     [P]
[W]     [C] [T] [S]     [H]     [N]
[T]     [J] [Z] [M] [N] [F]     [L]
[Q]     [W] [N] [J] [T] [Q] [R] [B]
[N] [B] [Q] [R] [V] [F] [D] [F] [M]
[H] [W] [S] [J] [P] [W] [L] [P] [S]
[D] [D] [T] [F] [G] [B] [B] [H] [Z]
 1   2   3   4   5   6   7   8   9 
*/
crate_stack stacks[NSTACKS];

void init_stacks()
{
    stacks[0].load("DHNQTWVB");
    stacks[1].load("DWB");
    stacks[2].load("TSQWJC");
    stacks[3].load("FJRNZTP");
    stacks[4].load("GPVJMST");
    stacks[5].load("BWFTN");
    stacks[6].load("BLDQFHVN");
    stacks[7].load("HPFR");
    stacks[8].load("ZSMBLNPH");
}


void sort_lines()
{
    line_index[0] = 0;
    
    // simple insertion sort
    for(int i=1; i < line_num; i++ )
    {
        line_index[i] = i;

        for (int j=i;j>0;j--)
        {
            if (lines[line_index[j]] >= lines[line_index[j-1]]) break;
            // swap lines
            int save = line_index[j-1];
            line_index[j-1] = line_index[j];
            line_index[j] = save;
        }
    }
}

void dump_lines()
{
    for(int i=0;i<line_num;i++)
    {
        Serial.println(lines[i]);
    }
}

void dump_crates()
{
    for(int i=0;i<NSTACKS;i++)
    {
        stacks[i].dump(i+1);
    }
}

void start_of_file()
{
    uint32_t heap = ESP.getFreeHeap();
    Serial.printf("START: HEAP %lu\n",heap);

    // reset
    iserial_buffer = 0;
    line_num = 0;
    serial_buffer[0] = '\0';
    char_count = 0;
    t_start = millis();

    init_stacks();
}

void parse_line(String s)
{
    int imove = s.indexOf("move ");
    int ifrom = s.indexOf("from ");
    int ito = s.indexOf("to ");
    
    if (imove >= 0 && ifrom > 0 && ito > 0)
    {
        int ncount = s.substring(imove+5,ifrom).toInt();
        int nfrom = s.substring(ifrom+5,ito).toInt();
        int nto = s.substring(ito+3).toInt();

        Serial.printf("%i x %i -> %i\n", ncount, nfrom, nto );

        // Part 1
        /*
        for(int i=0;i<ncount;i++)
        {
            char c = stacks[nfrom-1].pop();
            stacks[nto-1].push(c);
        }
        */
       // Part 2
        char * c = stacks[nfrom-1].topn(ncount);
        if (c)
        {
            stacks[nto-1].pushn(c,ncount);
            stacks[nfrom-1].chopn(ncount);
        }
    }
}

void dump_tops()
{
    for(int i=0;i<NSTACKS;i++)
    {
        Serial.printf("STACK %i: TOP: %c\n", i+1, stacks[i].top());
    }
    for(int i=0;i<NSTACKS;i++)
    {
        Serial.print(stacks[i].top());
    }
    Serial.println();
}

void process_moves()
{
    for(int i=0; i<line_num; i++)
    {
        if (lines[i].startsWith("move "))
        {
            parse_line(lines[i]);

            //dump_crates();
            //break;
        }
    }
}

void end_of_file()
{
    uint32_t time_ms = millis() - t_start;
    Serial.printf("PROCESSING: CHARS %lu, TIME %lums\n", char_count, time_ms );

    // dump_lines();
    dump_crates();
    process_moves();
    dump_tops();

    uint32_t heap = ESP.getFreeHeap();
    time_ms = millis() - t_start;
    Serial.printf("END: %u lines read : HEAP %lu : TIME %lums\n", line_num, heap, time_ms );
}

void process_line()
{
    Serial.print(line_num);
    Serial.print(": ");
    //if (line_num)
    //    Serial.println(lines[line_num-1]);
    Serial.println(serial_buffer);
}

void read_serial()
{
    while (Serial.available() > 0)
    {
        int ch = Serial.read();
        char_count++;

        // Serial.write(ch);

        if (ch == 1) // start of file
        {
            // start_of_file();
            state_bof = true;
            return;
        }
        else if (ch == 4) // end of file
        {
            // end_of_file();
            state_eof = true;
            return;
        }
        else if (ch == '\n')
        {
            serial_buffer[iserial_buffer] = '\0';

            // process_line();
            
            lines[line_num] = serial_buffer;
            iserial_buffer = 0;
            line_num++;
            if (line_num >= MAX_INPUT_LINES )
            {
                Serial.println("ERROR: line buffer full.");
                line_num = 0;
            }

            state_eol = true;
            return;
        }
        else if (ch >= ' ')
        {
            if (iserial_buffer >= sizeof(serial_buffer))
            {
                Serial.println("ERROR: buffer full.");
                iserial_buffer = 0;
            }
            serial_buffer[iserial_buffer++] = ch;
        }
    }
}
