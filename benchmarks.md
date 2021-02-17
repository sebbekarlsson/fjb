# Benchmarks
> These benchmarks were generated at **2021-02-17 16:12:10.321734**





<details>
  <summary>Ternary operator</summary>

## What's being executed
> Code executed:
```tsx
function getNumber(x)
{
  switch (x) {
    case "hello": return 1; break;
    case "world": return 2; break;
    default: return 0; break;
  }
}


const y = 4;

const x = y > 3 ? y : z;

const g = x > 0 ? { name: "john" } : { name: "sarah" };

```
![graphs/VGVybmFyeSBvcGVyYXRvcg==.png](graphs/VGVybmFyeSBvcGVyYXRvcg==.png)


  
<details>
  <summary>FJB - time: 0:00:00.002936, output size: 293 bytes</summary>

#### Output size
> The size of the output is:
```
293 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/ternary/index.js  > ./dist.js
````
> finished in:
```
0:00:00.002936
```
</details>


  
<details>
  <summary>esbuild - time: 0:00:00.005020, output size: 65 bytes</summary>

#### Output size
> The size of the output is:
```
65 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/ternary/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.005020
```
</details>


  
<details>
  <summary>parcel - time: 0:00:00.839122, output size: 1265 bytes</summary>

#### Output size
> The size of the output is:
```
1265 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/ternary/index.js 
````
> finished in:
```
0:00:00.839122
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.725064, output size: 4616 bytes</summary>

#### Output size
> The size of the output is:
```
4616 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/ternary/index.js 
````
> finished in:
```
0:00:01.725064
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         2174.359
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Destruct</summary>

## What's being executed
> Code executed:
```tsx
function xyz({
  firstname = "john",
  lastname
}) {
  console.log(firstname, lastname);
}

xyz("john", "doe");

```
![graphs/RGVzdHJ1Y3Q=.png](graphs/RGVzdHJ1Y3Q=.png)


  
<details>
  <summary>FJB - time: 0:00:00.008993, output size: 201 bytes</summary>

#### Output size
> The size of the output is:
```
201 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/destruct/index.js  > ./dist.js
````
> finished in:
```
0:00:00.008993
```
</details>


  
<details>
  <summary>esbuild - time: 0:00:00.015647, output size: 89 bytes</summary>

#### Output size
> The size of the output is:
```
89 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/destruct/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.015647
```
</details>


  
<details>
  <summary>parcel - time: 0:00:00.811038, output size: 1228 bytes</summary>

#### Output size
> The size of the output is:
```
1228 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/destruct/index.js 
````
> finished in:
```
0:00:00.811038
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.529942, output size: 4571 bytes</summary>

#### Output size
> The size of the output is:
```
4571 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/destruct/index.js 
````
> finished in:
```
0:00:01.529942
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         900.067
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Importing ES6 modules</summary>

## What's being executed
> Code executed:
```tsx
import { subtract } from './helpers/math';


const x = 10;
const y = 5;
const r = subtract(x, y);
console.log(r);

```
![graphs/SW1wb3J0aW5nIEVTNiBtb2R1bGVz.png](graphs/SW1wb3J0aW5nIEVTNiBtb2R1bGVz.png)


  
<details>
  <summary>FJB - time: 0:00:00.010341, output size: 461 bytes</summary>

#### Output size
> The size of the output is:
```
461 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/es6/index.js  > ./dist.js
````
> finished in:
```
0:00:00.010341
```
</details>


  
<details>
  <summary>esbuild - time: 0:00:00.015962, output size: 76 bytes</summary>

#### Output size
> The size of the output is:
```
76 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/es6/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.015962
```
</details>


  
<details>
  <summary>parcel - time: 0:00:00.835978, output size: 1488 bytes</summary>

#### Output size
> The size of the output is:
```
1488 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/es6/index.js 
````
> finished in:
```
0:00:00.835978
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.530906, output size: 5517 bytes</summary>

#### Output size
> The size of the output is:
```
5517 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/es6/index.js 
````
> finished in:
```
0:00:01.530906
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         1699.364
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Importing aliased ES6 modules</summary>

## What's being executed
> Code executed:
```tsx
import { multiply as mul } from './utils';


