#define _GNU_SOURCE
#include <dlfcn.h>
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/syscall.h>
#include <ucontext.h>
#include <unistd.h>

//#define fprintf(...)
#define LOG(...) fprintf(stderr, __VA_ARGS__)
//#define LOG(...)

typedef long long (*mm_fn)(long long, long long);
mm_fn mm = (mm_fn)0x080487ec;

long long mm_wrap(long long a, long long b) {
  printf("mm: %lld %lld\n", a, b);
  return mm(a, b);
}

void undefined() {
  fprintf(stderr, "undefined function is called\n");
  abort();
}

int main(int argc, char* argv[]) {
  int i;
  int fd, len;
  char* elf;
  int entry, phoff, phnum;
  int* ph;

#if 0
  if (argc < 2)
    error("Usage: el <elf>");
  LOG("loading %s\n", argv[1]);
#endif
  fd = open("simple_hash", O_RDONLY);
  if (fd < 0)
    error("Usage: el <elf>");
  len = lseek(fd, 0, SEEK_END);
  elf = (char*)malloc(len);
  lseek(fd, 0, SEEK_SET);
  read(fd, elf, len);

  if (*(int*)elf != 0x464c457f)
    error("not elf");
  if (*(int*)(elf+16) != 0x30002)
    error("not i386 exec");

  entry = *(int*)(elf+24);
  phoff = *(int*)(elf+28);
  phnum = *(int*)(elf+42);
  LOG("%x %x %x\n", entry, phoff, phnum);

  ph = (int*)(elf + phoff);
  for (i = 0; i < phnum >> 16; i++) {
    int poff, paddr, pfsize, psize, pafsize, pflag /*, palign */;
    poff = ph[1];
    paddr = ph[2];
    pfsize = ph[4];
    psize = ph[5];
    pflag = ph[6];
    /*palign = ph[7];*/
    switch (ph[0]) {
    case 1: {
      int prot = 0;
      if (pflag & 1)
        prot |= PROT_EXEC;
      if (pflag & 2)
        prot |= PROT_WRITE;
      if (pflag & 4)
        prot |= PROT_READ;
      if (prot & PROT_EXEC) {
        prot |= PROT_WRITE;
      }

      psize += paddr & 0xfff;
      pfsize += paddr & 0xfff;
      poff -= paddr & 0xfff;
      paddr &= ~0xfff;
      pafsize = (pfsize + 0xfff) & ~0xfff;
      psize = (psize + 0xfff) & ~0xfff;
      LOG("PT_LOAD size=%d fsize=%d flag=%d addr=%x prot=%d poff=%d\n",
             psize, pafsize, pflag, paddr, prot, poff);
      if (mmap((void*)paddr, pafsize, prot, MAP_FILE|MAP_PRIVATE|MAP_FIXED,
               fd, poff) == MAP_FAILED) {
        error("mmap(file)");
      }
      if ((prot & PROT_WRITE)) {
        LOG("%p\n", (char*)paddr);
        for (; pfsize < pafsize; pfsize++) {
          char* p = (char*)paddr;
          p[pfsize] = 0;
        }
        if (pfsize != psize) {
          if (mmap((void*)(paddr + pfsize),
                   psize - pfsize, prot, MAP_ANON|MAP_PRIVATE,
                   -1, 0) == MAP_FAILED) {
            error("mmap(anon)");
          }
        }
      }

      break;
    }
    case 2: {
      char* dyn;
      char* dstr = NULL;
      char* dsym = NULL;
      char* rel = NULL;
      char* pltrel = NULL;
      int relsz, relent, pltrelsz = 0;
      int needed[999] = {}, *neededp = needed;
      puts("PT_DYNAMIC");
      dyn = elf + poff;
      for (;;) {
        short dtag = *(short*)dyn;
        int dval = *(int*)(dyn + 4);
        dyn += 8;
        if (dtag == 0)
          break;
        switch (dtag) {
        case 1: {  /* DT_NEEDED */
          *neededp++ = dval;
        }
        case 2: {
          pltrelsz = dval;
          LOG("pltrelsz: %d\n", pltrelsz);
          break;
        }
        case 5: {
          dstr = (char*)dval;
          LOG("dstr: %p %s\n", dstr, dstr+1);
          break;
        }
        case 6: {
          dsym = (char*)dval;
          LOG("dsym: %p\n", dsym);
          break;
        }
        case 17: {
          rel = (char*)dval;
          LOG("rel: %p\n", rel);
          break;
        }
        case 18: {
          relsz = dval;
          LOG("relsz: %d\n", relsz);
          break;
        }
        case 19: {
          relent = dval;
          LOG("relent: %d\n", relent);
          break;
        }
        case 20: {
          pltrel = (char*)dval;
          LOG("pltrel: %p\n", pltrel);
          break;
        }
        default:
          LOG("unknown DYN %d %d\n", dtag, dval);
        }
      }
      if (!dsym || !dstr) {
        error("no dsym or dstr");
      }

      for (neededp = needed; *neededp; neededp++) {
        LOG("needed: %s\n", dstr + *neededp);
        dlopen(dstr + *neededp, RTLD_NOW | RTLD_GLOBAL);
      }

      {
        int i, j;
        for (j = 0; j < 2; j++) {
          for (i = 0; i < relsz; rel += relent, i += relent) {
            int* addr = *(int**)rel;
            int info = *(int*)(rel + 4);
            int sym = info >> 8;
            int type = info & 0xff;

            int* ds = (int*)(dsym + 16 * sym);
            char* sname = dstr + *ds;
            void* val=0;
            int k;
#if 0
            for(k=0;T[k].n;k++){
              if(!strcmp(sname,T[k].n)){
                 val = T[k].f;
                 break;
              }
            }
#endif
            if(!val){
              if (!strcmp(sname,"stdout"))
                val = &stdout;
              else if (!strcmp(sname,"_Stdout"))
                val = stdout;
              else if (!strcmp(sname,"stderr"))
                val = &stderr;
              else if (!strcmp(sname,"_Stderr"))
                val = stderr;
              /*
              else if (!strcmp(sname, "__environ"))
                val = &environ;
              */
              else
                val = dlsym(RTLD_DEFAULT, sname);
            }

            LOG("%srel: %p %s(%d) %d => %p\n",
                   j ? "plt" : "", (void*)addr, sname, sym, type, val);

            if (!val) {
              val = (void*)&undefined;
            }

            switch (type) {
            case 1: {
              *addr += (int)val;
            }
            case 5: {
              if (val) {
                *addr = *(int*)val;
              } else {
                fprintf(stderr, "undefined: %s\n", sname);
                //abort();
              }
            }
            case 6: {
              if (val) {
                *addr = (int)val;
              } else {
                fprintf(stderr, "undefined data %s\n", sname);
              }
              break;
            }
            case 7: {
              if (val) {
                *addr = (int)val;
              } else {
                *addr = (int)&undefined;
              }
              break;
            }
            }
          }

          if ((int)pltrel != 17) {
            rel = pltrel;
          }
          relsz = pltrelsz;
        }
      }

      break;
    }
    default:
      fprintf(stderr, "unknown PT %d\n", ph[0]);
    }
    ph += 8;
  }

  //g_argc = argc-1;
  //g_argv = argv+1;

  fprintf(stderr, "start!: %s %x\n", argv[1], entry);

#define TEST(a, b) printf("%lld*%lld=%lld (%lld)\n", a, b, mm(a, b), a * b % 0x38d7ea4c68025LL)
  TEST(1LL, 2LL);
  TEST(0xdeaddeadULL, 0xdeaddeadULL);

  *(char**)(0x8048963+1) = (char*)mm_wrap - (0x8048963 + 5);

  char* str = (char*)0x80491a0;
  str[0] = 'X';
  str[1] = 'X';
  str[2] = 0;
  long long (*calc_hash)() = (long long (*)())0x08048927;
  printf("%lld\n", calc_hash());

  //run((void*)entry);
  return 0;
}
