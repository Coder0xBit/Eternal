#pragma once

#ifdef _DEBUG
#define ETERNAL_VULKAN_DEBUG_VALIDATION 1
#else 
#define ETERNAL_VULKAN_DEBUG_VALIDATION 0
#endif

#define ETERNAL_VULKAN_ENABLED(flags) ((flags & 1) == 1)
