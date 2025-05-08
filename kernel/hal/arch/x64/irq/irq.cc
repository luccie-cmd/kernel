#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdlib>
#include <kernel/acpi/acpi.h>
#include <kernel/acpi/tables.h>
#include <kernel/hal/arch/x64/irq/irq.h>
#include <kernel/mmu/vmm/vmm.h>
#define MODULE "IRQ"

#define IA32_APIC_BASE_MSR 0x1B

#define LAPIC_ID_REGISTER 0x20
#define LAPIC_VERSION_REGISTER 0x30
#define LAPIC_TASK_PRIORITY_REGISTER 0x80
#define LAPIC_ARBITRATION_PRIORITY_REGISTER 0x90
#define LAPIC_PROCESSOR_PRIORITY_REGISTER 0xA0
#define LAPIC_EOI_REGISTER 0xB0
#define LAPIC_REMOTE_READ_REGISTER 0xC0
#define LAPIC_LOGICAL_DESTINATION_REGISTER 0xD0
#define LAPIC_DESTINATION_FORMAT_REGISTER 0xE0
#define LAPIC_SPURIOUS_INTERRUPT_VECTOR_REGISTER 0xF0
#define LAPIC_IN_SERVICE_REGISTER_REGISTER 0x100
#define LAPIC_TRIGGER_MODE_REGISTER 0x180
#define LAPIC_INTERRUPT_REQUEST_REGISTER 0x200
#define LAPIC_ERROR_STATUS_REGISTER 0x280
#define LAPIC_LVT_CMCI_REGISTER 0x2F0 // LVT Corrected Machine Check Interrupt (CMCI) Register
#define LAPIC_INTERRUPT_COMMAND_REGISTER 0x300
#define LAPIC_LVT_TIMER_REGISTER 0x320
#define LAPIC_LVT_THERMAL_SENSOR_REGISTER 0x330
#define LAPIC_LVT_PMC_REGISTER 0x340 // LVT Performance Monitoring Counters Register
#define LAPIC_LVT_LINT0_REGISTER 0x350
#define LAPIC_LVT_LINT1_REGISTER 0x360
#define LAPIC_LVT_ERROR_REGISTER 0x370
#define LAPIC_INITIAL_COUNT_REGISTER_TIMER_REGISTER 0x380
#define LAPIC_CURRENT_COUNT_TIMER_REGISTER 0x390
#define LAPIC_DIVIDE_CONFIGURATION_REGISTER_TIMER_REGISTER 0x3E0

namespace hal::arch::x64::irq {
uint64_t lapicAddr = 0;

struct IOApicDesc {
    uint64_t base;
    uint32_t minGSI;
    uint32_t maxGSI;
};
struct ISODesc {
    uint8_t  source;
    uint8_t  bus;
    uint32_t gsi;
};
std::vector<IOApicDesc*> ioApicdescs;
std::vector<ISODesc*>    ioApicIsodescs;

static void lapicWrite(uint32_t offset, uint32_t value) {
    *(volatile uint32_t*)(lapicAddr + offset) = value;
}

static uint32_t lapicRead(uint32_t offset) {
    return *(volatile uint32_t*)(lapicAddr + offset);
}

static uint32_t ioapicRead(uint32_t base, uint32_t offset) {
    volatile uint32_t* ioapic_index = reinterpret_cast<volatile uint32_t*>(base);
    volatile uint32_t* ioapic_data  = reinterpret_cast<volatile uint32_t*>(base + 0x10);
    *ioapic_index                   = offset;
    return *ioapic_data;
}

static uint32_t getMaxRedirections(uint32_t addr) {
    return ((ioapicRead(addr, 0x01) >> 16) & 0xFF) + 1;
}

static uint64_t getAPICBase() {
    return io::rdmsr(IA32_APIC_BASE_MSR);
}

static void setAPICBase(uint64_t base) {
    io::wrmsr(IA32_APIC_BASE_MSR, (base & 0xFFFFF000) | (getAPICBase() & 0xFFF) | (1 << 11));
}

void init() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    acpi::MADT* madt = reinterpret_cast<acpi::MADT*>(acpi::getTableBySign((char*)"APIC"));
    if (madt == nullptr) {
        dbg::printm(MODULE, "TODO: Handle dual PIC\n");
        std::abort();
    }
    io::outb(0x21, 0xFF); // Mask PIC master
    io::outb(0xA1, 0xFF); // Mask PIC slave
    lapicAddr = mmu::vmm::makeVirtual(madt->lapic_address);
    ioApicdescs.clear();
    uint8_t* entries_start = reinterpret_cast<uint8_t*>(madt) + sizeof(acpi::MADT);
    uint8_t* entries_end   = reinterpret_cast<uint8_t*>(madt) + madt->sdt.Length;

    uint8_t* current_entry = entries_start;
    while (current_entry < entries_end) {
        acpi::MADTEntry* entry = reinterpret_cast<acpi::MADTEntry*>(current_entry);
        switch (entry->entry_type) {
        case MADT_ENTRY_TYPE_LAPIC: {
            acpi::MADTLAPIC* lapic = reinterpret_cast<acpi::MADTLAPIC*>(entry);
            if ((lapic->flags & 1) ^ ((lapic->flags >> 1) & 1)) {
                setAPICBase(getAPICBase());
                lapicWrite(LAPIC_SPURIOUS_INTERRUPT_VECTOR_REGISTER,
                           lapicRead(LAPIC_SPURIOUS_INTERRUPT_VECTOR_REGISTER) | 0x100);
            }
        } break;
        case MADT_ENTRY_TYPE_IOAPIC: {
            acpi::MADTIOAPIC* ioapic = reinterpret_cast<acpi::MADTIOAPIC*>(entry);
            ioApicdescs.push_back(new IOApicDesc{
                mmu::vmm::makeVirtual(ioapic->ioapic_address), ioapic->gsi_base,
                ioapic->gsi_base +
                    getMaxRedirections(mmu::vmm::makeVirtual(ioapic->ioapic_address))});
        } break;
        case MADT_ENTRY_TYPE_IOAPIC_INTERRUPT_SOURCE_OVERRIDE: {
            acpi::MADTIOAPICISO* ioapiciso = reinterpret_cast<acpi::MADTIOAPICISO*>(entry);
            ioApicIsodescs.push_back(
                new ISODesc{ioapiciso->irq_source, ioapiciso->bus_source, ioapiciso->gsi});
        } break;
        case MADT_ENTRY_TYPE_LAPIC_NON_MASKABLE_INTERRUPTS: {
            acpi::MADTLAPICNMI* lapicnmi = reinterpret_cast<acpi::MADTLAPICNMI*>(entry);
            uint32_t            lvt      = 0x400;
            if (lapicnmi->flags & 0x2) {
                lvt |= (1 << 13);
            }
            if (lapicnmi->flags & 0x8) {
                lvt |= (1 << 15);
            }
            lvt &= ~(1 << 16);
            if (lapicnmi->lint == 0) {
                lapicWrite(LAPIC_LVT_LINT0_REGISTER, lvt);
            } else if (lapicnmi->lint == 1) {
                lapicWrite(LAPIC_LVT_LINT1_REGISTER, lvt);
            }
        } break;
        default: {
            dbg::printm(MODULE, "TODO: Handle MADT entry type %hhu\n", entry->entry_type);
            std::abort();
        } break;
        }
        current_entry += entry->entry_length;
    }
    dbg::printm(MODULE, "Initialized\n");
    dbg::popTrace();
}
}; // namespace hal::arch::x64::irq