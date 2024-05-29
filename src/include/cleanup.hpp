#pragma once
#include "miniaudio.hpp"
#include <stdlib.h>

class UninitDeviceOnExit {
	ma_device* device;
public:
	UninitDeviceOnExit(ma_device* device) : device(device) {}
	~UninitDeviceOnExit() {
		ma_device_uninit(device);
	}
};

class UninitContextOnExit {
	ma_context* context;
public:
	UninitContextOnExit(ma_context* context) : context(context) {}
	~UninitContextOnExit() {
		ma_context_uninit(context);
	}
};

class FreePointerOnExit {
	void* pointer;
	public:
		FreePointerOnExit(void* pointer) : pointer(pointer) {}
		~FreePointerOnExit() {
			free(pointer);
		}
};

class DeletePointerOnExit {
	void* pointer;
	public:
		DeletePointerOnExit(void* pointer) : pointer(pointer) {}
		~DeletePointerOnExit() {
			delete pointer;
		}
};