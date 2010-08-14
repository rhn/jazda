void setup_cpu(void) {
// makes CPU run at 1 MHz
// clock prescaler, device clock must be 8 MHz
  clock_prescale_set(clock_div_8);
}

