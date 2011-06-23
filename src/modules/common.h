void module_erase_screen(uint8_t count) {
    for (; count > 0; count--) {
        send_raw_byte(0, true);
    }
}
