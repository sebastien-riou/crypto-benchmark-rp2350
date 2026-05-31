#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#include "pico/sync.h"

void lean_benchmark(unsigned int ninfo, const char*info[], bool run_forever);


#ifdef __arm__
//****************************************************************************

volatile unsigned int *DWT_CYCCNT   = (volatile unsigned int *)0xE0001004;
volatile unsigned int *DWT_CONTROL  = (volatile unsigned int *)0xE0001000;
volatile unsigned int *DWT_LAR      = (volatile unsigned int *)0xE0001FB0;
volatile unsigned int *SCB_DHCSR    = (volatile unsigned int *)0xE000EDF0;
volatile unsigned int *SCB_DEMCR    = (volatile unsigned int *)0xE000EDFC;
volatile unsigned int *ITM_TER      = (volatile unsigned int *)0xE0000E00;
volatile unsigned int *ITM_TCR      = (volatile unsigned int *)0xE0000E80;

volatile unsigned int *DWT_COMP0   = (volatile unsigned int *)0xE0001020;
volatile unsigned int *DWT_MASK0   = (volatile unsigned int *)0xE0001024;
volatile unsigned int *DWT_FUNC0   = (volatile unsigned int *)0xE0001028;
#define DWT_FUNC0_CYCMATCH 			(1<<7)
#define DWT_FUNC0_GEN_WATCHPOINT 	4

uint64_t __attribute__ ((noinline)) LBMK_get_cpu_timestamp(){
	return *DWT_CYCCNT;
}
void __attribute__ ((noinline)) enable_cpu_counters(){
  *SCB_DEMCR |= 0x01000000;
  *DWT_LAR = 0xC5ACCE55; // enable access
  *DWT_CYCCNT = 0; // reset the counter
  *DWT_COMP0 = 0;
  *DWT_MASK0 = 0;
  *DWT_CONTROL |= 1 ; // enable the counter    
}
#else 
uint64_t __attribute__ ((noinline)) LBMK_get_cpu_timestamp(){
	uint32_t mcycle, mcycleh;

	// get mcycle and mcycleh
	asm volatile ("csrr %0, mcycle" : "=r"(mcycle));
	asm volatile ("csrr %0, mcycleh": "=r"(mcycleh));
    if(mcycle>=0xFFFFFFFF){
        mcycleh--;
        printf("WARNING: mcycle wrapped around while reading it\n");
    }
    return ((uint64_t)mcycleh << 32) | mcycle;
}

uint32_t __attribute__ ((noinline)) read_mcountinhibit(){
    uint32_t mcountinhibit;
    asm volatile ("csrr %0, mcountinhibit" : "=r"(mcountinhibit));
    return mcountinhibit;
}

void __attribute__ ((noinline)) enable_cpu_counters(){
    asm volatile ("csrw mcountinhibit, 0x0");//enable instruction and cycle counters
}
#endif

static volatile uint64_t heap_usage;
static volatile uint64_t heap_peak_usage;
void LBMK_init_heap_usage(){
    heap_usage = 0;
    heap_peak_usage = 0;
}
uint64_t LBMK_get_heap_usage(){
    if(heap_usage > heap_peak_usage) heap_peak_usage = heap_usage;
    return heap_peak_usage;
}
#include <stdlib.h>
#include <malloc.h>
extern void *REAL_FUNC(malloc)(size_t size);
extern void *REAL_FUNC(calloc)(size_t count, size_t size);
extern void *REAL_FUNC(realloc)(void *mem, size_t size);
extern void REAL_FUNC(free)(void *mem);
void *WRAPPER_FUNC(malloc)(size_t size) {
    void *rc = REAL_FUNC(malloc)(size);
    if(rc) heap_usage+=malloc_usable_size(rc);
    return rc;
}

void *WRAPPER_FUNC(calloc)(size_t count, size_t size) {
    void *rc = REAL_FUNC(calloc)(count, size);
    if(rc) heap_usage+=malloc_usable_size(rc);
    return rc;
}

void *WRAPPER_FUNC(realloc)(void *mem, size_t size) {
    void *rc = REAL_FUNC(realloc)(mem, size);
    if(rc) heap_usage+=malloc_usable_size(rc);
    return rc;
}

void WRAPPER_FUNC(free)(void *mem) {
    if(heap_usage > heap_peak_usage) heap_peak_usage = heap_usage;
    heap_usage -= malloc_usable_size(mem);
    REAL_FUNC(free)(mem);
}

void test_malloc(){
    while(1){
        LBMK_init_heap_usage();
        void*a[3];
        for(unsigned int i = 0;i<3;i++){
            a[i] = malloc(17);
        }
        for(unsigned int i = 0;i<3;i++){
            free(a[i]);
        }
        uint32_t heap = LBMK_get_heap_usage();
        printf("heap=%d\n",heap);

    }
}


#include "pico/stdio.h"
#include "pico/stdio/driver.h"
void com_tx(const void *const buf, unsigned int size){
    uint32_t status = save_and_disable_interrupts();
    enable_interrupts();
    stdio_usb.out_chars(buf,size);
    restore_interrupts(status);
}
void com_rx(void *const buf, unsigned int size){
	uint32_t status = save_and_disable_interrupts();
    enable_interrupts();
    uint8_t*buf8 = (uint8_t*)buf;
    for(unsigned int i=0;i<size;i++){
        buf8[i] = getchar();
    }
    restore_interrupts(status);
}
int LBMK_putchar(int ch);
static void stdio_lbmk_out_chars(const char *buffer, int length) {
    for(unsigned int i=0;i<length;i++){LBMK_putchar(buffer[i]);}
}
static int stdio_lbmk_in_chars(char *buffer, int length) {
    return stdio_usb.in_chars(buffer,length);
}
stdio_driver_t stdio_lbmk = {.out_chars = stdio_lbmk_out_chars,
                            .in_chars = stdio_lbmk_in_chars,
                            .crlf_enabled = 0};
int stdio_lbmk_init(void) {
    stdio_set_translate_crlf(&stdio_usb, false);
    stdio_set_driver_enabled(&stdio_lbmk, true);
    stdio_filter_driver(&stdio_lbmk);
    return 0;
}

void LBMK_init_leancom();
int main(){
    stdio_init_all();
    /*while(1){
        #ifdef __arm__
        printf("hello world ARM\n");
        #else
        printf("hello world RISCV\n");
        #endif
    }*/
    stdio_lbmk_init();
    LBMK_init_leancom();
    printf("crypto-benchmark-rp2350-rv\r\n");
    enable_cpu_counters();

    const uint32_t sys_clk = clock_get_hz(clk_sys);
    printf("SYS clock frequency = %lu MHz\r\n",sys_clk/1000000);
    char frequency_mhz[10] = {0};
    sprintf(frequency_mhz,"%lu",sys_clk/1000000);
  
    const char*hw_info[] = {
	  "hw_platform", "RP2350-RV",
	  "frequency_mhz", frequency_mhz
	};

    uint32_t status = save_and_disable_interrupts();
    lean_benchmark(sizeof(hw_info)/sizeof(char*),hw_info,0);
    restore_interrupts(status);
    while (true) {printf("crypto-benchmark-rp2350-rv done\r\n");sleep_ms(1000);}
}