const r = mul(5, 5);

console.log(r);

```
![graphs/SW1wb3J0aW5nIGFsaWFzZWQgRVM2IG1vZHVsZXM=.png](graphs/SW1wb3J0aW5nIGFsaWFzZWQgRVM2IG1vZHVsZXM=.png)


  
<details>
  <summary>FJB - time: 0:00:00.005759, output size: 420 bytes</summary>

#### Output size
> The size of the output is:
```
420 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/alias_imports/index.js  > ./dist.js
````
> finished in:
```
0:00:00.005759
```
</details>


  
<details>
  <summary>esbuild - time: 0:00:00.009692, output size: 67 bytes</summary>

#### Output size
> The size of the output is:
```
67 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/alias_imports/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.009692
```
</details>


  
<details>
  <summary>parcel - time: 0:00:00.832119, output size: 1383 bytes</summary>

#### Output size
> The size of the output is:
```
1383 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/alias_imports/index.js 
````
> finished in:
```
0:00:00.832119
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.522416, output size: 5270 bytes</summary>

#### Output size
> The size of the output is:
```
5270 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/alias_imports/index.js 
````
> finished in:
```
0:00:01.522416
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         1000.045
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Importing CSS</summary>

## What's being executed
> Code executed:
```tsx
import style from './style.css';


const cssEl = document.createElement('style')
cssEl.innerText = style;
document.head.appendChild(cssEl);

```
![graphs/SW1wb3J0aW5nIENTUw==.png](graphs/SW1wb3J0aW5nIENTUw==.png)


  
<details>
  <summary>FJB - time: 0:00:00.009180, output size: 718 bytes</summary>

#### Output size
> The size of the output is:
```
718 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/css_import/index.js  > ./dist.js
````
> finished in:
```
0:00:00.009180
```
</details>


  
<details>
  <summary>esbuild - time: 0:00:00.014363, output size: 102 bytes</summary>

#### Output size
> The size of the output is:
```
102 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/css_import/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.014363
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.555001, output size: 18537 bytes</summary>

#### Output size
> The size of the output is:
```
18537 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/css_import/index.js 
````
> finished in:
```
0:00:01.555001
```
</details>


  
<details>
  <summary>parcel - time: 0:00:08.527165, output size: 1383 bytes</summary>

#### Output size
> The size of the output is:
```
1383 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/css_import/index.js 
````
> finished in:
```
0:00:08.527165
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         2898.475
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Importing JSON</summary>

## What's being executed
> Code executed:
```tsx
import people from './people.json';

console.log(people[0].first_name);

```
![graphs/SW1wb3J0aW5nIEpTT04=.png](graphs/SW1wb3J0aW5nIEpTT04=.png)


  
<details>
  <summary>FJB - time: 0:00:00.003861, output size: 1432 bytes</summary>

#### Output size
> The size of the output is:
```
1432 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/json_import/index.js  > ./dist.js
````
> finished in:
```
0:00:00.003861
```
</details>


  
<details>
  <summary>esbuild - time: 0:00:00.005518, output size: 519 bytes</summary>

#### Output size
> The size of the output is:
```
519 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/json_import/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.005518
```
</details>


  
<details>
  <summary>parcel - time: 0:00:00.836184, output size: 1822 bytes</summary>

#### Output size
> The size of the output is:
```
1822 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/json_import/index.js 
````
> finished in:
```
0:00:00.836184
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.539229, output size: 5846 bytes</summary>

#### Output size
> The size of the output is:
```
5846 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/json_import/index.js 
````
> finished in:
```
0:00:01.539229
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         1095.813
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Importing `ceil` from lodash, with tree shaking.</summary>

## What's being executed
> Code executed:
```tsx
import { ceil } from 'lodash';


console.log(ceil(25.443));

```
![graphs/SW1wb3J0aW5nIGBjZWlsYCBmcm9tIGxvZGFzaCwgd2l0aCB0cmVlIHNoYWtpbmcu.png](graphs/SW1wb3J0aW5nIGBjZWlsYCBmcm9tIGxvZGFzaCwgd2l0aCB0cmVlIHNoYWtpbmcu.png)


  
<details>
  <summary>esbuild - time: 0:00:00.021705, output size: 73253 bytes</summary>

