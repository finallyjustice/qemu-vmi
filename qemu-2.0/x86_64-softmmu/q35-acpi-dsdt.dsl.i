
/* ACPI_EXTRACT_ALL_CODE Q35AcpiDsdtAmlCode */

DefinitionBlock (
    "q35-acpi-dsdt.aml",
    "DSDT",
    0x01,
    "BXPC",
    "BXDSDT",
    0x2
    )
{
Scope(\) {
    OperationRegion(DBG, SystemIO, 0x0402, 0x01)
    Field(DBG, ByteAcc, NoLock, Preserve) {
        DBGB, 8,
    }
    Method(DBUG, 1) {
        ToHexString(Arg0, Local0)
        ToBuffer(Local0, Local0)
        Subtract(SizeOf(Local0), 1, Local1)
        Store(Zero, Local2)
        While (LLess(Local2, Local1)) {
            Store(DerefOf(Index(Local0, Local2)), DBGB)
            Increment(Local2)
        }
        Store(0x0A, DBGB)
    }
}
    Scope(\_SB) {
        OperationRegion(PCST, SystemIO, 0xae00, 0x0c)
        OperationRegion(PCSB, SystemIO, 0xae0c, 0x01)
        Field(PCSB, AnyAcc, NoLock, WriteAsZeros) {
            PCIB, 8,
        }
    }
    Scope(\_SB) {
        Device(PCI0) {
            Name(_HID, EisaId("PNP0A08"))
            Name(_CID, EisaId("PNP0A03"))
            Name(_ADR, 0x00)
            Name(_UID, 1)
            External(ISA, DeviceObj)
            Name(SUPP, 0)
            Name(CTRL, 0)
            Method(_OSC, 4) {
                CreateDWordField(Arg3, 0, CDW1)
                If (LEqual(Arg0, ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))) {
                    CreateDWordField(Arg3, 4, CDW2)
                    CreateDWordField(Arg3, 8, CDW3)
                    Store(CDW2, SUPP)
                    Store(CDW3, CTRL)
                    And(CTRL, 0x1D, CTRL)
                    If (LNotEqual(Arg1, One)) {
                        Or(CDW1, 0x08, CDW1)
                    }
                    If (LNotEqual(CDW3, CTRL)) {
                        Or(CDW1, 0x10, CDW1)
                    }
                    Store(CTRL, CDW3)
                } Else {
                    Or(CDW1, 4, CDW1)
                }
                Return (Arg3)
            }
        }
    }
