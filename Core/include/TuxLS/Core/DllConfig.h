//
// Author oldlonecoder, lussier.serge@gmail.com
//

#pragma once



#ifdef _WIN32
#   pragma warning(disable : 4996)
#   pragma warning(disable : 4005)
#   pragma warning(disable : 4267)
//class __declspec(dllimport) std::thread;
#   define _CRT_SECURE_NO_WARNINGS
#   define _SCL_SECURE_NO_WARNINGS

#   ifdef CORE_DLL_EXPORT
#       define CORE_LIB __declspec(dllexport)
#   else
#       define CORE_LIB __declspec(dllimport)
#   endif
#else
#   define CORE_LIB   __attribute__ ((visibility ("default"))) // Not ms-Windows/MSVC
#endif

// Mandatory. Globally used where it needs to.
#ifdef _WIN32
#   include <Windows.h> // Must be in before everythings
#   include <thread>
#   include <cstdint>
#   define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

//---------------------------------------------


// --  Basic pseudo reflexion macros : --
#ifndef DECLOBJ_ID
#define DECLOBJ_ID \
    public:\
        virtual const char*  Class() const noexcept; \
    private:\


#define  CLASSNAME_IMPL(CLASS) \
         const char*   CLASS::Class() const noexcept{ return #CLASS; }
#endif
//----------------------------------------------------------------------------





