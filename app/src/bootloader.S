# this path is relative to linkerfile 
# make file knows how to make a object file from an assembly file %.o: %.S in make file
.section .bootloader_section
    .incbin "../bootloader/bootloader.bin"