#### Output size
> The size of the output is:
```
73253 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/with_lodash/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.021705
```
</details>


  
<details>
  <summary>FJB - time: 0:00:00.527021, output size: 2195 bytes</summary>

#### Output size
> The size of the output is:
```
2195 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/with_lodash/index.js  > ./dist.js
````
> finished in:
```
0:00:00.527021
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.515809, output size: 549615 bytes</summary>

#### Output size
> The size of the output is:
```
549615 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/with_lodash/index.js 
````
> finished in:
```
0:00:01.515809
```
</details>


  
<details>
  <summary>parcel - time: 0:00:03.293814, output size: 94714 bytes</summary>

#### Output size
> The size of the output is:
```
94714 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/with_lodash/index.js 
````
> finished in:
```
0:00:03.293814
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         1112.983
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Importing `Vue` from vue, with tree shaking.</summary>

## What's being executed
> Code executed:
```tsx
const Vue = require('vue/dist/vue.js');

var app = new Vue({
  el: '#root',
  data: {
    message: "hello"
  }
});

console.log(app);

```
![graphs/SW1wb3J0aW5nIGBWdWVgIGZyb20gdnVlLCB3aXRoIHRyZWUgc2hha2luZy4=.png](graphs/SW1wb3J0aW5nIGBWdWVgIGZyb20gdnVlLCB3aXRoIHRyZWUgc2hha2luZy4=.png)


  
<details>
  <summary>esbuild - time: 0:00:00.024213, output size: 118580 bytes</summary>

#### Output size
> The size of the output is:
```
118580 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/with_vue/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.024213
```
</details>


  
<details>
  <summary>FJB - time: 0:00:00.705102, output size: 205486 bytes</summary>

#### Output size
> The size of the output is:
```
205486 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/with_vue/index.js  > ./dist.js
````
> finished in:
```
0:00:00.705102
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.532779, output size: 363610 bytes</summary>

#### Output size
> The size of the output is:
```
363610 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/with_vue/index.js 
````
> finished in:
```
0:00:01.532779
```
</details>


  
<details>
  <summary>parcel - time: 0:00:02.850415, output size: 117866 bytes</summary>

#### Output size
> The size of the output is:
```
117866 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/with_vue/index.js 
````
> finished in:
```
0:00:02.850415
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         1500.102
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Importing `hljs` from hightlight.min.js, with tree shaking.</summary>

## What's being executed
> Code executed:
```tsx
import { hljs } from './highlight.min.js';

console.log(hljs);

```
![graphs/SW1wb3J0aW5nIGBobGpzYCBmcm9tIGhpZ2h0bGlnaHQubWluLmpzLCB3aXRoIHRyZWUgc2hha2luZy4=.png](graphs/SW1wb3J0aW5nIGBobGpzYCBmcm9tIGhpZ2h0bGlnaHQubWluLmpzLCB3aXRoIHRyZWUgc2hha2luZy4=.png)


  
<details>
  <summary>esbuild - time: 0:00:00.012002, output size: 117937 bytes</summary>

#### Output size
> The size of the output is:
```
117937 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/highlightjs/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.012002
```
</details>


  
<details>
  <summary>FJB - time: 0:00:00.174168, output size: 20034 bytes</summary>

#### Output size
> The size of the output is:
```
20034 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/highlightjs/index.js  > ./dist.js
````
> finished in:
```
0:00:00.174168
```
</details>


  
<details>
  <summary>poi - time: 0:00:02.234241, output size: 191460 bytes</summary>

#### Output size
> The size of the output is:
```
191460 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/highlightjs/index.js 
````
> finished in:
```
0:00:02.234241
```
</details>


  
<details>
  <summary>parcel - time: 0:00:02.608790, output size: 127443 bytes</summary>

#### Output size
> The size of the output is:
```
127443 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/highlightjs/index.js 
````
> finished in:
```
0:00:02.608790
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         3195.537
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Require `jquery`</summary>

