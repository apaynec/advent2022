#include <Arduino.h>

void setup() {
  Serial.begin(921600);
}

char serial_buffer[256];
int iserial_buffer = 0;

int line_num = 0;

int item_sum = 0;
int badge_sum = 0;

String sack_stack[2];

char find_dupe(const char * in, int len)
{
  for (int i=0;i<len/2;i++)
  {
    for (int j=len/2;j<len;j++)
    {
      if (in[i] == in[j]) return in[i];
    }
  }
  return 0;
}

int get_value(char ch)
{
  if (ch >= 'a' && ch <= 'z') return ch - 'a' + 1;
  if (ch >= 'A' && ch <= 'Z') return ch - 'A' + 27;
  return 0;
}

char find_badge(const char * in, int len)
{
  for (int i=0;i<len;i++)
  {
    bool is_badge = true;
    for (int j=0;j<2;j++)
    {
      int idx = sack_stack[j].indexOf(in[i]);
      if (idx == -1) 
      {
        is_badge = false;
        break;
      }
    }
    if (is_badge) return in[i];
  }
  return 0;
}

void read_serial()
{
  while (Serial.available() > 0)
  {
    int ch = Serial.read();
    
    // Serial.write(ch);

    if (ch == 10)
    {
      serial_buffer[iserial_buffer] = '\0';
      if (iserial_buffer > 2)
      {
        Serial.print(line_num);
        Serial.print(": ");

        char ch = find_dupe(serial_buffer,iserial_buffer);

        int value = get_value(ch);

        item_sum += value;

        Serial.print(ch);
        Serial.print(" ");
        Serial.print(value);

        Serial.print(" sum: ");
        Serial.println(item_sum);

        int rank = ((line_num+1) % 3);
        if (rank == 0)
        {
          // check last against stack
          char badge = find_badge(serial_buffer,iserial_buffer);
          int badge_val = get_value(badge);
          badge_sum += badge_val;

          Serial.print("badge: ");
          Serial.print(badge);
          Serial.print(" ");
          Serial.print(badge_val);

          Serial.print(" bsum: ");
          Serial.println(badge_sum);

        }
        else
        {
          sack_stack[rank-1] = serial_buffer;
        }


      }
      iserial_buffer = 0;
      line_num++;
    }
    else if (ch != 13)
    {
        if (iserial_buffer >= sizeof(serial_buffer))
        {
          Serial.println("ERROR: buffer full.");
          iserial_buffer = 0;
        }
        serial_buffer[iserial_buffer] = ch;
        iserial_buffer++;
    }
  }
}

void loop() {
  read_serial();
}