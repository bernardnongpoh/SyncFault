
/*! @file
 *  This is an example of the PIN tool that demonstrates some basic PIN APIs 
 *  and could serve as the starting point for developing your first PIN tool
 */

#include "pin.H"
#include <iostream>
#include <fstream>
#include "instlib.H"
#include <iomanip>
/* ================================================================== */
// Global variables 
/* ================================================================== */
using namespace INSTLIB;
UINT64 insCount = 0;        //number of dynamically executed instructions
UINT64 bblCount = 0;        //number of dynamically executed basic blocks
UINT64 threadCount = 0;     //total number of threads, including main thread
std::string startMarker;
std::string endMarker;
BOOL outOfROIPhase = false;




std::ostream * out = &cerr;
LOCALVAR std::ofstream outs;
FILTER filter;
/* ===================================================================== */
// Command line switches
/* ===================================================================== */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,  "pintool",
    "o", "", "specify file name for SyncFault output");


KNOB<std::string>   KnobStartMarker(KNOB_MODE_WRITEONCE,       "pintool",
                                    "startMarker", "", "start marker function name");
KNOB<std::string>   KnobEndMarker(KNOB_MODE_WRITEONCE,       "pintool",
                                  "endMarker", "", "end marker function name");


/* ===================================================================== */
// Utilities
/* ===================================================================== */

/*!
 *  Print out help message.
 */
INT32 Usage()
{
    cerr << "This tools is to study fault in Coherence communication between cores " << endl;
            

    cerr << KNOB_BASE::StringKnobSummary() << endl;

    return -1;
}


VOID FunStartInstrumentation() {
    outOfROIPhase = false;


}

VOID FunEndInstrumentation() {
    outOfROIPhase = true;

}


VOID EmitXMM(THREADID threadid, UINT32 regno, PIN_REGISTER* xmm)
{
    //if (!Emit(threadid))
      //  return;
    outs << "\t\t\tXMM" << dec << regno << " := " << setfill('0') << hex;
    outs.unsetf(ios::showbase);
    for(int i=0;i<16;i++) {
        if (i==4 || i==8 || i==12)
            outs << "_";
        outs << setw(2) << (int)xmm->byte[15-i]; // msb on the left as in registers
    }
    outs  << setfill(' ') << endl;
    outs.setf(ios::showbase);
    //Flush();
}

VOID AddXMMEmit(INS ins, IPOINT point, REG xmm_dst)
{
    INS_InsertCall(ins, point, AFUNPTR(EmitXMM), IARG_THREAD_ID,
                   IARG_UINT32, xmm_dst - REG_XMM0,
                   IARG_REG_CONST_REFERENCE, xmm_dst,
                   IARG_END);
}


VOID ShowN(UINT32 n, VOID *ea)
{
    outs.unsetf(ios::showbase);
    // Print out the bytes in "big endian even though they are in memory little endian.
    // This is most natural for 8B and 16B quantities that show up most frequently.
    // The address pointed to
    outs << std::setfill('0');
    UINT8 b[512];
    UINT8* x;
    if (n > 512)
        x = new UINT8[n];
    else
        x = b;
    PIN_SafeCopy(x,static_cast<UINT8*>(ea),n);
    for (UINT32 i = 0; i < n; i++)
    {
        outs << std::setw(2) <<  static_cast<UINT32>(x[n-i-1]);
        if (((reinterpret_cast<ADDRINT>(ea)+n-i-1)&0x3)==0 && i<n-1)
            outs << "_";
    }
    outs << std::setfill(' ');
    outs.setf(ios::showbase);
    if (n>512)
        delete [] x;
}



