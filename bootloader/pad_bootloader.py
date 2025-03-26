BOOTLOADER_SIZE = 0x8000
BOOTLOADER_FILE = "bootloader.bin"

# read as binary file 
with open(BOOTLOADER_FILE, "rb") as f:
    # read the hole file
    raw_file = f.read()
    f.close()
    

# linking would fail if larger than 32 KiB
# rawfile is of type bytes 
bytes_to_pad = BOOTLOADER_SIZE - len(raw_file)
padding = bytes([0xff for _ in range(bytes_to_pad)])

with open("bootloader.bin", "wb") as f:
    f.write(raw_file + padding)
    f.close()
    
# run python pad_bootloader.py
# check ls -lB 

# -rwxr-xr-x 1 user user   608 Mar 25 22:06 bootloader.bin
# -rw-r--r-- 1 user user 28678 Mar 25 22:07 bootloader.map
# -rw-r--r-- 1 user user 32768 Mar 25 22:15 bootloader_padded.bin
# drwxr-xr-x 2 user user  4096 Mar 25 21:48 inc
# -rw-r--r-- 1 user user  2770 Mar 25 22:05 linkerscript.ld
# -rw-r--r-- 1 user user   456 Mar 25 22:13 pad_bootloader.py

# 32768 Bytes 32KiB bootloader_padded.bin