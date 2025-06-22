#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <common/ordered_map.h>
#include <drivers/input/kbd/ps2.h>
#include <kernel/hal/arch/x64/irq/irq.h>
#define MODULE "PS2 Keyboard driver"

// TODO: Keypad specific stuff. For now we just map them to their numbers and things
static OrderedMap<uint8_t, drivers::input::kbd::TranslatedKey> normalScancodes = {
    // {0x0D, '\t'},
    // {0x0E, '`'},
    // {0x15, 'q'},
    // {0x16, '1'},
    // {0x1A, 'z'},
    // {0x1B, 's'},
    // {0x1C, 'a'},
    // {0x1D, 'w'},
    // {0x1E, '2'},
    // {0x21, 'c'},
    // {0x22, 'x'},
    // {0x23, 'd'},
    // {0x24, 'e'},
    // {0x25, '4'},
    // {0x26, '3'},
    // {0x29, ' '},
    // {0x2A, 'v'},
    // {0x2B, 'f'},
    // {0x2C, 't'},
    // {0x2D, 'r'},
    // {0x2E, '5'},
    // {0x31, 'n'},
    // {0x32, 'b'},
    // {0x33, 'h'},
    // {0x34, 'g'},
    // {0x35, 'y'},
    // {0x36, '6'},
    // {0x3A, 'm'},
    // {0x3B, 'j'},
    // {0x3C, 'u'},
    // {0x3D, '7'},
    // {0x3E, '8'},
    // {0x41, ','},
    // {0x42, 'k'},
    // {0x43, 'i'},
    // {0x44, 'o'},
    // {0x45, '0'},
    // {0x46, '9'},
    // {0x49, '.'},
    // {0x4A, '/'},
    // {0x4B, 'l'},
    // {0x4C, ';'},
    // {0x4D, 'p'},
    // {0x4E, '-'},
    // {0x52, '\''},
    // {0x54, '['},
    // {0x55, '='},
    // {0x5A, '\n'},
    // {0x5B, ']'},
    // {0x5D, '\\'},
    // {0x66, '\b'},
    // // Keypad
    // {0x69, '1'},
    // {0x6B, '4'},
    // {0x6C, '7'},
    // {0x70, '0'},
    // {0x71, '.'},
    // {0x72, '2'},
    // {0x73, '5'},
    // {0x74, '6'},
    // {0x75, '8'},
    // {0x76, '\e'},
    // {0x79, '+'},
    // {0x7A, '3'},
    // {0x7B, '-'},
    // {0x7C, '*'},
    // {0x7D, '9'},
    // End keypad
};
static OrderedMap<uint8_t, drivers::input::kbd::TranslatedKey> shiftScancodes = {
    // {0x0D, '\t'},
    // {0x0E, '~'},
    // {0x15, 'Q'},
    // {0x16, '!'},
    // {0x1A, 'Z'},
    // {0x1B, 'S'},
    // {0x1C, 'A'},
    // {0x1D, 'W'},
    // {0x1E, '@'},
    // {0x21, 'C'},
    // {0x22, 'X'},
    // {0x23, 'D'},
    // {0x24, 'E'},
    // {0x25, '$'},
    // {0x26, '#'},
    // {0x29, ' '},
    // {0x2A, 'V'},
    // {0x2B, 'F'},
    // {0x2C, 'T'},
    // {0x2D, 'R'},
    // {0x2E, '%'},
    // {0x31, 'N'},
    // {0x32, 'B'},
    // {0x33, 'H'},
    // {0x34, 'G'},
    // {0x35, 'Y'},
    // {0x36, '^'},
    // {0x3A, 'M'},
    // {0x3B, 'J'},
    // {0x3C, 'U'},
    // {0x3D, '&'},
    // {0x3E, '*'},
    // {0x41, '<'},
    // {0x42, 'K'},
    // {0x43, 'I'},
    // {0x44, 'O'},
    // {0x45, ')'},
    // {0x46, '('},
    // {0x49, '>'},
    // {0x4A, '?'},
    // {0x4B, 'L'},
    // {0x4C, ':'},
    // {0x4D, 'P'},
    // {0x4E, '_'},
    // {0x52, '\"'},
    // {0x54, '{'},
    // {0x55, '+'},
    // {0x5A, '\n'},
    // {0x5B, '}'},
    // {0x5D, '|'},
    // {0x66, '\b'},
    // // Keypad
    // {0x69, '1'},
    // {0x6B, '4'},
    // {0x6C, '7'},
    // {0x70, '0'},
    // {0x71, '.'},
    // {0x72, '2'},
    // {0x73, '5'},
    // {0x74, '6'},
    // {0x75, '8'},
    // {0x76, '\e'},
    // {0x79, '+'},
    // {0x7A, '3'},
    // {0x7B, '-'},
    // {0x7C, '*'},
    // {0x7D, '9'},
    // End keypad
};
static drivers::input::kbd::TranslatedKey translateScancode(uint8_t scancode, bool pressed,
                                                            bool special) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    static bool shift    = false;
    static bool capsLock = false;
    switch (scancode) {
    case 0x59: // Right Shift
    case 0x12: // Left Shift
    {
        shift = capsLock ? !pressed : pressed;
        dbg::popTrace();
        return drivers::input::kbd::TranslatedKey::None;
    }
    case 0x58: // Capslock
    {
        capsLock = pressed ? !capsLock : capsLock;
        shift    = capsLock;
        dbg::popTrace();
        return drivers::input::kbd::TranslatedKey::None;
    } break;
    }
    if (!special) {
        auto map = shift ? shiftScancodes : normalScancodes;
        if (!map.contains(scancode)) {
            dbg::printm(MODULE, "TODO: Handle scancode 0x%hhx\n", scancode);
            dbg::popTrace();
            return drivers::input::kbd::TranslatedKey::None;
        }
        drivers::input::kbd::TranslatedKey asciiChar = map.at(scancode);
        dbg::popTrace();
        return asciiChar;
    }
    dbg::printm(MODULE, "TODO: Handle special scancode 0x%hhx\n", scancode);
    dbg::popTrace();
    return drivers::input::kbd::TranslatedKey::None;
}