Scope(\_SB.PCI0) {
    Name(CRES, ResourceTemplate() {
        WordBusNumber(ResourceProducer, MinFixed, MaxFixed, PosDecode,
            0x0000,
            0x0000,
            0x00FF,
            0x0000,
            0x0100,
            ,, )
        IO(Decode16,
            0x0CF8,
            0x0CF8,
            0x01,
            0x08,
            )
        WordIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange, 0x0000, 0x0000, 0x0CD7, 0x0000, 0x0CD8, ,, , TypeStatic) WordIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange, 0x0000, 0x0D00, 0xFFFF, 0x0000, 0xF300, ,, , TypeStatic)
        DWordMemory(ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
            0x00000000,
            0x000A0000,
            0x000BFFFF,
            0x00000000,
            0x00020000,
            ,, , AddressRangeMemory, TypeStatic)
        DWordMemory(ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
            0x00000000,
            0xE0000000,
            0xFEBFFFFF,
            0x00000000,
            0x1EC00000,
            ,, PW32, AddressRangeMemory, TypeStatic)
    })
    Name(CR64, ResourceTemplate() {
        QWordMemory(ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
            0x00000000,
            0x8000000000,
            0xFFFFFFFFFF,
            0x00000000,
            0x8000000000,
            ,, PW64, AddressRangeMemory, TypeStatic)
    })
    Method(_CRS, 0) {
        External(P0S, IntObj)
        External(P0E, IntObj)
        External(P1V, IntObj)
        External(P1S, BuffObj)
        External(P1E, BuffObj)
        External(P1L, BuffObj)
        CreateDWordField(CRES, \_SB.PCI0.PW32._MIN, PS32)
        CreateDWordField(CRES, \_SB.PCI0.PW32._MAX, PE32)
        CreateDWordField(CRES, \_SB.PCI0.PW32._LEN, PL32)
        Store(P0S, PS32)
        Store(P0E, PE32)
        Store(Add(Subtract(P0E, P0S), 1), PL32)
        If (LEqual(P1V, Zero)) {
            Return (CRES)
        }
        CreateQWordField(CR64, \_SB.PCI0.PW64._MIN, PS64)
        CreateQWordField(CR64, \_SB.PCI0.PW64._MAX, PE64)
        CreateQWordField(CR64, \_SB.PCI0.PW64._LEN, PL64)
        Store(P1S, PS64)
        Store(P1E, PE64)
        Store(P1L, PL64)
        ConcatenateResTemplate(CRES, CR64, Local0)
        Return (Local0)
    }
}
Scope(\_SB) {
    Device(HPET) {
        Name(_HID, EISAID("PNP0103"))
        Name(_UID, 0)
        OperationRegion(HPTM, SystemMemory, 0xFED00000, 0x400)
        Field(HPTM, DWordAcc, Lock, Preserve) {
            VEND, 32,
            PRD, 32,
        }
        Method(_STA, 0, NotSerialized) {
            Store(VEND, Local0)
            Store(PRD, Local1)
            ShiftRight(Local0, 16, Local0)
            If (LOr(LEqual(Local0, 0), LEqual(Local0, 0xffff))) {
                Return (0x0)
            }
            If (LOr(LEqual(Local1, 0), LGreater(Local1, 100000000))) {
                Return (0x0)
            }
            Return (0x0F)
        }
        Name(_CRS, ResourceTemplate() {
            Memory32Fixed(ReadOnly,
                0xFED00000,
                0x00000400,
                )
        })
    }
}
    Scope(\_SB.PCI0) {
        Device(ISA) {
            Name (_ADR, 0x001F0000)
            OperationRegion(PIRQ, PCI_Config, 0x60, 0x0C)
            OperationRegion(LPCD, PCI_Config, 0x80, 0x2)
            Field(LPCD, AnyAcc, NoLock, Preserve) {
                COMA, 3,
                    , 1,
                COMB, 3,
                Offset(0x01),
                LPTD, 2,
                    , 2,
                FDCD, 2
            }
            OperationRegion(LPCE, PCI_Config, 0x82, 0x2)
            Field(LPCE, AnyAcc, NoLock, Preserve) {
                CAEN, 1,
                CBEN, 1,
                LPEN, 1,
                FDEN, 1
            }
        }
    }
