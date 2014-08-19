// Copyright 2014 Louix Gu
// Author: gzc9047@gmail.com (Louix Gu)

// CppFreeMock: a tool for mock global function, member function, class static function.
//
// Check os and include the special implement.

#ifndef CPP_FREE_MOCK_RUNTIME_PATCH_H_
#define CPP_FREE_MOCK_RUNTIME_PATCH_H_

#include <vector>

namespace CppFreeMock {

namespace RuntimePatcherImpl {
    // Need impl in architecture relevant file.
    static int UnprotectMemory(const void* const address, const size_t length);
    static int UnprotectMemoryForOnePage(void* const address);
    static int SetJump(void* const address, const void* const destination, std::vector<char>& binary_backup);
    static void RevertJump(void* address, const std::vector<char>& binary_backup);
}

} // namespace CppFreeMock

#if defined(__x86_64__) || defined(__i386__)
#include "x86/runtime_patch_impl.h"
#endif

#ifdef __APPLE__
#include "posix/runtime_patch_impl.h"
#elif __linux__
#include "posix/runtime_patch_impl.h"
#endif

namespace CppFreeMock {

// Provide 2 interface, GraftFunction and RevertGraft.
struct RuntimePatcher {
    template < typename F1, typename F2 >
    static int GraftFunction(F1 address, F2 destination, std::vector<char>& binary_backup) {
        void* function = reinterpret_cast<void*>((std::size_t&)address);
        if (0 != RuntimePatcherImpl::UnprotectMemoryForOnePage(function)) {
            int err = errno;
            std::cerr << "Unprotect memory meet errno: " << err << " description: " << strerror(err) << std::endl;
            std::abort();
        }
        return RuntimePatcherImpl::SetJump(function, reinterpret_cast<void*>((std::size_t&)destination), binary_backup);
    }

    template < typename F >
    static void RevertGraft(F address, const std::vector<char>& binary_backup) {
        RuntimePatcherImpl::RevertJump(reinterpret_cast<void*>((std::size_t&)address), binary_backup);
    }
};

} // namespace CppFreeMock

#endif // CPP_FREE_MOCK_RUNTIME_PATCH_H_
