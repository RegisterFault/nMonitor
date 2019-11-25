#ifndef MSR_H
#define MSR_H

#define THERM_MSR 0x19C
typedef union {  //msr 0x19C
        struct  {
                unsigned status:1;
                unsigned slog:1;
                unsigned prochot:1;
                unsigned prochotlog:1;
                unsigned crit:1;
                unsigned critlog:1;
                unsigned thresh1:1;
                unsigned thresh1l:1;
                unsigned thresh2:1;
                unsigned thresh2l:1;
                unsigned powl:1;
                unsigned powll:1;
                unsigned curl:1;
                unsigned curll:1;
                unsigned xdoml:1;
                unsigned xdomll:1;
                unsigned ro:7;
                unsigned pad1:4;
                unsigned res:4;
                unsigned valid:1;
                unsigned pad:32;
        }s __attribute__((packed));
        unsigned long w;
} THERM;


#define LIMITS_MSR 0x64F
typedef union { //msr 0x64F
        struct{
                unsigned prochot:1;     /* prochot line active */
                unsigned thermal:1;     /* thermal throttling */
                unsigned res1:2;        /* reserved */
                unsigned rsr:1;         /* residency state regulation */
                unsigned ratl:1;        /* running average thermal limit */
                unsigned vreg:1;        /* voltage regulator thermal alert */
                unsigned cur:1;         /* thermal design current exceeded */
                unsigned other:1;       /* other constraints */
                unsigned res2:1;        /* reserved */
                unsigned pl1:1;         /* power limit 1 exceeded */
                unsigned pl2:1;         /* power limit 2 exceeded */
                unsigned max_turbo:1;   /* proc at max all-core turbo */
                unsigned ttas:1;        /* turbo transition attenuation */
                unsigned res3:2;        /* reserved */
                unsigned prochot_log:1; /* the rest are log versions of the above */
                unsigned thermal_log:1;
                unsigned res4:2;
                unsigned rsr_log:1;
                unsigned ratl_log:1;
                unsigned vreg_log:1;
                unsigned cur_log:1;
                unsigned other_log:1;
                unsigned res5:1;
                unsigned pl1_log:1;
                unsigned pl2_log:1;
                unsigned max_turbo_log:1;
                unsigned ttas_log:1;
                unsigned long res6:34;
        }s __attribute__((packed));
        unsigned long w;
}LIMITS;


#define PWCTL_MSR 0x1FC
typedef union{
        struct {
                unsigned res1:1;
                unsigned c1e_enable;
                unsigned res2:17;
                unsigned r2h_disable:1;
                unsigned eeo_disable:1;
                unsigned long res3:43;
        }s __attribute__((packed));
        unsigned long w;
}PWCTL;

#define PPLC_MSR 0x65C //power limit control register
typedef union {
        struct {
                unsigned pl1_value:15; //long duration power limit. multiplied by MSR_RAPLPOWER_UNIT
                unsigned pl1_enable:1;
                unsigned pl1_clamping:1;
                unsigned pl1_window:7;
                unsigned res1:8;
                unsigned pl2_value:15;
                unsigned pl2_enable:1;
                unsigned pl2_clamping:1;
                unsigned pl2_window:7;
                unsigned res2:14;
                unsigned lock:1;

        }s __attribute__((packed));
        unsigned long w;
}PPLC;


#define RAPLU_MSR 0x606  //power unit register
typedef union {
        struct {
                unsigned power_units:4; // 1 / 2^power_units. default 3 = 1/8 watts
                unsigned res1:4;
                unsigned es_units:5;
                unsigned res2:2;
                unsigned time_units:4;
                unsigned long res3:44;
        }s __attribute__((packed));
        unsigned long w;
}RAPLU;

#define SPEC_MSR 0x48
typedef union {
        struct {
                unsigned ibrs:1;
                unsigned stibp:1;
                unsigned ssbd:1;
                unsigned long res1:61;
        }s __attribute__((packed));
        unsigned long w;
}SPEC;

#define ENER_MSR 0x64D
typedef union {
        struct {
                unsigned long energy:32; //multiply by RAPL_UNITS
                unsigned long res1:32;
        }s __attribute__((packed));
        unsigned long w;
}ENER;

#define CPU_PLANE 0
#define GPU_PLANE 1
#define CACHE_PLANE 2
#define AGENT_PLANE 3
#define ANALOG_PLANE 4
#define VOLT_READ  (0x8000001000000000L)
#define VOLT_WRITE (0x8000001100000000L)
#define VOLT_MSR 0x150 //undocumented voltage msr
typedef union {
        struct {
                unsigned long res1:21;
                signed volt:11;
                unsigned rw:1;  //read/write bit
                unsigned res2:3;
                unsigned one:1;
                unsigned res3:3;
                unsigned plane:4;
                unsigned long res4:20;
        }s __attribute__((packed));
        unsigned long w;
}VOLT;

#define NRGP_MSR 0x611
typedef union {
        struct {
                unsigned long energy:32; /* joules consumed total, 15.3 uJ */
                unsigned long res:32;
        }s __attribute__((packed));
        unsigned long w;
} NRGP;

#define NRG0_MSR 0x639
typedef union {
        struct {
                unsigned long energy:32; /* joules consumed total, 15.3 uJ */
                unsigned long res:32;
        }s __attribute__((packed));
        unsigned long w;
} NRG0;

#define AMD_RAPLU_MSR 0xC0010299
typedef union {
        struct {
                unsigned power_units:4;
                unsigned res1:4;
                unsigned energy_units:5;
                unsigned res2:3;
                unsigned time_units:4;
                unsigned long res3:44;
        }s __attribute__((packed));
        unsigned long w;
} AMD_RAPLU;

#define AMD_NRGP_MSR 0xC001029B
typedef union{
        struct {
                unsigned long energy:32;
                unsigned long res:32;
        }s __attribute__((packed));
        unsigned long w;
} AMD_NRGP;




void fail(void);
unsigned long rdmsr(unsigned int);
unsigned long wrmsr(unsigned int, unsigned long);

#endif
