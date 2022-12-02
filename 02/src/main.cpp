#include <Arduino.h>

void setup() {
  Serial.begin(921600);
}

char serial_buffer[256];
int iserial_buffer = 0;

int line_num = 0;

enum Moves
{
  Unknown = 0,
  Rock,
  Paper,
  Scissors
};

int total_score = 0;

Moves get_move(char in)
{
  switch(in)
  {
    case 'A' : return Moves::Rock;
    case 'B' : return Moves::Paper;
    case 'C' : return Moves::Scissors;
    case 'X' : return Moves::Rock;
    case 'Y' : return Moves::Paper;
    case 'Z' : return Moves::Scissors;
  }
  return Moves::Unknown;
}

Moves get_move2(Moves m1, char outcome )
{
  if (outcome == 'Y') return m1;
  switch(m1)
  {
    case Moves::Rock:
      if (outcome == 'X') return Moves::Scissors; // lose
      return Moves::Paper; // win
    case Moves::Paper:
      if (outcome == 'X') return Moves::Rock; // lose
      return Moves::Scissors; // win
    case Moves::Scissors:
      if (outcome == 'X') return Moves::Paper; // lose
      return Moves::Rock; // win
  }
  return Moves::Unknown;
}

int get_score( Moves m1, Moves m2 )
{
  int score = (int)m2;
  if (m1 == m2) return score+3;
  // Rock defeats Scissors, Scissors defeats Paper, and Paper defeats Rock
  if (m1 == Moves::Rock)
  {
    if (m2 == Moves::Paper) return score+6;
    if (m2 == Moves::Scissors) return score+0;
  }
  else if (m1 == Moves::Paper)
  {
    if (m2 == Moves::Rock) return score+0;
    if (m2 == Moves::Scissors) return score+6;
  }
  else if (m1 == Moves::Scissors)
  {
    if (m2 == Moves::Rock) return score+6;
    if (m2 == Moves::Paper) return score+0;
  }
  return score+0;
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
        Serial.print(serial_buffer[0]);
        Serial.print(" + ");
        Serial.print(serial_buffer[2]);
        Moves m1 = get_move(serial_buffer[0]);
        // Part 1
        // Moves m2 = get_move(serial_buffer[2]);
        // Part 2
        Moves m2 = get_move2(m1,serial_buffer[2]);
        int score = get_score(m1,m2);
        total_score += score;

        Serial.print(" => ");
        Serial.print(score);
        Serial.print(" sum: ");
        Serial.println(total_score);
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