
/* ACPI_EXTRACT_ALL_CODE ssdp_proc_aml */

DefinitionBlock ("ssdt-proc.aml", "SSDT", 0x01, "BXPC", "BXSSDT", 0x1)
{
    
/* ACPI_EXTRACT_PROCESSOR_START ssdt_proc_start */

    
/* ACPI_EXTRACT_PROCESSOR_END ssdt_proc_end */

    
/* ACPI_EXTRACT_PROCESSOR_STRING ssdt_proc_name */

    Processor(CPAA, 0xAA, 0x00000000, 0x0) {
        
/* ACPI_EXTRACT_NAME_BYTE_CONST ssdt_proc_id */

        Name(ID, 0xAA)
        Name(_HID, "ACPI0007")
        External(CPMA, MethodObj)
        External(CPST, MethodObj)
        External(CPEJ, MethodObj)
        Method(_MAT, 0) {
            Return (CPMA(ID))
        }
        Method(_STA, 0) {
            Return (CPST(ID))
        }
        Method(_EJ0, 1, NotSerialized) {
            CPEJ(ID, Arg0)
        }
    }
}
