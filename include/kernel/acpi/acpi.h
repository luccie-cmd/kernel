#if !defined(_KERNEL_ACPI_ACPI_H_)
#define _KERNEL_ACPI_ACPI_H_

namespace acpi
{
    void initialize();
    bool isInitialized();
    void* getTableBySign(char sign[4]);
    void printInfo();
};

#endif // _KERNEL_ACPI_ACPI_H_
