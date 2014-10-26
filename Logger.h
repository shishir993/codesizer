#pragma once

// ---------------------------------------------------
// The CodeSizer Project
// Github: https://github.com/shishir993/codesizer
// Author: Shishir Bhat [http://www.shishirbhat.com]
// The MIT License (MIT)
// Copyright (c) 2014
//
// Logger
// Logging helpers defines as a set of macros
//

#ifdef __cplusplus
extern "C" {  
#endif

/**
 * Original work from http://c.learncodethehardway.org/book/ex16.html
 * Modified/additions to my liking
 */

#define logtrace(M, ...) fwprintf(stderr, L"[TRACE] %s : " M L"\n", __FUNCTIONW__, ##__VA_ARGS__)
#define logerr(M, err, ...)  fwprintf(stderr, L"[ERROR] %s : (error: 0x%08x) " M L"\n", __FUNCTIONW__, err, ##__VA_ARGS__)
#define logwarn(M, ...) fwprintf(stderr, L"[WARN ] %s :" M L"\n", __FUNCTIONW__, ##__VA_ARGS__)
#define loginfo(M, ...) fwprintf(stderr, L"[INFO ] %s : " M L"\n", __FUNCTIONW__, ##__VA_ARGS__)

#ifdef _DEBUG

    #define logdbg(M, ...)  fwprintf(stdout, L"[DEBUG] %s : " M L"\n", __FUNCTIONW__, ##__VA_ARGS__)

#else
    #define logdbg(M, ...)

#endif

#ifdef __cplusplus
}
#endif
