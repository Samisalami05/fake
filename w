[4mcurs_initscr[24m(3X)           Library calls           [4mcurs_initscr[24m(3X)

[1mNAME[0m
     [1minitscr[22m, [1mnewterm[22m, [1mendwin[22m, [1misendwin[22m, [1mset_term[22m, [1mdelscreen [22m- ini‐
     tialize, manipulate, or tear down [4mcurses[24m terminal interface

[1mSYNOPSIS[0m
     [1m#include <curses.h>[0m

     [1mWINDOW * initscr(void);[0m
     [1mint endwin(void);[0m

     [1mbool isendwin(void);[0m

     [1mSCREEN * newterm(const char * [4m[22mtype[24m[1m, FILE * [4m[22moutf[24m[1m, FILE * [4m[22minf[24m[1m);[0m
     [1mSCREEN * set_term(SCREEN * [4m[22mnew[24m[1m);[0m
     [1mvoid delscreen(SCREEN * [4m[22msp[24m[1m);[0m

[1mDESCRIPTION[0m
   [1minitscr[0m
     [1minitscr  [22mdetermines  the terminal type and initializes the li‐
     brary's [4mSCREEN[24m, [4mWINDOW[24m, and other data structures.  It is nor‐
     mally the first [4mcurses[24m function call a program performs.  How‐
     ever, an application with unusual needs  might  employ  a  few
     other [4mcurses[24m functions beforehand:

     •   [1mslk_init[22m(3X) to set up soft-label keys;

     •   [1mfilter[22m(3X)  if  the  program  is  designed to operate in a
         process pipeline;

     •   [1mripoffline[22m(3X) to reserve up to  five  lines  at  the  top
         and/or bottom of the screen from management by [1mstdscr[22m, the
         standard [4mcurses[24m window; and

     •   [1muse_env[22m(3X)  and/or [1muse_tioctl[22m(3X) to configure use of the
         process environment and operating system's  terminal  dri‐
         ver,  respectively, when determining the dimensions of the
         terminal display.

     Further, a [4mcurses[24m program might call [1mnewterm [22mprior to  or  in‐
     stead  of  [1minitscr  [22min  two specialized cases described in its
     subsection below.

     [1minitscr [22mcauses the first [1mrefresh[22m(3X) call to clear the screen.
     If errors occur, [1minitscr [22mwrites an appropriate diagnostic mes‐
     sage to the standard error stream and exits; otherwise, it re‐
     turns a pointer to [1mstdscr[22m.

   [1mnewterm[0m
     An application that manages  multiple  terminals  should  call
     [1mnewterm   [22monce  for  each  such  device  [4minstead[24m  of  [1minitscr[22m.
     [1mnewterm[22m's arguments are

     •   the [4mtype[24m of the associated terminal, or a null pointer  to
         use the [4mTERM[24m environment variable;

     •   an output stream [4moutf[24m connected to the terminal; and

     •   an input stream [4minf[24m connected to the terminal.

     [1mnewterm  [22mreturns  a  variable of pointer-to-[4mSCREEN[24m type, which
     should be saved for later use with [1mset_term [22mand [1mdelscreen[22m.

     [1mnewterm [22mpasses the file descriptor of the output stream to the
     [4mterminfo[24m function [1msetupterm[22m(3X), which returns a pointer to  a
     [4mTERMINAL[24m  structure  that  [1mnewterm [22mstores in the [4mSCREEN[24m it re‐
     turns to the application.

     An application that needs to inspect a terminal  type's  capa‐
     bilities,  so  that  it can continue to run in a line-oriented
     mode if the terminal type does not  support  capabilities  the
     application  demands,  would also use [1mnewterm[22m.  If at most one
     terminal connection is needed, the  programmer  could  perform
     such  a  capability test, decide the mode in which to operate,
     then call [1mdelscreen [22mon the pointer returned  by  [1mnewterm[22m,  and
     proceed with either [1minitscr [22mor a non-[4mcurses[24m interface.

   [1mendwin[0m
     The program must also call [1mendwin [22mfor each terminal being used
     before  exiting  from  [4mcurses[24m.  If [1mnewterm [22mis called more than
     once for the same terminal, the  first  terminal  referred  to
     must be the last one for which [1mendwin [22mis called.

     A  program should always call [1mendwin [22mbefore exiting the appli‐
     cation or temporarily suspending [4mcurses[24m's  management  of  the
     terminal.  [1mendwin[22m:

     •   (if [1mstart_color[22m(3X) has been called) resets the terminal's
         foreground  and background colors to correspond with those
         of color pair 0 (the default pair),

     •   moves the cursor to the  lower  left-hand  corner  of  the
         screen,

     •   (if  [1mstart_color[22m(3X) has been called) restores the default
         color pair,

     •   clears the line,

     •   sets the cursor to normal visibility (see [1mcurs_set[22m(3X)),

     •   if applicable,  stops  cursor-addressing  mode  using  the
         [1mexit_ca_mode [22m([1mrmcup[22m) terminal capability, and

     •   restores terminal modes (see [1mreset_shell_mode[22m(3X)).

     Calling  [1mrefresh[22m(3X) or [1mdoupdate[22m(3X) after a temporary suspen‐
     sion causes [4mcurses[24m to resume managing the terminal.

   [1misendwin[0m
     [1misendwin [22mreturns [1mTRUE [22mif  [1mwrefresh[22m(3X)  has  not  been  called
     since the most recent [1mendwin [22mcall, and [1mFALSE [22motherwise.

   [1mset_term[0m
     [1mset_term [22mre-orients the [4mcurses[24m library's operations to another
     terminal when the application has arranged to manage more than
     one  with  [1mnewterm[22m.   [1mset_term [22mexpects a [4mSCREEN[24m pointer previ‐
     ously returned by [1mnewterm [22mas an argument, and returns the pre‐
     vious one.  [1mset_term [22mis the only standard [4mcurses[24m API  function
     that  manipulates  [4mSCREEN[24m pointers; all others affect only the
     current terminal (but see [1mcurs_sp_funcs[22m(3X)).

   [1mdelscreen[0m
     [1mdelscreen  [22mfrees  the  storage  backing  the  supplied  [4mSCREEN[0m
     pointer argument.  [1mendwin [22mdoes not, so that an application can
     resume  managing a terminal with [4mcurses[24m after a (possibly con‐
     ditional or temporary) suspension; see  [1mcurs_kernel[22m(3X).   Use
     [1mdelscreen  [22mafter  [1mendwin [22mwhen the application has no more need
     of a terminal device but will not soon exit.

[1mRETURN VALUE[0m
     [1mdelscreen [22mreturns no value.  [1mendwin [22mreturns [1mOK [22mon success  and
     [1mERR  [22mon  failure.  [1misendwin [22mreturns [1mTRUE [22mor [1mFALSE [22mas described
     above.

     In [4mncurses[24m,

     •   [1mendwin [22mreturns [1mERR [22mif

         •   the terminal was not initialized,

         •   it is called  more  than  once  without  updating  the
             screen, or

         •   its call of [1mreset_shell_mode[22m(3X) returns [1mERR[22m; and

     •   [1mnewterm  [22mreturns [1mERR [22mif it cannot allocate storage for the
         [4mSCREEN[24m structure or the  [4mWINDOW[24m  structures  automatically
         associated with it: [1mcurscr[22m, [1mnewscr[22m, and [1mstdscr[22m.

     Functions  that return pointers return null pointers on error.
     In [4mncurses[24m, [1mset_term [22mdoes not fail, and [1minitscr [22mexits the  ap‐
     plication if it does not operate successfully.

[1mNOTES[0m
     [4mncurses[24m  establishes signal handlers when a function that ini‐
     tializes a [4mSCREEN[24m, either [1minitscr [22mor [1mnewterm[22m, is first called.
     Applications that wish to handle the following  signals  them‐
     selves  should  set up their corresponding handlers [4mafter[24m ini‐
     tializing the screen.

     [4mSIGINT[0m
            [4mncurses[24m's handler [4mattempts[24m to clean up  the  screen  on
            exit.  Although it [4musually[24m works as expected, there are
            limitations.

            •   Walking  the  [4mSCREEN[24m list is unsafe, since all list
                management is done without any signal blocking.

            •   When  an  application  has  been  built  with   the
                [4m_REENTRANT[24m  macro defined (and corresponding system
                support), [1mset_term [22muses functions that could  dead‐
                lock or misbehave in other ways.

            •   [1mendwin  [22mcalls  other  functions,  many of which use
                [4mstdio[24m(3)  or  other  library  functions  that   are
                clearly unsafe.

     [4mSIGTERM[0m
            [4mncurses[24m  uses  the same handler as for [4mSIGINT[24m, with the
            same  limitations.   It  is  not  mentioned  in  X/Open
            Curses,  but  is  more  suitable  for this purpose than
            [4mSIGQUIT[24m (which is used in debugging).

     [4mSIGTSTP[0m
            [4mncurses[24m's handler manages the  terminal-generated  stop
            signal,   used  in  job  control.   When  resuming  the
            process,   [4mncurses[24m   discards   pending   input    with
            [1mflushinp[22m(3X)  and repaints the screen, assuming that it
            has been completely altered.  It also updates the saved
            terminal modes with [1mdef_shell_mode[22m(3X).

     [4mSIGWINCH[0m
            [4mncurses[24m handles changes to the terminal's window  size,
            a  phenomenon  ignored  in standardization efforts.  It
            sets a (signal-safe) variable that is later  tested  by
            [1mwgetch[22m(3X) and [1mwget_wch[22m(3X).

            •   [1mwgetch [22mreturns the key code [1mKEY_RESIZE[22m.

            •   [1mwget_wch  [22mreturns [1mKEY_CODE_YES [22mand sets its [4mwch[24m pa‐
                rameter to [1mKEY_RESIZE[22m.

            At the same time, [4mncurses[24m calls [1mresizeterm[22m(3X)  to  ad‐
            just the standard screen [1mstdscr [22mand update global vari‐
            ables such as [1mLINES [22mand [1mCOLS[22m.

[1mPORTABILITY[0m
     X/Open Curses Issue 4 describes these functions.  It specifies
     no error conditions for them.

   [1mDifferences[0m
     X/Open  Curses  specifies  that portable applications must not
     call [4minitscr[24m more than once.

     •   The portable way  to  use  [4minitscr[24m  is  once  only,  using
         [4mrefresh[24m to restore the screen after [4mendwin[24m.

     •   [4mncurses[24m permits use of [4minitscr[24m after [4mendwin[24m.

     [4minitscr[24m  in  BSD,  from its inception (1980) through the Net/2
     release (1991) returned [4mERR[24m cast to a [4mWINDOW[24m pointer when  de‐
     tecting  an  error.   4.4BSD  (1995)  instead  returned a null
     pointer.  Neither exited the application.  It is safe but  re‐
     dundant to check the return value of [4minitscr[24m in X/Open Curses.

     Calling  [4mendwin[24m  does  not  dispose of the memory allocated by
     [4minitscr[24m or [4mnewterm[24m.  Deleting a [4mSCREEN[24m provides a  way  to  do
     this.

     •   X/Open  Curses  does  not say what happens to [4mWINDOW[24ms when
         [4mdelscreen[24m “frees storage associated with the  [4mSCREEN[24m”  nor
         does the SVr4 documentation help, adding that it should be
         called after [4mendwin[24m if a [4mSCREEN[24m is no longer needed.

     •   However,  every  [4mWINDOW[24m  is  implicitly  associated with a
         [4mSCREEN[24m, so it is reasonable to expect [4mdelscreen[24m to dispose
         of them.

     •   SVr4 deletes the standard  [4mWINDOW[24m  structures  [4mstdscr[24m  and
         [4mcurscr[24m  as  well  as a work area [4mnewscr[24m.  It ignores other
         windows.

     •   Since version 4.0 (1996), [4mncurses[24m has maintained a list of
         all windows for each screen,  using  that  information  to
         delete those windows when [4mdelscreen[24m is called.

     •   NetBSD  copied  this feature of [4mncurses[24m in 2001.  [4mPDCurses[0m
         follows the SVr4 model, deleting only the standard  [4mWINDOW[0m
         structures and [4mnewscr[24m.

   [1mHigh-level versus Low-level Functions[0m
     Implementations  disagree  regarding  the level of abstraction
     applicable to a function or  property.   For  example,  [4mSCREEN[0m
     (returned by [4mnewterm[24m) and [4mTERMINAL[24m (returned by [1msetupterm[22m(3X))
     hold  file  descriptors for the output stream.  If an applica‐
     tion switches screens using [4mset_term[24m,  or  switches  terminals
     using  [1mset_curterm[22m(3X), applications using the output file de‐
     scriptor can behave differently  depending  on  the  structure
     holding the corresponding descriptor.

     •   NetBSD's   [4mbaudrate[24m   function   uses  the  descriptor  in
         [4mTERMINAL[24m.  [4mncurses[24m and SVr4 use the descriptor in [4mSCREEN[24m.

     •   NetBSD and [4mncurses[24m use the descriptor in [4mTERMINAL[24m for ter‐
         minal     I/O     modes,     e.g.,     [1mdef_shell_mode[22m(3X),
         [1mdef_prog_mode[22m(3X).  SVr4 uses the descriptor in [4mSCREEN[24m.

   [1mUnset [4mTERM[24m Environment Variable[0m
     If  the  [4mTERM[24m variable is not set in the environment or has an
     empty value, [4minitscr[24m uses the value “unknown”, which  normally
     corresponds to a terminal entry with the [1mgeneric [22m([1mgn[22m) capabil‐
     ity.  Generic entries are detected by [1msetupterm[22m(3X) and cannot
     be  used for full-screen operation.  Other implementations may
     handle a missing or empty [4mTERM[24m variable differently.

   [1mSignal Handlers[0m
     Quoting X/Open Curses Issue 7, section 3.1.1:

          Curses implementations may provide for  special  handling
          of the SIGINT, SIGQUIT, and SIGTSTP signals if their dis‐
          position is SIG_DFL at the time [4minitscr[24m() is called...

          Any  special handling for these signals may remain in ef‐
          fect for the life of the process  or  until  the  process
          changes the disposition of the signal.

          None of the Curses functions are required to be safe with
          respect to signals...

     Section “NOTES” above discusses [4mncurses[24m's signal handlers.

[1mHISTORY[0m
     4BSD (1980) introduced [4minitscr[24m and [4mendwin[24m.

     SVr2 (1984) added [4mnewterm[24m and [4mset_term[24m.

     SVr3.1 (1987) supplied [4mdelscreen[24m and [4misendwin[24m.

[1mSEE ALSO[0m
     [1mcurses[22m(3X),  [1mcurs_kernel[22m(3X),  [1mcurs_refresh[22m(3X), [1mcurs_slk[22m(3X),
     [1mcurs_terminfo[22m(3X), [1mcurs_util[22m(3X), [1mcurs_variables[22m(3X)

ncurses 6.6                  2025-08-23            [4mcurs_initscr[24m(3X)
