#pragma once
class CPrefs {
	ma_context context;
	ma_device_info* pPlaybackDeviceInfos;
	ma_uint32 playbackDeviceCount;
	ma_device_info* pCaptureDeviceInfos;
	ma_uint32 captureDeviceCount;
	bool contextInit = false;


public:
	ma_device_config pDeviceConfig, cDeviceConfig;
	void* input = NULL;

	CPrefs(int argc, char* argv[], int* result);

	~CPrefs() {
		if(contextInit)
			ma_context_uninit(&context);
		if(input != NULL)
			free(input);
	}

	void listDevices();
	int initDevice(int);
};