Scope(\_SB.PCI0.ISA) {
    Device (SMC) {
        Name(_HID, EisaId("APP0001"))
        
/* ACPI_EXTRACT_NAME_BYTE_CONST q35_dsdt_applesmc_sta */

        Name(_STA, 0xF0)
        Name(_CRS, ResourceTemplate () {
            IO (Decode16, 0x0300, 0x0300, 0x01, 0x20)
            IRQNoFlags() { 6 }
        })
    }
    Device(RTC) {
        Name(_HID, EisaId("PNP0B00"))
        Name(_CRS, ResourceTemplate() {
            IO(Decode16, 0x0070, 0x0070, 0x10, 0x02)
            IRQNoFlags() { 8 }
            IO(Decode16, 0x0072, 0x0072, 0x02, 0x06)
        })
    }
    Device(KBD) {
        Name(_HID, EisaId("PNP0303"))
        Method(_STA, 0, NotSerialized) {
            Return (0x0f)
        }
        Name(_CRS, ResourceTemplate() {
            IO(Decode16, 0x0060, 0x0060, 0x01, 0x01)
            IO(Decode16, 0x0064, 0x0064, 0x01, 0x01)
            IRQNoFlags() { 1 }
        })
    }
    Device(MOU) {
        Name(_HID, EisaId("PNP0F13"))
        Method(_STA, 0, NotSerialized) {
            Return (0x0f)
        }
        Name(_CRS, ResourceTemplate() {
            IRQNoFlags() { 12 }
        })
    }
    Device(FDC0) {
        Name(_HID, EisaId("PNP0700"))
        Method(_STA, 0, NotSerialized) {
            Store(FDEN, Local0)
            If (LEqual(Local0, 0)) {
                Return (0x00)
            } Else {
                Return (0x0F)
            }
        }
        Name(_CRS, ResourceTemplate() {
            IO(Decode16, 0x03F2, 0x03F2, 0x00, 0x04)
            IO(Decode16, 0x03F7, 0x03F7, 0x00, 0x01)
            IRQNoFlags() { 6 }
            DMA(Compatibility, NotBusMaster, Transfer8) { 2 }
        })
    }
    Device(LPT) {
        Name(_HID, EisaId("PNP0400"))
        Method(_STA, 0, NotSerialized) {
            Store(LPEN, Local0)
            If (LEqual(Local0, 0)) {
                Return (0x00)
            } Else {
                Return (0x0F)
            }
        }
        Name(_CRS, ResourceTemplate() {
            IO(Decode16, 0x0378, 0x0378, 0x08, 0x08)
            IRQNoFlags() { 7 }
        })
    }
    Device(COM1) {
        Name(_HID, EisaId("PNP0501"))
        Name(_UID, 0x01)
        Method(_STA, 0, NotSerialized) {
            Store(CAEN, Local0)
            If (LEqual(Local0, 0)) {
                Return (0x00)
            } Else {
                Return (0x0F)
            }
        }
        Name(_CRS, ResourceTemplate() {
            IO(Decode16, 0x03F8, 0x03F8, 0x00, 0x08)
            IRQNoFlags() { 4 }
        })
    }
    Device(COM2) {
        Name(_HID, EisaId("PNP0501"))
        Name(_UID, 0x02)
        Method(_STA, 0, NotSerialized) {
            Store(CBEN, Local0)
            If (LEqual(Local0, 0)) {
                Return (0x00)
            } Else {
                Return (0x0F)
            }
        }
        Name(_CRS, ResourceTemplate() {
            IO(Decode16, 0x02F8, 0x02F8, 0x00, 0x08)
            IRQNoFlags() { 3 }
        })
    }
}
    Name(\PICF, Zero)
    Method(\_PIC, 1, NotSerialized) {
        Store(Arg0, \PICF)
    }
    Scope(\_SB) {
        Scope(PCI0) {
            Name(PRTP, package() {
                Package() { 0x0000ffff, 0, LNKE, 0 }, Package() { 0x0000ffff, 1, LNKF, 0 }, Package() { 0x0000ffff, 2, LNKG, 0 }, Package() { 0x0000ffff, 3, LNKH, 0 },
                Package() { 0x0001ffff, 0, LNKF, 0 }, Package() { 0x0001ffff, 1, LNKG, 0 }, Package() { 0x0001ffff, 2, LNKH, 0 }, Package() { 0x0001ffff, 3, LNKE, 0 },
                Package() { 0x0002ffff, 0, LNKG, 0 }, Package() { 0x0002ffff, 1, LNKH, 0 }, Package() { 0x0002ffff, 2, LNKE, 0 }, Package() { 0x0002ffff, 3, LNKF, 0 },
                Package() { 0x0003ffff, 0, LNKH, 0 }, Package() { 0x0003ffff, 1, LNKE, 0 }, Package() { 0x0003ffff, 2, LNKF, 0 }, Package() { 0x0003ffff, 3, LNKG, 0 },
                Package() { 0x0004ffff, 0, LNKE, 0 }, Package() { 0x0004ffff, 1, LNKF, 0 }, Package() { 0x0004ffff, 2, LNKG, 0 }, Package() { 0x0004ffff, 3, LNKH, 0 },
                Package() { 0x0005ffff, 0, LNKF, 0 }, Package() { 0x0005ffff, 1, LNKG, 0 }, Package() { 0x0005ffff, 2, LNKH, 0 }, Package() { 0x0005ffff, 3, LNKE, 0 },
                Package() { 0x0006ffff, 0, LNKG, 0 }, Package() { 0x0006ffff, 1, LNKH, 0 }, Package() { 0x0006ffff, 2, LNKE, 0 }, Package() { 0x0006ffff, 3, LNKF, 0 },
                Package() { 0x0007ffff, 0, LNKH, 0 }, Package() { 0x0007ffff, 1, LNKE, 0 }, Package() { 0x0007ffff, 2, LNKF, 0 }, Package() { 0x0007ffff, 3, LNKG, 0 },
                Package() { 0x0008ffff, 0, LNKE, 0 }, Package() { 0x0008ffff, 1, LNKF, 0 }, Package() { 0x0008ffff, 2, LNKG, 0 }, Package() { 0x0008ffff, 3, LNKH, 0 },
                Package() { 0x0009ffff, 0, LNKF, 0 }, Package() { 0x0009ffff, 1, LNKG, 0 }, Package() { 0x0009ffff, 2, LNKH, 0 }, Package() { 0x0009ffff, 3, LNKE, 0 },
                Package() { 0x000affff, 0, LNKG, 0 }, Package() { 0x000affff, 1, LNKH, 0 }, Package() { 0x000affff, 2, LNKE, 0 }, Package() { 0x000affff, 3, LNKF, 0 },
                Package() { 0x000bffff, 0, LNKH, 0 }, Package() { 0x000bffff, 1, LNKE, 0 }, Package() { 0x000bffff, 2, LNKF, 0 }, Package() { 0x000bffff, 3, LNKG, 0 },
                Package() { 0x000cffff, 0, LNKE, 0 }, Package() { 0x000cffff, 1, LNKF, 0 }, Package() { 0x000cffff, 2, LNKG, 0 }, Package() { 0x000cffff, 3, LNKH, 0 },
                Package() { 0x000dffff, 0, LNKF, 0 }, Package() { 0x000dffff, 1, LNKG, 0 }, Package() { 0x000dffff, 2, LNKH, 0 }, Package() { 0x000dffff, 3, LNKE, 0 },
                Package() { 0x000effff, 0, LNKG, 0 }, Package() { 0x000effff, 1, LNKH, 0 }, Package() { 0x000effff, 2, LNKE, 0 }, Package() { 0x000effff, 3, LNKF, 0 },
                Package() { 0x000fffff, 0, LNKH, 0 }, Package() { 0x000fffff, 1, LNKE, 0 }, Package() { 0x000fffff, 2, LNKF, 0 }, Package() { 0x000fffff, 3, LNKG, 0 },
                Package() { 0x0010ffff, 0, LNKE, 0 }, Package() { 0x0010ffff, 1, LNKF, 0 }, Package() { 0x0010ffff, 2, LNKG, 0 }, Package() { 0x0010ffff, 3, LNKH, 0 },
                Package() { 0x0011ffff, 0, LNKF, 0 }, Package() { 0x0011ffff, 1, LNKG, 0 }, Package() { 0x0011ffff, 2, LNKH, 0 }, Package() { 0x0011ffff, 3, LNKE, 0 },
                Package() { 0x0012ffff, 0, LNKG, 0 }, Package() { 0x0012ffff, 1, LNKH, 0 }, Package() { 0x0012ffff, 2, LNKE, 0 }, Package() { 0x0012ffff, 3, LNKF, 0 },
                Package() { 0x0013ffff, 0, LNKH, 0 }, Package() { 0x0013ffff, 1, LNKE, 0 }, Package() { 0x0013ffff, 2, LNKF, 0 }, Package() { 0x0013ffff, 3, LNKG, 0 },
                Package() { 0x0014ffff, 0, LNKE, 0 }, Package() { 0x0014ffff, 1, LNKF, 0 }, Package() { 0x0014ffff, 2, LNKG, 0 }, Package() { 0x0014ffff, 3, LNKH, 0 },
                Package() { 0x0015ffff, 0, LNKF, 0 }, Package() { 0x0015ffff, 1, LNKG, 0 }, Package() { 0x0015ffff, 2, LNKH, 0 }, Package() { 0x0015ffff, 3, LNKE, 0 },
                Package() { 0x0016ffff, 0, LNKG, 0 }, Package() { 0x0016ffff, 1, LNKH, 0 }, Package() { 0x0016ffff, 2, LNKE, 0 }, Package() { 0x0016ffff, 3, LNKF, 0 },
                Package() { 0x0017ffff, 0, LNKH, 0 }, Package() { 0x0017ffff, 1, LNKE, 0 }, Package() { 0x0017ffff, 2, LNKF, 0 }, Package() { 0x0017ffff, 3, LNKG, 0 },
                Package() { 0x0018ffff, 0, LNKE, 0 }, Package() { 0x0018ffff, 1, LNKF, 0 }, Package() { 0x0018ffff, 2, LNKG, 0 }, Package() { 0x0018ffff, 3, LNKH, 0 },
                Package() { 0x0019ffff, 0, LNKA, 0 }, Package() { 0x0019ffff, 1, LNKB, 0 }, Package() { 0x0019ffff, 2, LNKC, 0 }, Package() { 0x0019ffff, 3, LNKD, 0 },
                Package() { 0x001affff, 0, LNKA, 0 }, Package() { 0x001affff, 1, LNKB, 0 }, Package() { 0x001affff, 2, LNKC, 0 }, Package() { 0x001affff, 3, LNKD, 0 },
                Package() { 0x001bffff, 0, LNKA, 0 }, Package() { 0x001bffff, 1, LNKB, 0 }, Package() { 0x001bffff, 2, LNKC, 0 }, Package() { 0x001bffff, 3, LNKD, 0 },
                Package() { 0x001cffff, 0, LNKA, 0 }, Package() { 0x001cffff, 1, LNKB, 0 }, Package() { 0x001cffff, 2, LNKC, 0 }, Package() { 0x001cffff, 3, LNKD, 0 },
                Package() { 0x001dffff, 0, LNKA, 0 }, Package() { 0x001dffff, 1, LNKB, 0 }, Package() { 0x001dffff, 2, LNKC, 0 }, Package() { 0x001dffff, 3, LNKD, 0 },
                Package() { 0x001effff, 0, LNKE, 0 }, Package() { 0x001effff, 1, LNKF, 0 }, Package() { 0x001effff, 2, LNKG, 0 }, Package() { 0x001effff, 3, LNKH, 0 },
                Package() { 0x001fffff, 0, LNKA, 0 }, Package() { 0x001fffff, 1, LNKB, 0 }, Package() { 0x001fffff, 2, LNKC, 0 }, Package() { 0x001fffff, 3, LNKD, 0 }
            })
            Name(PRTA, package() {
                Package() { 0x0000ffff, 0, GSIE, 0 }, Package() { 0x0000ffff, 1, GSIF, 0 }, Package() { 0x0000ffff, 2, GSIG, 0 }, Package() { 0x0000ffff, 3, GSIH, 0 },
                Package() { 0x0001ffff, 0, GSIF, 0 }, Package() { 0x0001ffff, 1, GSIG, 0 }, Package() { 0x0001ffff, 2, GSIH, 0 }, Package() { 0x0001ffff, 3, GSIE, 0 },
                Package() { 0x0002ffff, 0, GSIG, 0 }, Package() { 0x0002ffff, 1, GSIH, 0 }, Package() { 0x0002ffff, 2, GSIE, 0 }, Package() { 0x0002ffff, 3, GSIF, 0 },
                Package() { 0x0003ffff, 0, GSIH, 0 }, Package() { 0x0003ffff, 1, GSIE, 0 }, Package() { 0x0003ffff, 2, GSIF, 0 }, Package() { 0x0003ffff, 3, GSIG, 0 },
                Package() { 0x0004ffff, 0, GSIE, 0 }, Package() { 0x0004ffff, 1, GSIF, 0 }, Package() { 0x0004ffff, 2, GSIG, 0 }, Package() { 0x0004ffff, 3, GSIH, 0 },
                Package() { 0x0005ffff, 0, GSIF, 0 }, Package() { 0x0005ffff, 1, GSIG, 0 }, Package() { 0x0005ffff, 2, GSIH, 0 }, Package() { 0x0005ffff, 3, GSIE, 0 },
                Package() { 0x0006ffff, 0, GSIG, 0 }, Package() { 0x0006ffff, 1, GSIH, 0 }, Package() { 0x0006ffff, 2, GSIE, 0 }, Package() { 0x0006ffff, 3, GSIF, 0 },
                Package() { 0x0007ffff, 0, GSIH, 0 }, Package() { 0x0007ffff, 1, GSIE, 0 }, Package() { 0x0007ffff, 2, GSIF, 0 }, Package() { 0x0007ffff, 3, GSIG, 0 },
                Package() { 0x0008ffff, 0, GSIE, 0 }, Package() { 0x0008ffff, 1, GSIF, 0 }, Package() { 0x0008ffff, 2, GSIG, 0 }, Package() { 0x0008ffff, 3, GSIH, 0 },
                Package() { 0x0009ffff, 0, GSIF, 0 }, Package() { 0x0009ffff, 1, GSIG, 0 }, Package() { 0x0009ffff, 2, GSIH, 0 }, Package() { 0x0009ffff, 3, GSIE, 0 },
                Package() { 0x000affff, 0, GSIG, 0 }, Package() { 0x000affff, 1, GSIH, 0 }, Package() { 0x000affff, 2, GSIE, 0 }, Package() { 0x000affff, 3, GSIF, 0 },
                Package() { 0x000bffff, 0, GSIH, 0 }, Package() { 0x000bffff, 1, GSIE, 0 }, Package() { 0x000bffff, 2, GSIF, 0 }, Package() { 0x000bffff, 3, GSIG, 0 },
                Package() { 0x000cffff, 0, GSIE, 0 }, Package() { 0x000cffff, 1, GSIF, 0 }, Package() { 0x000cffff, 2, GSIG, 0 }, Package() { 0x000cffff, 3, GSIH, 0 },
                Package() { 0x000dffff, 0, GSIF, 0 }, Package() { 0x000dffff, 1, GSIG, 0 }, Package() { 0x000dffff, 2, GSIH, 0 }, Package() { 0x000dffff, 3, GSIE, 0 },
                Package() { 0x000effff, 0, GSIG, 0 }, Package() { 0x000effff, 1, GSIH, 0 }, Package() { 0x000effff, 2, GSIE, 0 }, Package() { 0x000effff, 3, GSIF, 0 },
                Package() { 0x000fffff, 0, GSIH, 0 }, Package() { 0x000fffff, 1, GSIE, 0 }, Package() { 0x000fffff, 2, GSIF, 0 }, Package() { 0x000fffff, 3, GSIG, 0 },
                Package() { 0x0010ffff, 0, GSIE, 0 }, Package() { 0x0010ffff, 1, GSIF, 0 }, Package() { 0x0010ffff, 2, GSIG, 0 }, Package() { 0x0010ffff, 3, GSIH, 0 },
                Package() { 0x0011ffff, 0, GSIF, 0 }, Package() { 0x0011ffff, 1, GSIG, 0 }, Package() { 0x0011ffff, 2, GSIH, 0 }, Package() { 0x0011ffff, 3, GSIE, 0 },
                Package() { 0x0012ffff, 0, GSIG, 0 }, Package() { 0x0012ffff, 1, GSIH, 0 }, Package() { 0x0012ffff, 2, GSIE, 0 }, Package() { 0x0012ffff, 3, GSIF, 0 },
                Package() { 0x0013ffff, 0, GSIH, 0 }, Package() { 0x0013ffff, 1, GSIE, 0 }, Package() { 0x0013ffff, 2, GSIF, 0 }, Package() { 0x0013ffff, 3, GSIG, 0 },
                Package() { 0x0014ffff, 0, GSIE, 0 }, Package() { 0x0014ffff, 1, GSIF, 0 }, Package() { 0x0014ffff, 2, GSIG, 0 }, Package() { 0x0014ffff, 3, GSIH, 0 },
                Package() { 0x0015ffff, 0, GSIF, 0 }, Package() { 0x0015ffff, 1, GSIG, 0 }, Package() { 0x0015ffff, 2, GSIH, 0 }, Package() { 0x0015ffff, 3, GSIE, 0 },
                Package() { 0x0016ffff, 0, GSIG, 0 }, Package() { 0x0016ffff, 1, GSIH, 0 }, Package() { 0x0016ffff, 2, GSIE, 0 }, Package() { 0x0016ffff, 3, GSIF, 0 },
                Package() { 0x0017ffff, 0, GSIH, 0 }, Package() { 0x0017ffff, 1, GSIE, 0 }, Package() { 0x0017ffff, 2, GSIF, 0 }, Package() { 0x0017ffff, 3, GSIG, 0 },
                Package() { 0x0018ffff, 0, GSIE, 0 }, Package() { 0x0018ffff, 1, GSIF, 0 }, Package() { 0x0018ffff, 2, GSIG, 0 }, Package() { 0x0018ffff, 3, GSIH, 0 },
                Package() { 0x0019ffff, 0, GSIA, 0 }, Package() { 0x0019ffff, 1, GSIB, 0 }, Package() { 0x0019ffff, 2, GSIC, 0 }, Package() { 0x0019ffff, 3, GSID, 0 },
                Package() { 0x001affff, 0, GSIA, 0 }, Package() { 0x001affff, 1, GSIB, 0 }, Package() { 0x001affff, 2, GSIC, 0 }, Package() { 0x001affff, 3, GSID, 0 },
                Package() { 0x001bffff, 0, GSIA, 0 }, Package() { 0x001bffff, 1, GSIB, 0 }, Package() { 0x001bffff, 2, GSIC, 0 }, Package() { 0x001bffff, 3, GSID, 0 },
                Package() { 0x001cffff, 0, GSIA, 0 }, Package() { 0x001cffff, 1, GSIB, 0 }, Package() { 0x001cffff, 2, GSIC, 0 }, Package() { 0x001cffff, 3, GSID, 0 },
                Package() { 0x001dffff, 0, GSIA, 0 }, Package() { 0x001dffff, 1, GSIB, 0 }, Package() { 0x001dffff, 2, GSIC, 0 }, Package() { 0x001dffff, 3, GSID, 0 },
                Package() { 0x001effff, 0, GSIE, 0 }, Package() { 0x001effff, 1, GSIF, 0 }, Package() { 0x001effff, 2, GSIG, 0 }, Package() { 0x001effff, 3, GSIH, 0 },
                Package() { 0x001fffff, 0, GSIA, 0 }, Package() { 0x001fffff, 1, GSIB, 0 }, Package() { 0x001fffff, 2, GSIC, 0 }, Package() { 0x001fffff, 3, GSID, 0 }
            })
            Method(_PRT, 0, NotSerialized) {
                If (LEqual(\PICF, Zero)) {
                    Return (PRTP)
                } Else {
                    Return (PRTA)
                }
            }
        }
        Field(PCI0.ISA.PIRQ, ByteAcc, NoLock, Preserve) {
            PRQA, 8,
            PRQB, 8,
            PRQC, 8,
            PRQD, 8,
            Offset(0x08),
            PRQE, 8,
            PRQF, 8,
            PRQG, 8,
            PRQH, 8
        }
        Method(IQST, 1, NotSerialized) {
            If (And(0x80, Arg0)) {
                Return (0x09)
            }
            Return (0x0B)
        }
        Method(IQCR, 1, Serialized) {
            Name(PRR0, ResourceTemplate() {
                Interrupt(, Level, ActiveHigh, Shared) { 0 }
            })
            CreateDWordField(PRR0, 0x05, PRRI)
            Store(And(Arg0, 0x0F), PRRI)
            Return (PRR0)
        }
        Device(LNKA) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 0) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 5, 10, 11 } }) Method(_STA, 0, NotSerialized) { Return (IQST(PRQA)) } Method(_DIS, 0, NotSerialized) { Or(PRQA, 0x80, PRQA) } Method(_CRS, 0, NotSerialized) { Return (IQCR(PRQA)) } Method(_SRS, 1, NotSerialized) { CreateDWordField(Arg0, 0x05, PRRI) Store(PRRI, PRQA) } }
        Device(LNKB) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 1) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 5, 10, 11 } }) Method(_STA, 0, NotSerialized) { Return (IQST(PRQB)) } Method(_DIS, 0, NotSerialized) { Or(PRQB, 0x80, PRQB) } Method(_CRS, 0, NotSerialized) { Return (IQCR(PRQB)) } Method(_SRS, 1, NotSerialized) { CreateDWordField(Arg0, 0x05, PRRI) Store(PRRI, PRQB) } }
        Device(LNKC) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 2) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 5, 10, 11 } }) Method(_STA, 0, NotSerialized) { Return (IQST(PRQC)) } Method(_DIS, 0, NotSerialized) { Or(PRQC, 0x80, PRQC) } Method(_CRS, 0, NotSerialized) { Return (IQCR(PRQC)) } Method(_SRS, 1, NotSerialized) { CreateDWordField(Arg0, 0x05, PRRI) Store(PRRI, PRQC) } }
        Device(LNKD) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 3) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 5, 10, 11 } }) Method(_STA, 0, NotSerialized) { Return (IQST(PRQD)) } Method(_DIS, 0, NotSerialized) { Or(PRQD, 0x80, PRQD) } Method(_CRS, 0, NotSerialized) { Return (IQCR(PRQD)) } Method(_SRS, 1, NotSerialized) { CreateDWordField(Arg0, 0x05, PRRI) Store(PRRI, PRQD) } }
        Device(LNKE) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 4) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 5, 10, 11 } }) Method(_STA, 0, NotSerialized) { Return (IQST(PRQE)) } Method(_DIS, 0, NotSerialized) { Or(PRQE, 0x80, PRQE) } Method(_CRS, 0, NotSerialized) { Return (IQCR(PRQE)) } Method(_SRS, 1, NotSerialized) { CreateDWordField(Arg0, 0x05, PRRI) Store(PRRI, PRQE) } }
        Device(LNKF) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 5) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 5, 10, 11 } }) Method(_STA, 0, NotSerialized) { Return (IQST(PRQF)) } Method(_DIS, 0, NotSerialized) { Or(PRQF, 0x80, PRQF) } Method(_CRS, 0, NotSerialized) { Return (IQCR(PRQF)) } Method(_SRS, 1, NotSerialized) { CreateDWordField(Arg0, 0x05, PRRI) Store(PRRI, PRQF) } }
        Device(LNKG) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 6) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 5, 10, 11 } }) Method(_STA, 0, NotSerialized) { Return (IQST(PRQG)) } Method(_DIS, 0, NotSerialized) { Or(PRQG, 0x80, PRQG) } Method(_CRS, 0, NotSerialized) { Return (IQCR(PRQG)) } Method(_SRS, 1, NotSerialized) { CreateDWordField(Arg0, 0x05, PRRI) Store(PRRI, PRQG) } }
        Device(LNKH) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 7) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 5, 10, 11 } }) Method(_STA, 0, NotSerialized) { Return (IQST(PRQH)) } Method(_DIS, 0, NotSerialized) { Or(PRQH, 0x80, PRQH) } Method(_CRS, 0, NotSerialized) { Return (IQCR(PRQH)) } Method(_SRS, 1, NotSerialized) { CreateDWordField(Arg0, 0x05, PRRI) Store(PRRI, PRQH) } }
        Device(GSIA) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 0) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x10 } }) Name(_CRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x10 } }) Method(_SRS, 1, NotSerialized) { } }
        Device(GSIB) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 0) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x11 } }) Name(_CRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x11 } }) Method(_SRS, 1, NotSerialized) { } }
        Device(GSIC) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 0) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x12 } }) Name(_CRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x12 } }) Method(_SRS, 1, NotSerialized) { } }
        Device(GSID) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 0) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x13 } }) Name(_CRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x13 } }) Method(_SRS, 1, NotSerialized) { } }
        Device(GSIE) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 0) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x14 } }) Name(_CRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x14 } }) Method(_SRS, 1, NotSerialized) { } }
        Device(GSIF) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 0) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x15 } }) Name(_CRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x15 } }) Method(_SRS, 1, NotSerialized) { } }
        Device(GSIG) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 0) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x16 } }) Name(_CRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x16 } }) Method(_SRS, 1, NotSerialized) { } }
        Device(GSIH) { Name(_HID, EISAID("PNP0C0F")) Name(_UID, 0) Name(_PRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x17 } }) Name(_CRS, ResourceTemplate() { Interrupt(, Level, ActiveHigh, Shared) { 0x17 } }) Method(_SRS, 1, NotSerialized) { } }
    }
