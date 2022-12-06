#pragma once

void read_serial();
void start_of_file();
void end_of_file();
void process_line();

extern bool state_bof;
extern bool state_eof;
extern bool state_eol;

