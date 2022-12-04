#include <Arduino.h>
#include "utils.h"

#define MAX_INPUT_LINES 5000

char serial_buffer[256];
int iserial_buffer = 0;
int line_num = 0;
uint32_t t_start = 0;

String lines[MAX_INPUT_LINES];

int line_index[MAX_INPUT_LINES];

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
        Serial.println(lines[line_index[i]]);
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
    t_start = millis();
}

class range
{
public:
    int a;
    int b;

    bool overlaps(range r2)
    {
        // return ((r2.a >= this->a && r2.b <= this->b) || (this->a >= r2.a && this->b <= r2.b));
        return ((r2.a >= this->a && r2.a <= this->b) || (this->a >= r2.a && this->a <= r2.b));
    }
};

range parse_range(String s)
{
    int idash = s.indexOf('-');
    if (idash > 0)
    {
        String r1 = s.substring(0,idash);
        String r2 = s.substring(idash+1);

        return range { (int)r1.toInt(), (int)r2.toInt() };
    }
    return range {0,0};
}

int count_overlaps()
{
    int c = 0;
    for(int i=0;i<line_num;i++)
    {
        int idelim = lines[i].indexOf(',');
        if (idelim > 0)
        {
            String p1 = lines[i].substring(0,idelim);
            String p2 = lines[i].substring(idelim+1);

            Serial.print(p1);
            Serial.print(" + ");
            Serial.print(p2);

            auto r1 = parse_range(p1);
            auto r2 = parse_range(p2);

            if (r1.overlaps(r2)) c++;

            Serial.printf(" => %i ~ %i, %i ~ %i, c=%u\n", r1.a, r1.b, r2.a, r2.b, c );

        }
    }
    Serial.printf("OVERLAPS = %u\n",c);
    return c;
}

void end_of_file()
{
    //sort_lines();
    //dump_lines();
    count_overlaps();

    uint32_t heap = ESP.getFreeHeap();
    uint32_t time_ms = millis() - t_start;
    Serial.printf("END: %u lines read : HEAP %lu : TIME %lums\n", line_num, heap, time_ms );
}

void process_line()
{
    Serial.print(line_num+1);
    Serial.print(": ");
    Serial.println(serial_buffer);
}

void read_serial()
{
    while (Serial.available() > 0)
    {
        int ch = Serial.read();

        // Serial.write(ch);

        if (ch == 1) // start of file
        {
            start_of_file();
        }
        else if (ch == 4) // end of file
        {
            end_of_file();
        }
        else if (ch == '\n')
        {
            serial_buffer[iserial_buffer] = '\0';

            process_line();
            
            lines[line_num] = serial_buffer;
            iserial_buffer = 0;
            line_num++;
            if (line_num >= MAX_INPUT_LINES )
            {
                Serial.println("ERROR: line buffer full.");
                line_num = 0;
            }
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
