#include <../limine/limine.h>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdlib>
#include <cstring>
#include <kernel/acpi/acpi.h>
#include <kernel/acpi/tables.h>
#include <kernel/mmu/vmm/vmm.h>
#define MODULE "ACPI"

namespace acpi
{
limine_rsdp_request rsdpRequest = {
    .id       = LIMINE_RSDP_REQUEST,
    .revision = 0,
    .response = nullptr,
};
bool   initialized = false;
XSDT*  xsdt        = nullptr;
size_t numEntries  = 0;
void   initialize()
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (rsdpRequest.response == nullptr)
    {
        dbg::printm(MODULE, "Limine failed to set RSDP request response\n");
        std::abort();
    }
    void* rsdpAddr = rsdpRequest.response->address;
    XSDP* xsdp     = reinterpret_cast<XSDP*>(rsdpAddr);
    if (xsdp->Revision == 0)
    {
        dbg::printm(MODULE, "RSDT is not supported, please buy new hardware that wasn't made "
                            "before 2000 (How did you even manage to boot?)\n");
        std::abort();
    }
    xsdt = reinterpret_cast<XSDT*>(mmu::vmm::makeVirtual(xsdp->XsdtAddress));
    if (xsdt == nullptr)
    {
        dbg::printm(MODULE, "Failed to acquire the XSDT\n");
        std::abort();
    }
    numEntries  = (xsdt->h.Length - sizeof(xsdt->h)) / sizeof(xsdt->Entries[0]);
    initialized = true;
    FADT* fadt  = reinterpret_cast<FADT*>(getTableBySign((char*)"FACP"));
    if (fadt->SMI_CommandPort == 0)
    {
        dbg::printm(MODULE, "ACPI mode already enabled. SMI command port == 0.\n");
        dbg::popTrace();
        return;
    }
    if ((fadt->AcpiEnable == fadt->AcpiDisable) == 0)
    {
        dbg::printm(MODULE,
                    "ACPI mode already enabled. fadt->AcpiEnable == fadt->AcpiDisable == 0\n");
        dbg::popTrace();
        return;
    }
    if (fadt->PM1aControlBlock & 1)
    {
        dbg::printm(MODULE, "ACPI mode already enabled. (fadt->PM1aControlBlock & 1) == 1\n");
        dbg::popTrace();
        return;
    }
    io::outb(fadt->SMI_CommandPort, fadt->AcpiEnable);
    while ((io::inw(fadt->PM1aControlBlock) & 1) == 0)
    {
        asm volatile("nop");
    }
    dbg::printm(MODULE, "Enabled ACPI mode\n");
    dbg::popTrace();
}
void* getTableBySign(char sign[4])
{
    if (!isInitialized())
    {
        initialize();
    }
    dbg::addTrace(__PRETTY_FUNCTION__);
    for (size_t i = 0; i < numEntries; ++i)
    {
        ACPISDTHeader* sdtHeader =
            reinterpret_cast<ACPISDTHeader*>(mmu::vmm::makeVirtual(xsdt->Entries[i]));
        if (std::memcmp(sign, sdtHeader->Signature, 4) == 0)
        {
            dbg::popTrace();
            return reinterpret_cast<void*>(sdtHeader);
        }
    }
    dbg::popTrace();
    dbg::printm(MODULE, "Table %.4s couldn't be found\n", sign);
    return nullptr;
}
bool isInitialized()
{
    return initialized;
}
void printInfo()
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    dbg::printm(MODULE, "TODO\n");
    std::abort();
    dbg::popTrace();
}
}; // namespace acpi