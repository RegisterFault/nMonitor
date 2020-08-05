#ifndef MSR_H
#define MSR_H
#include <stdint.h>

#define IA32_THERM_STATUS_MSR 0x0000019C
typedef union {
        struct {
                uint8_t status:1;
                uint8_t slog:1;
                uint8_t prochot:1;
                uint8_t prochotlog:1;
                uint8_t crit:1;
                uint8_t critlog:1;
                uint8_t thresh1:1;
                uint8_t thresh1l:1;
                uint8_t thresh2:1;
                uint8_t thresh2l:1;
                uint8_t powl:1;
                uint8_t powll:1;
                uint8_t curl:1;
                uint8_t curll:1;
                uint8_t xdoml:1;
                uint8_t xdomll:1;
                uint8_t ro:7;
                uint8_t pad1:4;
                uint8_t res:4;
                uint8_t valid:1;
                uint32_t pad:32;
        } s __attribute__((packed));
        uint64_t w;
} IA32_THERM_STATUS;

#define CORE_PERF_LIMIT_REASONS_MSR 0x0000064F
typedef union {
        struct {
                uint8_t prochot:1;     /* prochot line active */
                uint8_t thermal:1;     /* thermal throttling */
                uint8_t res1:2;        /* reserved */
                uint8_t rsr:1;         /* residency state regulation */
                uint8_t ratl:1;        /* running average thermal limit */
                uint8_t vreg:1;        /* voltage regulator thermal alert */
                uint8_t cur:1;         /* thermal design current exceeded */
                uint8_t other:1;       /* other constraints */
                uint8_t res2:1;        /* reserved */
                uint8_t pl1:1;         /* power limit 1 exceeded */
                uint8_t pl2:1;         /* power limit 2 exceeded */
                uint8_t max_turbo:1;   /* proc at max all-core turbo */
                uint8_t ttas:1;        /* turbo transition attenuation */
                uint8_t res3:2;        /* reserved */
                uint8_t prochot_log:1; /* the rest are log versions of the above */
                uint8_t thermal_log:1;
                uint8_t res4:2;
                uint8_t rsr_log:1;
                uint8_t ratl_log:1;
                uint8_t vreg_log:1;
                uint8_t cur_log:1;
                uint8_t other_log:1;
                uint8_t res5:1;
                uint8_t pl1_log:1;
                uint8_t pl2_log:1;
                uint8_t max_turbo_log:1;
                uint8_t ttas_log:1;
                uint64_t res6:34;
        } s __attribute__((packed));
        uint64_t w;
} CORE_PERF_LIMIT_REASONS;

#define POWER_CTL_MSR 0x000001FC
typedef union {
        struct {
                uint8_t res1:1;
                uint8_t c1e_enable:1;
                uint32_t res2:18-2;
                uint8_t r2h_disable:1;
                uint8_t eeo_disable:1;
                uint64_t res3:63-12;
        } s __attribute__((packed));
        uint64_t w;
} POWER_CTL;

#define PLATFORM_POWER_LIMIT_MSR 0x0000065C //power limit control register
typedef union {
        struct {
                uint16_t pl1_value:15; //long duration power limit. multiplied by MSR_RAPLPOWER_UNIT
                uint8_t pl1_enable:1;
                uint8_t pl1_clamping:1;
                uint8_t pl1_window:7;
                uint8_t res1:8;
                uint16_t pl2_value:15;
                uint8_t pl2_enable:1;
                uint8_t pl2_clamping:1;
                uint8_t pl2_window:7;
                uint16_t res2:14;
                uint8_t lock:1;
        } s __attribute__((packed));
        uint64_t w;
} PLATFORM_POWER_LIMIT;

#define PKG_POWER_LIMIT_MSR 0x00000610
typedef union {
        struct {
                uint16_t limit:14-0;
                uint8_t enable:15-14;
                uint8_t clamp:16-15;
                uint16_t timew:23-17;
                uint16_t res1:30-24;
                uint8_t lock:31-30;
                uint32_t res2:63-32;
        } s __attribute__((packed));
        uint64_t w;
} PKG_POWER_LIMIT;

#define RAPL_POWER_UNIT_MSR 0x00000606  //power unit register
typedef union {
        struct {
                uint8_t power_units:4; // 1 / 2^power_units. default 3 = 1/8 watts
                uint8_t res1:4;
                uint8_t es_units:5;
                uint8_t res2:2;
                uint8_t time_units:4;
                uint64_t res3:44;
        } s __attribute__((packed));
        uint64_t w;
} RAPL_POWER_UNIT;

#define SPEC_MSR 0x00000048
typedef union {
        struct {
                uint8_t ibrs:1;
                uint8_t stibp:1;
                uint8_t ssbd:1;
                uint64_t res1:61;
        } s __attribute__((packed));
        uint64_t w;
} SPEC;

#define PLATFORM_ENERGY_COUNTER_MSR 0x0000064D
typedef union {
        struct {
                uint32_t energy:32; //multiply by RAPL_UNITS
                uint32_t res1:32;
        } s __attribute__((packed));
        uint64_t w;
} PLATFORM_ENERGY_COUNTER;

#define IA32_PM_ENABLE_MSR 0x00000770
typedef union {
        struct {
                uint64_t enable:1;
                uint64_t res:63;
        } s __attribute__((packed));
        uint64_t w;
} IA32_PM_ENABLE;

