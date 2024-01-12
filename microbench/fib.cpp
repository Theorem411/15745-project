// #ifdef _MSC_VER
// #include <intrin.h>
// #else
// #include <x86intrin.h>
// #endif

// #include <iostream>
// #include <cilk/cilk.h>

// using namespace std;
// inline unsigned long long readTSC() {
//     // _mm_lfence();  // optionally wait for earlier insns to retire before reading the clock
//     return __rdtsc();
//     // _mm_lfence();  // optionally block later instructions until rdtsc retires
// }

// long long fib(long long n) {
//   if (n < 2) return n;
//   int x, y;
//   x = cilk_spawn fib(n - 1);
//   y = fib(n - 2);
//   cilk_sync;
//   return x + y;
// }

// int main() {
//     auto start = readTSC(); 
//     auto res = fib(30);
//     auto end = readTSC();
//     auto elapse = end - start;
    
//     cout << "result = " << res << " with machine cycles: " << end - start << "\n";
//     return 0;
// }

/*
 * Copyright (c) 1994-2003 Massachusetts Institute of Technology
 * Copyright (c) 2003 Bradley C. Kuszmaul
 * Copyright (c) 2013 I-Ting Angelina Lee and Tao B. Schardl 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <cilk/cilk.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

unsigned long long todval (struct timeval *tp) {
    return tp->tv_sec * 1000 * 1000 + tp->tv_usec;
}

__attribute__((noinline))
void startup_cilk() {
  double g = 2.0;
  cilk_for(int i = 0; i<1000; i++) {
    g++;
  }
}

#ifdef SERIAL
#include <cilk/cilk_stub.h>
#endif

int fib(int n) {
    if (n < 2) { 
        return (n);

    } else {
        /* int x = 0; */
        /* int y = 0; */
      int x, y;
	x = cilk_spawn fib(n - 1);
        y = fib(n - 2);
        cilk_sync;

        return (x + y);
    }
}

int main(int argc, char *argv[]) {

  int n, result, round;
  round = 1;
  if (argc != 3) {
    fprintf(stderr, "Usage: fib [<cilk options>] <n> <number of round>\n");
    exit(1); 
  }

  // Start up cilk runtime
  startup_cilk();
  
  n = atoi(argv[1]);
  round = atoi(argv[2]);

  for(int r=0; r<round; r++) {

    struct timeval t1, t2;
    gettimeofday(&t1,0);
    result = fib(n);

    gettimeofday(&t2,0);
    unsigned long long runtime_ms = (todval(&t2)-todval(&t1))/1000;
    printf("PBBS-time: %f\n", runtime_ms/1000.0);
    fprintf(stderr, "Result: %d\n", result);

  }
  return 0;
}