VOID EmitRead(THREADID threadid,ADDRINT ip,VOID * ea, UINT32 size)
{

    outs<<std::hex<<ip <<":"<<ea<<"\n";
    outs << "                                 Read ";


    switch(size)
    {
      case 0:
        outs << "0 repeat count" << endl;
        break;

      case 1:
        {
            UINT8 x;
            PIN_SafeCopy(&x,static_cast<UINT8*>(ea),1);
            outs << static_cast<UINT32>(x) << " = *(UINT8*)" << ea << endl;
        }
        break;

      case 2:
        {
            UINT16 x;
            PIN_SafeCopy(&x,static_cast<UINT16*>(ea),2);
            outs << x << " = *(UINT16*)" << ea << endl;
        }
        break;

      case 4:
        {
            UINT32 x;
            PIN_SafeCopy(&x,static_cast<UINT32*>(ea),4);
            outs <<std::hex<<x << " = *(UINT32*)" <<std::hex<<ea << endl;
        }
        break;

      case 8:
        {
            UINT64 x;
            PIN_SafeCopy(&x,static_cast<UINT64*>(ea),8);
            outs <<std::dec<<x << " = *(UINT64*)" <<std::hex<< ea << endl;
        }
        break;

      default:
        ShowN(size,ea);
        outs << " = *(UINT" << dec << size * 8 << hex << ")" << ea << endl;
        break;
    }

    //Flush();
}



string FormatAddress(ADDRINT address, RTN rtn)
{
    string s = StringFromAddrint(address);

    if (RTN_Valid(rtn))
    {
        IMG img = SEC_Img(RTN_Sec(rtn));
        s+= " ";
        if (IMG_Valid(img))
        {
            s += IMG_Name(img) + ":";
        }

        s += RTN_Name(rtn);

        ADDRINT delta = address - RTN_Address(rtn);
        if (delta != 0)
        {
            s += "+" + hexstr(delta, 4);
        }
    }
    bool KnobLines=true;// Hardcoded for now copy from debugtrace.cpp
    if (KnobLines)
    {
        INT32 line;
        string file;

        PIN_GetSourceLocation(address, NULL, &line, &file);

        if (file != "")
        {
            s += " (" + file + ":" + decstr(line) + ")";
        }
    }
    return s;
}




/* ===================================================================== */
// Instrumentation callbacks
/* ===================================================================== */

/*!
 * Insert call to the CountBbl() analysis routine before every basic block 
 * of the trace.
 * This function is called every time a new trace is encountered.
 * @param[in]   trace    trace to be instrumented
 * @param[in]   v        value specified by the tool in the TRACE_AddInstrumentFunction
 *                       function call
 */
VOID InstructionTrace(TRACE trace, INS ins)
{


 ADDRINT addr = INS_Address(ins);
    ASSERTX(addr);

     string traceString = "";
    string astring = FormatAddress(INS_Address(ins), TRACE_Rtn(trace));
    

     for (INT32 length = astring.length(); length < 30; length++)
    {
        traceString += " ";
    }
    traceString = astring + traceString;

    traceString += " " + INS_Disassemble(ins);

    for (INT32 length = traceString.length(); length < 80; length++)
    {
        traceString += " ";
    }

    // Get the filena,e

   // *out<<traceString<<"\n";

     if (INS_HasMemoryRead2(ins) && INS_IsStandardMemop(ins))
    {
        INS_InsertPredicatedCall(ins, IPOINT_BEFORE, AFUNPTR(EmitRead), IARG_THREAD_ID,IARG_INST_PTR, IARG_MEMORYREAD2_EA, IARG_MEMORYREAD_SIZE, IARG_END);
    }

    if (INS_IsMemoryRead(ins) && !INS_IsPrefetch(ins) && INS_IsStandardMemop(ins))
    {
        INS_InsertPredicatedCall(ins, IPOINT_BEFORE, AFUNPTR(EmitRead), IARG_THREAD_ID, IARG_INST_PTR,IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE, IARG_END);
    }

}