namespace drivers::input::kbd {

static bool                           pressed = true;
static bool                           normal  = true;
static std::vector<KeyboardInputPair> buffer;

static void keyboardPress(io::Registers* regs) {
    (void)regs;
    uint8_t byte = io::inb(0x60);
    if (byte == 0xE0) {
        normal = false;
    } else if (byte == 0xF0) {
        pressed = false;
    } else {
        TranslatedKey translated = translateScancode(byte, pressed, !normal);
        buffer.push_back(KeyboardInputPair(translated, !normal, !pressed));
        pressed = true;
        normal  = true;
    }
}
PS2Driver::PS2Driver() : KeyboardDriver(KeyboardType::PS2) {
    auto wait_input_buffer_clear = []() -> bool {
        while ((io::inb(0x64) & 0x02) == 0) {
            __asm__ volatile("nop");
            __asm__ volatile("mfence" : : : "memory");
        }
        return true;
    };

    auto wait_output_buffer_full = []() -> bool {
        while ((io::inb(0x64) & 0x01) == 0) {
            __asm__ volatile("nop");
            __asm__ volatile("mfence" : : : "memory");
        }
        return true;
    };

    auto send_command = [&](uint8_t cmd, uint8_t data = 0, bool wait = false) -> bool {
        if (wait) {
            if (!wait_input_buffer_clear()) {
                dbg::printm(MODULE, "Timeout waiting to send command 0x%hhx\n", cmd);
                return false;
            }
        }
        dbg::printm(MODULE, "Sending command 0x%hx\n", cmd);
        if (cmd > 0xED) {
            io::outb(0x60, cmd);
        } else {
            io::outb(0x64, cmd);
        }

        if (data != 0 || cmd == 0xF0) {
            dbg::printm(MODULE, "Sending data 0x%hx\n", data);
            if (wait) {
                if (!wait_input_buffer_clear()) {
                    dbg::printm(MODULE, "Timeout waiting to send data 0x%hhx\n", data);
                    return false;
                }
            }
            io::outb(0x60, data);
        }
        return true;
    };
    dbg::addTrace(__PRETTY_FUNCTION__);
    dbg::printm(MODULE, "Initializing PS/2 Keyboard\n");
    if (!send_command(0xAD) || !send_command(0xA7)) {
        dbg::printm(MODULE, "Failed to disable PS/2 ports\n");
        std::abort();
    }
    while (io::inb(0x64) & 0x01) {
        (void)io::inb(0x60);
    }
    if (!send_command(0xAA)) {
        dbg::printm(MODULE, "Self test failed\n");
        std::abort();
    }
    if (!wait_output_buffer_full()) {
        dbg::printm(MODULE, "Self test failed\n");
        std::abort();
    }
    if (io::inb(0x60) != 0x55) {
        dbg::printm(MODULE, "Controller self-test failed\n");
        std::abort();
    }
    if (!send_command(0xAB)) {
        dbg::printm(MODULE, "Keyboard port test failed\n");
        std::abort();
    }
    if (!wait_output_buffer_full()) {
        dbg::printm(MODULE, "Keyboard port test failed\n");
        std::abort();
    }
    if (io::inb(0x60) != 0x00) {
        dbg::printm(MODULE, "Keyboard port test failed\n");
        std::abort();
    }
    const uint8_t target_config = 0b00100101;
    if (!send_command(0x60, target_config)) {
        dbg::printm(MODULE, "Controller failed to recieve configuration\n");
        std::abort();
    }
    if (!send_command(0x20)) {
        dbg::printm(MODULE, "Failed to send read config command\n");
        std::abort();
    }
    if (!wait_output_buffer_full()) {
        dbg::printm(MODULE, "Failed to send read config command\n");
        std::abort();
    }
    uint8_t current_config = io::inb(0x60);
    if (current_config != target_config) {
        dbg::printm(MODULE, "Config mismatch (got 0x%hx, wanted 0x%hx)\n", current_config,
                    target_config);
        std::abort();
    }
    if (!send_command(0xAE)) {
        dbg::printm(MODULE, "Failed to enable keyboard\n");
        std::abort();
    }
    auto set_scancode = [&](uint8_t set) -> bool {
        if (!send_command(0xF0, set)) return false;
        if (!wait_output_buffer_full()) return false;
        return io::inb(0x60) == 0xFA;
    };
    if (!set_scancode(0x00)) {
        dbg::printm(MODULE, "Failed send scancode get command\n");
        std::abort();
    }
    if (!wait_output_buffer_full()) {
        dbg::printm(MODULE, "Output buffer timeout\n");
        std::abort();
    }
    uint8_t scancode_set = io::inb(0x60);
    if (scancode_set != 0x02) {
        dbg::printm(MODULE, "Setting scancode set 2 (current: 0x%hx)\n", scancode_set);
        if (!set_scancode(0x02)) {
            dbg::printm(MODULE, "Failed to set scancode set 2\n");
            std::abort();
        }
    }
    hal::arch::x64::irq::overrideIrq(1, keyboardPress);
    dbg::printm(MODULE, "Initialized PS/2 keyboard\n");
    dbg::popTrace();
}
PS2Driver::~PS2Driver() {}
void PS2Driver::init(pci::device* dev) {
    (void)dev;
    dbg::addTrace(__PRETTY_FUNCTION__);
    dbg::printm(MODULE, "PS/2 keyboard shouldn't be initialized with PCI device!!!\n");
    std::abort();
}
void PS2Driver::deinit() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    dbg::printm(MODULE, "PS/2 keyboard shouldn't be deinitialized with PCI device!!!\n");
    std::abort();
}
std::vector<KeyboardInputPair> PS2Driver::getKeyPresses(size_t number) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    std::vector<KeyboardInputPair> retBuffer;
    while (buffer.size() < number) {
        __builtin_ia32_pause();
    }
    for (KeyboardInputPair b : buffer) {
        if (number == 0) {
            dbg::popTrace();
            buffer.clear();
            return retBuffer;
        }
        number--;
        retBuffer.push_back(b);
    }
    __builtin_unreachable();
}
PS2Driver* loadPS2Driver() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    PS2Driver* ps2Driver = new PS2Driver();
    // while (1) {
    //     while ((io::inb(0x64) & 1) == 0);
    //     uint8_t byte    = io::inb(0x60);
    //     bool    pressed = true;
    //     bool    normal  = true;
    //     if (byte == 0xE0) {
    //         byte   = io::inb(0x60);
    //         normal = false;
    //     }
    //     if (byte == 0xF0) {
    //         byte    = io::inb(0x60);
    //         pressed = false;
    //     }
    //     dbg::printf("%c", translateScancode(byte, pressed, !normal));
    //     dbg::printf("%c %s%s\n", translateScancode(byte, pressed, !normal),
    //     pressed ? "pressed" : "released", normal ? "" : " multimedia");
    // }
    dbg::printm(MODULE, "Added PS/2 keyboard\n");
    dbg::popTrace();
    return ps2Driver;
}
}; // namespace drivers::input::kbd