#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdlib>
#include <cstring>
#include <kernel/acpi/acpi.h>
#include <kernel/acpi/tables.h>
#include <kernel/mmu/vmm/vmm.h>
#include <limine.h>
#define MODULE "ACPI"

namespace acpi {
limine_rsdp_request __attribute__((section(".limine_requests"))) rsdpRequest = {
    .id       = LIMINE_RSDP_REQUEST,
    .revision = 0,
    .response = nullptr,
};
bool   initialized = false;
XSDT*  xsdt        = nullptr;
RSDT*  rsdt        = nullptr;
size_t numEntries  = 0;
void   initialize() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (rsdpRequest.response == nullptr) {
        dbg::printm(MODULE, "Limine failed to set RSDP request response\n");
        std::abort();
    }
    void* rsdpAddr = rsdpRequest.response->address;
    XSDP* xsdp     = reinterpret_cast<XSDP*>(rsdpAddr);
    if (xsdp->Revision == 0) {
        rsdt = reinterpret_cast<RSDT*>(mmu::vmm::makeVirtual(xsdp->RsdtAddress));
        if (rsdt == nullptr) {
            dbg::printm(MODULE, "Failed to acquire the RSDT\n");
            std::abort();
        }
        numEntries = (rsdt->h.Length - sizeof(rsdt->h)) / sizeof(rsdt->Entries[0]);
    } else {
        xsdt = reinterpret_cast<XSDT*>(mmu::vmm::makeVirtual(xsdp->XsdtAddress));
        if (xsdt == nullptr) {
            dbg::printm(MODULE, "Failed to acquire the XSDT\n");
            std::abort();
        }
        numEntries = (xsdt->h.Length - sizeof(xsdt->h)) / sizeof(xsdt->Entries[0]);
    }
    initialized = true;
    FADT* fadt  = reinterpret_cast<FADT*>(getTableBySign((char*)"FACP"));
    if (fadt->SMI_CommandPort == 0) {
        dbg::printm(MODULE, "ACPI mode already enabled. SMI command port == 0.\n");
        dbg::popTrace();
        return;
    }
    if ((fadt->AcpiEnable == fadt->AcpiDisable) == 0) {
        dbg::printm(MODULE,
                      "ACPI mode already enabled. fadt->AcpiEnable == fadt->AcpiDisable == 0\n");
        dbg::popTrace();
        return;
    }
    if (fadt->PM1aControlBlock & 1) {
        dbg::printm(MODULE, "ACPI mode already enabled. (fadt->PM1aControlBlock & 1) == 1\n");
        dbg::popTrace();
        return;
    }
    io::outb(fadt->SMI_CommandPort, fadt->AcpiEnable);
    while ((io::inw(fadt->PM1aControlBlock) & 1) == 0) {
        asm volatile("nop");
    }
    dbg::printm(MODULE, "Enabled ACPI mode\n");
    dbg::popTrace();
}
void* getTableBySign(char sign[4]) {
    if (!isInitialized()) {
        initialize();
    }
    dbg::addTrace(__PRETTY_FUNCTION__);
    for (size_t i = 0; i < numEntries; ++i) {
        ACPISDTHeader* sdtHeader;
        if (xsdt != nullptr) {
            sdtHeader = reinterpret_cast<ACPISDTHeader*>(mmu::vmm::makeVirtual(xsdt->Entries[i]));
        } else if (rsdt != nullptr) {
            sdtHeader = reinterpret_cast<ACPISDTHeader*>(mmu::vmm::makeVirtual(rsdt->Entries[i]));
        } else {
            dbg::printm(MODULE, "Neither a RSDT nor XSDT found!!!\n");
            std::abort();
        }
        if (std::memcmp(sign, sdtHeader->Signature, 4) == 0) {
            dbg::popTrace();
            return reinterpret_cast<void*>(sdtHeader);
        }
    }
    dbg::popTrace();
    dbg::printm(MODULE, "Table %.4s couldn't be found\n", sign);
    return nullptr;
}
bool isInitialized() {
    return initialized;
}
void printInfo() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    dbg::printm(MODULE, "Tables:\n");
    for (size_t i = 0; i < numEntries; ++i) {
        ACPISDTHeader* sdtHeader;
        uint64_t       addr;
        if (xsdt != nullptr) {
            sdtHeader = reinterpret_cast<ACPISDTHeader*>(mmu::vmm::makeVirtual(xsdt->Entries[i]));
            addr      = (xsdt->Entries[i]);
        } else if (rsdt != nullptr) {
            sdtHeader = reinterpret_cast<ACPISDTHeader*>(mmu::vmm::makeVirtual(rsdt->Entries[i]));
            addr      = (rsdt->Entries[i]);
        } else {
            dbg::printm(MODULE, "Neither a RSDT nor XSDT found!!!\n");
            std::abort();
        }
        dbg::printm(MODULE, "\t- %4.4s 0x%016llx %06x\n", sdtHeader->Signature, addr,
                    sdtHeader->Length);
    }
    dbg::popTrace();
}
}; // namespace acpi