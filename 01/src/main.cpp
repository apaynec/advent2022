#include <Arduino.h>

void setup() {
  Serial.begin(921600);
}

char serial_buffer[256];
int iserial_buffer = 0;

int line_num = 0;

int elf_highest = 0;
int elf_highest_sum = 0;
int elf_current = 0;
int elf_num = 0;
int top_3[3] = {0,0,0};

void check_top(int sum)
{
  for(int i=0; i < 3; i++)
  {
    if (sum >= top_3[i])
    {
      // shuffle down
      for (int j=1; j >= i; j--)
      {
        top_3[j+1] = top_3[j];
      }
      top_3[i] = sum;
      break;  
    }
  }
}

void dump_top()
{
  int sum = 0;
  Serial.print("top3: ");
  for(int i=0; i < 3; i++)
  {
    sum += top_3[i];
    Serial.print(top_3[i]);
    if (i < 2) Serial.print("+");
  }
  Serial.print(" = ");
  Serial.println(sum);
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
      if (iserial_buffer)
      {
        Serial.print(line_num);
        Serial.print(": ");
        Serial.println(serial_buffer);
      }
      if (elf_current && iserial_buffer == 0)
      {
        // new elf
        Serial.println("==============");
        Serial.print("elf");
        Serial.print(elf_num);
        Serial.print(" calories: ");
        Serial.println(elf_current);
        
        check_top(elf_current);

        if (elf_current > elf_highest_sum){
          elf_highest_sum = elf_current;
          elf_highest = elf_num;
        }

        Serial.print("highest: elf");
        Serial.print(elf_highest);
        Serial.print(" calories: ");
        Serial.println(elf_highest_sum);

        dump_top();


        elf_num++;
        elf_current = 0;
      }
      else
      {
        elf_current += atoi(serial_buffer);
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
  static uint32_t t_last = millis();
  unsigned int tnow = millis();
  //if (tnow - t_last > 20000)
  //{
  //  t_last = tnow;
  //  Serial.println("HO");
  //}

  read_serial();
}