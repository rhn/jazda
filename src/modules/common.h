void erase_screen(uint8_t count) {
    for (; count > 0; count--) {
        send_raw_byte(0, true);
    }
}

void erase_module_screen(void) {
    set_row(4);
    set_column(0);
    erase_screen(2*84);
}