## What's being executed
> Code executed:
```tsx
const $ = require('./jquery');

console.log($);

```
![graphs/UmVxdWlyZSBganF1ZXJ5YA==.png](graphs/UmVxdWlyZSBganF1ZXJ5YA==.png)


  
<details>
  <summary>esbuild - time: 0:00:00.016137, output size: 89981 bytes</summary>

#### Output size
> The size of the output is:
```
89981 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/with_jquery/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.016137
```
</details>


  
<details>
  <summary>FJB - time: 0:00:00.270476, output size: 99646 bytes</summary>

#### Output size
> The size of the output is:
```
99646 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/with_jquery/index.js  > ./dist.js
````
> finished in:
```
0:00:00.270476
```
</details>


  
<details>
  <summary>poi - time: 0:00:02.681494, output size: 142970 bytes</summary>

#### Output size
> The size of the output is:
```
142970 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/with_jquery/index.js 
````
> finished in:
```
0:00:02.681494
```
</details>


  
<details>
  <summary>parcel - time: 0:00:02.741223, output size: 90592 bytes</summary>

#### Output size
> The size of the output is:
```
90592 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/with_jquery/index.js 
````
> finished in:
```
0:00:02.741223
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         1800.080
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Importing `react` from react</summary>

## What's being executed
> Code executed:
```tsx
import React from 'react';

```
![graphs/SW1wb3J0aW5nIGByZWFjdGAgZnJvbSByZWFjdA==.png](graphs/SW1wb3J0aW5nIGByZWFjdGAgZnJvbSByZWFjdA==.png)


  
<details>
  <summary>esbuild - time: 0:00:00.008477, output size: 25345 bytes</summary>

#### Output size
> The size of the output is:
```
25345 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/react_import/index.jsx  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.008477
```
</details>


  
<details>
  <summary>FJB - time: 0:00:00.078862, output size: 43736 bytes</summary>

#### Output size
> The size of the output is:
```
43736 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/react_import/index.jsx  > ./dist.js
````
> finished in:
```
0:00:00.078862
```
</details>


  
<details>
  <summary>parcel - time: 0:00:01.035511, output size: 8574 bytes</summary>

#### Output size
> The size of the output is:
```
8574 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/react_import/index.jsx 
````
> finished in:
```
0:00:01.035511
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.528789, output size: 80555 bytes</summary>

#### Output size
> The size of the output is:
```
80555 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/react_import/index.jsx 
````
> finished in:
```
0:00:01.528789
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         1000.035
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>React project with JSX</summary>

## What's being executed
> Code executed:
```tsx
import React, { Component } from "react";
import { Card } from "./card";
import style from "./style.css";
import { render } from "react-dom";

// unfortunately, need to manually append style tag right now
const styleTag = document.createElement("style");
styleTag.innerText = style;
document.head.appendChild(styleTag);

const SUBTITLE = "It's so simple!";
const appStyle = {
  width: "100%",
  height: "100%",
  display: "flex",
  justifyContent: "center",
  alignItems: "center",
};

const App = () => {
  return (
    <div style={appStyle}>
      <div>
        <Card text={SUBTITLE} />
      </div>
    </div>
  );
};

render(<App />, document.getElementById("root"));

```
![graphs/UmVhY3QgcHJvamVjdCB3aXRoIEpTWA==.png](graphs/UmVhY3QgcHJvamVjdCB3aXRoIEpTWA==.png)


  
<details>
  <summary>esbuild - time: 0:00:00.040949, output size: 289816 bytes</summary>

#### Output size
> The size of the output is:
```
289816 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/with_react/index.jsx  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.040949
```
</details>


  
<details>
  <summary>FJB - time: 0:00:01.737488, output size: 592826 bytes</summary>

#### Output size
> The size of the output is:
```
592826 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/with_react/index.jsx  > ./dist.js
````
> finished in:
```
0:00:01.737488
```
</details>


  
<details>
  <summary>poi - time: 0:00:02.209399, output size: 1031155 bytes</summary>

