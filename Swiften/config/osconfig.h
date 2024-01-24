#pragma once


#if defined(_MSC_VER)
    #define SWIFTEN_I inline __forceinline
    #define SWIFTEN_N __declspec(noinline)
#elif defined(__GNUC__) || defined(__GNUG__)
    #define SWIFTEN_I inline __attribute__((always_inline))
    #define SWIFTEN_N __attribute__((noinline))    
#endif

#define SWIFTEN_SC static constexpr
#define SWIFTEN_NSC SWIFTEN_N SWIFTEN_SC
#define SWIFTEN_ISC SWIFTEN_I SWIFTEN_SC