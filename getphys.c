#include "ptedit_header.h"

#include <x86linux/helper.h>

int main(int argc, char *argv[]) {
  if (argc != 5) {
    fprintf(stderr, "usage: %s DEC_PID HEX_VIRT_START DEC_SIZE SHOW_PTE\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }
  int pid = atoi(argv[1]);
  uintptr_t virt_start = strtoul(argv[2], NULL, 16);
  size_t size = strtoul(argv[3], NULL, 10);
  int show_pte = atoi(argv[4]);

  LOG_INIT();
  log_enable(LOG_DEBUG);

  log_verify_error(ptedit_init());

  size_t pagesize = ptedit_get_pagesize();
  log_verify(!(virt_start % pagesize));
  log_verify(!(size % pagesize));

  size_t nr_pages = size / pagesize;
  for (size_t i = 0; i < nr_pages; ++i) {
    uintptr_t virt_addr = virt_start + pagesize * i;

    ptedit_entry_t vm = ptedit_resolve(address_cast(virt_addr), pid);
    log_verify(vm.pgd);

    size_t pfn = ptedit_get_pfn(vm.pte);
    loff_t phys_addr = pfn * pagesize;
    log(LOG_INFO, "[(i: %lu) virt: 0x%lx] (pfn: 0x%lx) phys: 0x%lx", i,
        virt_addr, pfn, phys_addr);
    if (show_pte) {
      log_verify(!bitset_test(&vm.pte, PTEDIT_PAGE_BIT_SOFTW4));
      log_verify(!bitset_test(&vm.pte, PTEDIT_PAGE_BIT_PKEY_BIT0));
      log_verify(!bitset_test(&vm.pte, PTEDIT_PAGE_BIT_PKEY_BIT1));
      log_verify(!bitset_test(&vm.pte, PTEDIT_PAGE_BIT_PKEY_BIT2));
      log_verify(!bitset_test(&vm.pte, PTEDIT_PAGE_BIT_PKEY_BIT3));
      ptedit_print_entry_t(vm);
    }
  }

  ptedit_cleanup();

  return EXIT_SUCCESS;
}