#### Output size
> The size of the output is:
```
1031155 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/with_react/index.jsx 
````
> finished in:
```
0:00:02.209399
```
</details>


  
<details>
  <summary>parcel - time: 0:00:03.306254, output size: 135732 bytes</summary>

#### Output size
> The size of the output is:
```
135732 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/with_react/index.jsx 
````
> finished in:
```
0:00:03.306254
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         989.467
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Importing cookie banner (dobarkod/cookie-banner)</summary>

## What's being executed
> Code executed:
```tsx
import { CookieBanner } from './cookiebanner.min.js';


console.log(CookieBanner);

```
![graphs/SW1wb3J0aW5nIGNvb2tpZSBiYW5uZXIgKGRvYmFya29kL2Nvb2tpZS1iYW5uZXIp.png](graphs/SW1wb3J0aW5nIGNvb2tpZSBiYW5uZXIgKGRvYmFya29kL2Nvb2tpZS1iYW5uZXIp.png)


  
<details>
  <summary>esbuild - time: 0:00:00.005138, output size: 10306 bytes</summary>

#### Output size
> The size of the output is:
```
10306 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/cookie_banner/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.005138
```
</details>


  
<details>
  <summary>FJB - time: 0:00:00.044594, output size: 10395 bytes</summary>

#### Output size
> The size of the output is:
```
10395 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/cookie_banner/index.js  > ./dist.js
````
> finished in:
```
0:00:00.044594
```
</details>


  
<details>
  <summary>parcel - time: 0:00:00.998914, output size: 10911 bytes</summary>

#### Output size
> The size of the output is:
```
10911 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/cookie_banner/index.js 
````
> finished in:
```
0:00:00.998914
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.530849, output size: 18462 bytes</summary>

#### Output size
> The size of the output is:
```
18462 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/cookie_banner/index.js 
````
> finished in:
```
0:00:01.530849
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         1000.017
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Simple regex</summary>

## What's being executed
> Code executed:
```tsx
function validateEmail(email) {
  var re = /\S+@\S+\.\S+/;
  return re.test(email);
}

```
![graphs/U2ltcGxlIHJlZ2V4.png](graphs/U2ltcGxlIHJlZ2V4.png)


  
<details>
  <summary>FJB - time: 0:00:00.008582, output size: 179 bytes</summary>

#### Output size
> The size of the output is:
```
179 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/regex/index.js  > ./dist.js
````
> finished in:
```
0:00:00.008582
```
</details>


  
<details>
  <summary>esbuild - time: 0:00:00.014734, output size: 12 bytes</summary>

#### Output size
> The size of the output is:
```
12 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/regex/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.014734
```
</details>


  
<details>
  <summary>parcel - time: 0:00:00.818695, output size: 1171 bytes</summary>

#### Output size
> The size of the output is:
```
1171 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/regex/index.js 
````
> finished in:
```
0:00:00.818695
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.539393, output size: 4408 bytes</summary>

#### Output size
> The size of the output is:
```
4408 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/regex/index.js 
````
> finished in:
```
0:00:01.539393
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         1000.000
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Array access</summary>

## What's being executed
> Code executed:
```tsx
const a = function (o) {
  ("readystatechange" == o.type && "complete" != n.readyState) ||
    (("load" == o.type ? e : n)[r](c + o.type, l, !1),
    !i && (i = !0) && t.call(e, o.type || o));
};

const b = [1, 2, 3, 4][0];

const c = ["hello", "world", "john", "doe"];

const d = c[b];

```
![graphs/QXJyYXkgYWNjZXNz.png](graphs/QXJyYXkgYWNjZXNz.png)


  
<details>
  <summary>FJB - time: 0:00:00.010873, output size: 353 bytes</summary>

#### Output size
> The size of the output is:
```
353 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/array_access/index.js  > ./dist.js
````
> finished in:
```
0:00:00.010873
```
</details>


  
<details>
  <summary>esbuild - time: 0:00:00.016177, output size: 71 bytes</summary>

#### Output size
> The size of the output is:
```
71 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/array_access/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.016177
```
</details>


  
<details>
  <summary>parcel - time: 0:00:00.826149, output size: 1318 bytes</summary>

