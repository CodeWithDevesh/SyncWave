#define MINIAUDIO_IMPLEMENTATION
#include "common.hpp"
#include <g3log/logworker.hpp>
#include <CustomSink.hpp>


bool data = false;
ma_uint32 size;
bool stoped = false;


void loopback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	if (stoped)
		return;
	size = frameCount * ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels);
	void* newMem = realloc(pDevice->pUserData, size );
	if (newMem == nullptr) {
		return;
	}
	pDevice->pUserData = newMem;
	memcpy(pDevice->pUserData, pInput, size);
	data = true;
}

void playback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	if (data && !stoped) {
		MA_COPY_MEMORY(pOutput, pDevice->pUserData, size);
	}
	data = false;
	(void)pInput;
}


int main(int argc, char* argv[])
{
	const std::string directory = "./";
	const std::string name = "SyncWave";
	auto worker = g3::LogWorker::createLogWorker();
	worker->addSink(std::make_unique<CustomSink>(), &CustomSink::ReceiveLogMessage);

	initializeLogging(worker.get());

	int ret = 0;
	CPrefs prefs(argc, argv, &ret);
	if (ret != ALL_OK) {
		return ret;
	}

	ma_device pdevice;
	ma_device cdevice;
	ma_device_config pDeviceConfig = prefs.pDeviceConfig, cDeviceConfig = prefs.cDeviceConfig;


	ma_result result;

	result = ma_device_init(NULL, &pDeviceConfig, &pdevice);
	if (result != MA_SUCCESS) {
		LOG(FATAL) << ("Failed to initialize playback device.\n");
		return -1;
	}
	UninitDeviceOnExit playback(&pdevice);


	/* Loopback mode is currently only supported on WASAPI. */
	ma_backend backends[] = {
		ma_backend_wasapi
	};
	result = ma_device_init_ex(backends, sizeof(backends) / sizeof(backends[0]), NULL, &cDeviceConfig, &cdevice);
	if (result != MA_SUCCESS) {
		LOG(FATAL) << ("Failed to initialize loopback device.\n");
		return -1;
	}
	UninitDeviceOnExit loopback(&cdevice);


	result = ma_device_start(&cdevice);
	if (result != MA_SUCCESS) {
		LOG(FATAL) << ("Failed to start capture device.\n");
		return -1;
	}
	result = ma_device_start(&pdevice);
	if (result != MA_SUCCESS) {

		LOG(FATAL) << ("Failed to start playback device.\n");
		return -1;
	}

	std::cout << ("Press Enter to stop...");
	getchar();
	stoped = true;

	return 0;
}