#define IA32_HWP_REQUEST_PKG_MSR 0x00000772
typedef union {
        struct {
                uint8_t min_perf:8;
                uint8_t max_perf:8;
                uint8_t des_perf:8;
                uint8_t nrg_pref:8;
                uint16_t act_win:10;
                uint32_t res:22;
        } s __attribute__((packed));
        uint64_t w;
} IA32_HWP_REQUEST_PKG;

#define IA32_HWP_REQUEST_MSR 0x00000774
typedef union {
        struct {
                uint8_t min_perf:8;
                uint8_t max_perf:8;
                uint8_t des_perf:8;
                uint8_t nrg_pref:8;
                uint16_t act_win:10;
                uint8_t pkg_ctl:1;
                uint16_t res1:16;
                uint8_t act_win_valid:1;
                uint8_t epp_valid:1;
                uint8_t des_valid:1;
                uint8_t max_valid:1;
                uint8_t min_valid:1;
        } s __attribute__((packed));
        uint64_t w;
} IA32_HWP_REQUEST;

#define PKG_HDC_CTL_MSR 0x00000DB0
typedef union {
        struct {
                uint8_t enable:1;
                uint64_t res:63;
        } s __attribute__((packed));
        uint64_t w;
} PKG_HDC_CTL;

#define PM_CTL1_MSR 0x00000652
typedef union {
        struct {
                uint8_t allow:1;
                uint64_t res:63;
        } s __attribute__((packed));
        uint64_t w;
} PM_CTL1;

#define CPU_PLANE 0
#define GPU_PLANE 1
#define CACHE_PLANE 2
#define AGENT_PLANE 3
#define ANALOG_PLANE 4
#define VOLT_READ  (0x8000001000000000L)
#define VOLT_WRITE (0x8000001100000000L)
#define VOLT_MSR 0x00000150 //undocumented voltage msr
typedef union {
        struct {
                uint64_t res1:21;
                int16_t volt:11;
                uint8_t rw:1;  //read/write bit
                uint8_t res2:3;
                uint8_t one:1;
                uint8_t res3:3;
                uint8_t plane:4;
                uint32_t res4:20;
        } s __attribute__((packed));
        uint64_t w;
} VOLT;

#define NRGP_MSR 0x00000611
typedef union {
        struct {
                uint64_t energy:32; /* joules consumed total, 15.3 uJ */
                uint64_t res:32;
        } s __attribute__((packed));
        uint64_t w;
} NRGP;

#define PP0_NRG_MSR 0x00000639
typedef union {
        struct {
                uint64_t energy:32;
                uint64_t res:32;
        } s __attribute__((packed));
        uint64_t w;
} PP0_NRG;

#define PP1_NRG_MSR 0x00000641
typedef union {
        struct {
                uint64_t energy:32;
                uint64_t res:32;
        } s __attribute__((packed));
        uint64_t w;
} PP1_NRG;

#define DRAM_NRG_MSR 0x00000619
typedef union {
        struct {
                uint64_t energy:32;
                uint64_t res:32;
        } s __attribute__((packed));
        uint64_t w;
} DRAM_NRG;

#define NRG0_MSR 0x639
typedef union {
        struct {
                uint64_t energy:32; /* joules consumed total, 15.3 uJ */
                uint64_t res:32;
        } s __attribute__((packed));
        uint64_t w;
} NRG0;

#define AMD_RAPLU_MSR 0xC0010299
typedef union {
        struct {
                uint8_t power_units:4;
                uint8_t res1:4;
                uint8_t energy_units:5;
                uint8_t res2:3;
                uint8_t time_units:4;
                uint64_t res3:44;
        } s __attribute__((packed));
        uint64_t w;
} AMD_RAPLU;

#define AMD_NRGP_MSR 0xC001029B
typedef union{
        struct {
                uint32_t energy:32;
                uint32_t res:32;
        } s __attribute__((packed));
        uint64_t w;
} AMD_NRGP;

#define AMDPS0_MSR 0xC0010064
#define AMDPS1_MSR 0xC0010065
#define AMDPS2_MSR 0xC0010066
#define AMDPS3_MSR 0xC0010067
#define AMDPS4_MSR 0xC0010068
#define AMDPS5_MSR 0xC0010069
#define AMDPS6_MSR 0xC001006A
#define AMDPS7_MSR 0xC001006B
typedef union {
        struct {
                uint8_t cfid:8; /* core frequency multiplier */
                uint8_t cdid:6; /* core frequency divisor */
                uint8_t cvid:8; /* core vid */
                uint8_t ciddv:8; /* core Idd value */
                uint8_t ciddd:2; /* core Idd divisor */
                uint64_t res:31;
                uint8_t en:1; /* pstate enable */
        } s __attribute__ ((packed));
        uint64_t w;
} AMD_PS;

int have_msr(void);
uint64_t rdmsr_cpu(uint32_t, unsigned int);
uint64_t rdmsr(uint32_t);
uint64_t wrmsr_cpu(uint32_t, uint64_t, unsigned int);
uint64_t wrmsr_all(uint32_t, uint64_t);
uint64_t wrmsr(uint32_t, uint64_t);

#endif
