#include <stdbool.h>
#include "../arraylist.h"
#include "../log.h"

bool builtin_arch(arraylist* args, int count, arraylist* out) {
	if (count != 0) {
		log_error("@arch(): Expects no arguments");
		return false;
	}

	char* arch;


	// Source - https://stackoverflow.com/a/66249936
	// Posted by FreakAnon, modified by community. See post 'Timeline' for change history
	// Retrieved 2026-09-28, License - CC BY-SA 4.0

#if defined(__x86_64__) || defined(_M_X64)
	arch = "x86_64";
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
	arch = "x86_32";
#elif defined(__ARM_ARCH_2__)
	arch = "ARM2";
#elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
	arch = "ARM3";
#elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
	arch = "ARM4T";
#elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
	arch = "ARM5"
#elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
	arch = "ARM6T2";
#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
	arch = "ARM6";
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
	arch = "ARM7";
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
	arch = "ARM7A";
#elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
	arch = "ARM7R";
#elif defined(__ARM_ARCH_7M__)
	arch = "ARM7M";
#elif defined(__ARM_ARCH_7S__)
	arch = "ARM7S";
#elif defined(__aarch64__) || defined(_M_ARM64)
	arch = "ARM64";
#elif defined(mips) || defined(__mips__) || defined(__mips)
	arch = "MIPS";
#elif defined(__sh__)
	arch = "SUPERH";
#elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
	arch = "POWERPC";
#elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
	arch = "POWERPC64";
#elif defined(__sparc__) || defined(__sparc)
	arch = "SPARC";
#elif defined(__m68k__)
	arch = "M68K";
#else
#error "Unknown processor architecture"
#endif

	arraylist_append(out, &arch);

	return true;
}
