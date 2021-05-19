//
// Created by 付聪 on 2017/6/21.
//

#ifndef EFANNA2E_UTIL_H
#define EFANNA2E_UTIL_H
#include <random>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <unistd.h>
#ifdef __APPLE__
#else
#include <malloc.h>
#endif
namespace efanna2e {

    static void GenRandom(std::mt19937 &rng, unsigned *addr, unsigned size, unsigned N) {
        for (unsigned i = 0; i < size; ++i) {
            addr[i] = rng() % (N - size);
        }
        std::sort(addr, addr + size);
        for (unsigned i = 1; i < size; ++i) {
            if (addr[i] <= addr[i - 1]) {
                addr[i] = addr[i - 1] + 1;
            }
        }
        unsigned off = rng() % N;
        for (unsigned i = 0; i < size; ++i) {
            addr[i] = (addr[i] + off) % N;
        }
    }

    inline float* data_align(float* data_ori, unsigned point_num, unsigned& dim){
      #ifdef __GNUC__
      #ifdef __AVX__
        #define DATA_ALIGN_FACTOR 8
      #else
      #ifdef __SSE2__
        #define DATA_ALIGN_FACTOR 4
      #else
        #define DATA_ALIGN_FACTOR 1
      #endif
      #endif
      #endif

      std::cout << "align with : "<<DATA_ALIGN_FACTOR << std::endl;
      float* data_new=0;
      unsigned new_dim = (dim + DATA_ALIGN_FACTOR - 1) / DATA_ALIGN_FACTOR * DATA_ALIGN_FACTOR;
      std::cout << "align to new dim: "<<new_dim << std::endl;
      #ifdef __APPLE__
        data_new = new float[new_dim * point_num];
      #else
        data_new = (float*)memalign(DATA_ALIGN_FACTOR * 4, point_num * new_dim * sizeof(float));
      #endif

      for(unsigned i=0; i<point_num; i++){
        memcpy(data_new + i * new_dim, data_ori + i * dim, dim * sizeof(float));
        memset(data_new + i * new_dim + dim, 0, (new_dim - dim) * sizeof(float));
      }
      dim = new_dim;
      #ifdef __APPLE__
        delete[] data_ori;
      #else
        free(data_ori);
      #endif
      return data_new;
    }

    /*
     * by johnpzh
     */
    class Mem {
    public:
        /**
         * reference: https://github.com/nmslib/hnswlib/blob/master/sift_1b.cpp
         * @return memory footprint in bytes.
         */
        static size_t getCurrentRSS() {
    #if defined(_WIN32)
            /* Windows -------------------------------------------------- */
        PROCESS_MEMORY_COUNTERS info;
        GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
        return (size_t)info.WorkingSetSize;

    #elif defined(__APPLE__) && defined(__MACH__)
            /* OSX ------------------------------------------------------ */
        struct mach_task_basic_info info;
        mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
        if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
            (task_info_t)&info, &infoCount) != KERN_SUCCESS)
            return (size_t)0L;      /* Can't access? */
        return (size_t)info.resident_size;

    #elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
            /* Linux ---------------------------------------------------- */
            long rss = 0L;
            FILE *fp = nullptr;
            if ((fp = fopen("/proc/self/statm", "r")) == nullptr)
                return (size_t) 0L;      /* Can't open? */
            if (fscanf(fp, "%*s%ld", &rss) != 1) {
                fclose(fp);
                return (size_t) 0L;      /* Can't read? */
            }
            fclose(fp);
            return (size_t) rss * (size_t) sysconf(_SC_PAGESIZE);

    #else
            /* AIX, BSD, Solaris, and Unknown OS ------------------------ */
        return (size_t)0L;          /* Unsupported. */
    #endif
        }
    };

}

#endif //EFANNA2E_UTIL_H