#### Output size
> The size of the output is:
```
1318 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/array_access/index.js 
````
> finished in:
```
0:00:00.826149
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.518658, output size: 4653 bytes</summary>

#### Output size
> The size of the output is:
```
4653 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/array_access/index.js 
````
> finished in:
```
0:00:01.518658
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         4451.437
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Simple bundle with zero imports</summary>

## What's being executed
> Code executed:
```tsx
export function hello() {
  console.log(123);
}

```
![graphs/U2ltcGxlIGJ1bmRsZSB3aXRoIHplcm8gaW1wb3J0cw==.png](graphs/U2ltcGxlIGJ1bmRsZSB3aXRoIHplcm8gaW1wb3J0cw==.png)


  
<details>
  <summary>FJB - time: 0:00:00.003748, output size: 196 bytes</summary>

#### Output size
> The size of the output is:
```
196 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/noimports/index.js  > ./dist.js
````
> finished in:
```
0:00:00.003748
```
</details>


  
<details>
  <summary>esbuild - time: 0:00:00.005273, output size: 42 bytes</summary>

#### Output size
> The size of the output is:
```
42 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/noimports/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.005273
```
</details>


  
<details>
  <summary>parcel - time: 0:00:00.801817, output size: 1242 bytes</summary>

#### Output size
> The size of the output is:
```
1242 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/noimports/index.js 
````
> finished in:
```
0:00:00.801817
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.539294, output size: 4604 bytes</summary>

#### Output size
> The size of the output is:
```
4604 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/noimports/index.js 
````
> finished in:
```
0:00:01.539294
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         1000.044
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>

<details>
  <summary>Escaped strings</summary>

## What's being executed
> Code executed:
```tsx
const pattern = "*(?:'((?:\\\\.|[^\\\\'])*)'|\"((?:\\\\.|[^\\\\\"])*)\"|(";

```
![graphs/RXNjYXBlZCBzdHJpbmdz.png](graphs/RXNjYXBlZCBzdHJpbmdz.png)


  
<details>
  <summary>FJB - time: 0:00:00.009057, output size: 177 bytes</summary>

#### Output size
> The size of the output is:
```
177 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/fjb.out /home/ianertson/workspace/fjb/tests/fjb-samples/strings/index.js  > ./dist.js
````
> finished in:
```
0:00:00.009057
```
</details>


  
<details>
  <summary>esbuild - time: 0:00:00.013190, output size: 12 bytes</summary>

#### Output size
> The size of the output is:
```
12 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/esbuild /home/ianertson/workspace/fjb/tests/fjb-samples/strings/index.js  --bundle --define:'process.env.NODE_ENV="dev"' --minify --outfile=./dist.js
````
> finished in:
```
0:00:00.013190
```
</details>


  
<details>
  <summary>parcel - time: 0:00:00.857004, output size: 1193 bytes</summary>

#### Output size
> The size of the output is:
```
1193 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/parcel build /home/ianertson/workspace/fjb/tests/fjb-samples/strings/index.js 
````
> finished in:
```
0:00:00.857004
```
</details>


  
<details>
  <summary>poi - time: 0:00:01.541503, output size: 4414 bytes</summary>

#### Output size
> The size of the output is:
```
4414 bytes
```
#### Time
> The command:
```
/home/ianertson/workspace/fjb/benchmark/node_modules/.bin/poi /home/ianertson/workspace/fjb/tests/fjb-samples/strings/index.js 
````
> finished in:
```
0:00:01.541503
```
</details>



#### Executed on
```
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          16
On-line CPU(s) list:             0-15
Thread(s) per core:              2
Core(s) per socket:              8
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i9-9980HK CPU @ 2.40GHz
Stepping:                        13
CPU MHz:                         1000.025
CPU max MHz:                     5000.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4800.00
Virtualization:                  VT-x
L1d cache:                       256 KiB
L1i cache:                       256 KiB
L2 cache:                        2 MiB
L3 cache:                        16 MiB
NUMA node0 CPU(s):               0-15
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Mitigation; TSX disabled
Vulnerability Tsx async abort:   Mitigation; TSX disabled
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities

```
</details>