VOID Trace(TRACE trace, VOID *v)
{
    if (!filter.SelectTrace(trace))
        return;

    if(outOfROIPhase)
        return;
    // Visit every basic block in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        // Insert a call to CountBbl() before every basic bloc, passing the number of instructions
        
  

         for(INS ins=BBL_InsHead(bbl);INS_Valid(ins);ins =  INS_Next(ins)){
                  // Create an Instruction trace here

            InstructionTrace(trace,ins);
         }     




        
    }
}

/*!
 * Increase counter of threads in the application.
 * This function is called for every thread created by the application when it is
 * about to start running (including the root thread).
 * @param[in]   threadIndex     ID assigned by PIN to the new thread
 * @param[in]   ctxt            initial register state for the new thread
 * @param[in]   flags           thread creation flags (OS specific)
 * @param[in]   v               value specified by the tool in the 
 *                              PIN_AddThreadStartFunction function call
 */
VOID ThreadStart(THREADID threadIndex, CONTEXT *ctxt, INT32 flags, VOID *v)
{
    threadCount++;
}

/*!
 * Print out analysis results.
 * This function is called when the application exits.
 * @param[in]   code            exit code of the application
 * @param[in]   v               value specified by the tool in the 
 *                              PIN_AddFiniFunction function call
 */

// This code is for tracking and marking region of interest 
VOID Routine(RTN rtn, VOID *v)
{
   RTN_Open(rtn);

    const string* rtn_name = new string(RTN_Name(rtn));
    if(startMarker.compare("") != 0)
    {
        cout <<"Start ROI\n";
        if(startMarker.compare(*rtn_name) == 0) {
            RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR) FunStartInstrumentation,
                           IARG_FUNCARG_ENTRYPOINT_VALUE, 0, IARG_END);
        }
    }
    if(endMarker.compare("") != 0)
    {
        cout<<"end ROI\n";
        if(endMarker.compare(*rtn_name) == 0) {
            RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR) FunEndInstrumentation,
                           IARG_FUNCARG_ENTRYPOINT_VALUE, 0, IARG_END);
        }
    }

    RTN_Close(rtn);

    }




VOID Fini(INT32 code, VOID *v)
{
    *out <<  "===============================================" << endl;
    *out <<  "MyPinTool analysis results: " << endl;
    *out <<  "Number of instructions: " << insCount  << endl;
    *out <<  "Number of basic blocks: " << bblCount  << endl;
    *out <<  "Number of threads: " << threadCount  << endl;
    *out <<  "===============================================" << endl;
}

/*!
 * The main procedure of the tool.
 * This function is called when the application image is loaded but not yet started.
 * @param[in]   argc            total number of elements in the argv array
 * @param[in]   argv            array of command line arguments, 
 *                              including pin -t <toolname> -- ...
 */
int main(int argc, char *argv[])
{
    PIN_InitSymbols();
    // Initialize PIN library. Print help message if -h(elp) is specified
    // in the command line or the command line is invalid 
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }

    startMarker = KnobStartMarker;
    endMarker = KnobEndMarker;

    if(startMarker.compare("") != 0)
    {
        outOfROIPhase = true;
    }
    else
    {
        outOfROIPhase = false;
    }

    outs.open("out.log");
    
    string fileName = KnobOutputFile.Value();

    if (!fileName.empty()) { out = new std::ofstream(fileName.c_str());}

    
    
        // Register function to be called to instrument traces
        TRACE_AddInstrumentFunction(Trace, 0);
         filter.Activate();

        // Register function to be called for every thread before it starts running
        PIN_AddThreadStartFunction(ThreadStart, 0);
        RTN_AddInstrumentFunction(Routine, 0);
        // Register function to be called when the application exits
        PIN_AddFiniFunction(Fini, 0);
    
    
    cerr <<  "===============================================" << endl;
    cerr <<  "This application is instrumented by MyPinTool" << endl;
    if (!KnobOutputFile.Value().empty()) 
    {
        cerr << "See file " << KnobOutputFile.Value() << " for analysis results" << endl;
    }
    cerr <<  "===============================================" << endl;

    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
