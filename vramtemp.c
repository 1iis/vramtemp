#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pci/pci.h>
#include <errno.h>
#include <string.h>

#define PG_SZ sysconf(_SC_PAGE_SIZE)
#define MEM_PATH "/dev/mem"
#define VRAM_OFFSET 0x0000E2A8
#define NVIDIA_VENDOR 0x10de

int main(int argc, char **argv) {
    int list = 0;
    if (argc > 1 && (!strcmp(argv[1], "-l") || !strcmp(argv[1], "--list")))
        list = 1;

    if (geteuid() != 0) {
        fprintf(stderr, "run as root\n");
        return 1;
    }

    struct pci_access *pacc = pci_alloc();
    pci_init(pacc);
    pci_scan_bus(pacc);

    uint32_t temps[2] = {0};
    int count = 0;

    for (struct pci_dev *dev = pacc->devices; dev; dev = dev->next) {
        pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES);
        if (dev->vendor_id != NVIDIA_VENDOR) continue;

        uint32_t reg_addr = (dev->base_addr[0] & 0xFFFFFFFF) + VRAM_OFFSET;
        uint32_t base = reg_addr & ~(PG_SZ - 1);

        int fd = open(MEM_PATH, O_RDWR | O_SYNC);
        if (fd < 0) continue;

        void *map = mmap(0, PG_SZ, PROT_READ, MAP_SHARED, fd, base);
        if (map != MAP_FAILED) {
            uint32_t val = *(uint32_t *)((char *)map + (reg_addr - base));
            uint32_t t = (val & 0x00000fff) / 0x20;
            if (t > 0 && t < 0x7f && count < 2)
                temps[count++] = t;
            munmap(map, PG_SZ);
        }
        close(fd);
    }
    pci_cleanup(pacc);

    if (count == 0) {
        fprintf(stderr, "no temps\n");
        return 1;
    }

    if (list) {
        for (int i = 0; i < count; i++)
            printf("%d %u\n", i, temps[i]);
    } else {
        for (int i = 0; i < count; i++) {
            if (i) printf(" ");
            printf("%u", temps[i]);
        }
        printf("\n");
    }
    return 0;
}
