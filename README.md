# SyncFault
SyncFault
# The main idea is to inject fault at location where coherency is maintained. Let consider the following Chip multiprocessors:- 

Consider a System with 4 cores, each maintains it own private L1 Cache and L2 Cache off chip.

Reading from CMP$IM: a Pin-Based on the fly multicores cache simulator.

Memory system simulators fall into two main categories : Trace-driven or execution driven. 

Execution driven cache simulators rely on functional/performance models to execute an application binaries. The memory addresses generated by the functional/ performance model are fed, in real time, to a cache simulator modeled within the functional/performance model.

CMP$IM gather detailed cache performance statistics as well as the total amount of sharing between different threads of a multi-threaded application.

# Mutithreading support by Pintool.

 To distinguish  between  the  different  threads  of  the  application, Pin  assigns  each  thread  with  a  unique  ID  which  is  different from the native process ID assigned by the operating system.Pin assigns the first thread, i.e. the main thread, with thread ID 0   and   each   additional   new   thread   is   assigned   the   next sequential  ID,  i.e.  1,  2,  3,  and  so  on.  Thus,  when  conducting studies   with   a   four-threaded   workload,   Pin   distinguishes
between threads by assigning the main thread with thread ID 0, and the three remaining threads with thread IDs 1, 2 and 3. It   is   the   responsibility   of   the   Pin   tool   to   distinguish instrumentation based on different thread IDs.

Need to model multicores cache memories. 

On Approximate load, if miss in the TLB it will first go to main memory and do the mapping what if we donot do this right away at this point. How do we know that the instruction is approximate at the MMU level. 

Is the check costly? What special bit to make this instruction visible as approximable.

May be when an instruction is approximable some flag is set in the register to notify that this is an approximable instruction. 


What type of memory characterization to gather insight in this work?