Scope(\_SB) {
    External(NTFY, MethodObj)
    External(CPON, PkgObj)
    Method(CPMA, 1, NotSerialized) {
        Store(DerefOf(Index(CPON, Arg0)), Local0)
        Store(Buffer(8) {0x00, 0x08, 0x00, 0x00, 0x00, 0, 0, 0}, Local1)
        Store(Arg0, Index(Local1, 2))
        Store(Arg0, Index(Local1, 3))
        Store(Local0, Index(Local1, 4))
        Return (Local1)
    }
    Method(CPST, 1, NotSerialized) {
        Store(DerefOf(Index(CPON, Arg0)), Local0)
        If (Local0) {
            Return (0xF)
        } Else {
            Return (0x0)
        }
    }
    Method(CPEJ, 2, NotSerialized) {
        Sleep(200)
    }
    OperationRegion(PRST, SystemIO, 0x0CD8, 32)
    Field(PRST, ByteAcc, NoLock, Preserve) {
        PRS, 256
    }
    Method(PRSC, 0) {
        Store(PRS, Local5)
        Store(Zero, Local2)
        Store(Zero, Local0)
        While (LLess(Local0, SizeOf(CPON))) {
            Store(DerefOf(Index(CPON, Local0)), Local1)
            If (And(Local0, 0x07)) {
                ShiftRight(Local2, 1, Local2)
            } Else {
                Store(DerefOf(Index(Local5, ShiftRight(Local0, 3))), Local2)
            }
            Store(And(Local2, 1), Local3)
            If (LNotEqual(Local1, Local3)) {
                Store(Local3, Index(CPON, Local0))
                If (LEqual(Local3, 1)) {
                    NTFY(Local0, 1)
                } Else {
                    NTFY(Local0, 3)
                }
            }
            Increment(Local0)
        }
    }
    Device(PRES) {
        Name(_HID, EisaId("PNP0A06"))
        Name(_CRS, ResourceTemplate() {
            IO(Decode16, 0x0CD8, 0x0CD8, 0, 32)
        })
        Name(_STA, 0xB)
    }
}
    Scope(\_GPE) {
        Name(_HID, "ACPI0006")
        Method(_L00) {
        }
        Method(_L01) {
        }
        Method(_E02) {
            \_SB.PRSC()
        }
        Method(_L03) {
        }
        Method(_L04) {
        }
        Method(_L05) {
        }
        Method(_L06) {
        }
        Method(_L07) {
        }
        Method(_L08) {
        }
        Method(_L09) {
        }
        Method(_L0A) {
        }
        Method(_L0B) {
        }
        Method(_L0C) {
        }
        Method(_L0D) {
        }
        Method(_L0E) {
        }
        Method(_L0F) {
        }
    }
}
