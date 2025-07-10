#include "common.hpp"

typedef struct
{
	ma_pcm_rb buffer;
	ma_uint32 frameSizeInBytes;
} SyncWaveContext;

void loopback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
	SyncWaveContext *ctx = (SyncWaveContext *)pDevice->pUserData;
	void *bufferOut;
	ma_uint32 framesToWrite = frameCount;
	if (ma_pcm_rb_acquire_write(&ctx->buffer, &framesToWrite, &bufferOut) != MA_SUCCESS)
	{
		warn("Failed to aquire write on buffer");
		return;
	}
	if (framesToWrite < frameCount)
	{
		trace("Buffer full, dropped {} frames.", (frameCount - framesToWrite));
	}

	memcpy(bufferOut, pInput, static_cast<size_t>(framesToWrite) * ctx->frameSizeInBytes);
	ma_pcm_rb_commit_write(&ctx->buffer, framesToWrite);
}

void playback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
	SyncWaveContext *ctx = (SyncWaveContext *)pDevice->pUserData;
	void *bufferIn;
	ma_uint32 framesToRead = frameCount;
	if (ma_pcm_rb_acquire_read(&ctx->buffer, &framesToRead, &bufferIn) != MA_SUCCESS)
	{
		warn("Failed to acquire read on buffer");
		return;
	}
	if (framesToRead < frameCount)
	{
		trace("Buffer empty, dropped {} frames.", (frameCount - framesToRead));
	}

	memcpy(pOutput, bufferIn, static_cast<size_t>(framesToRead) * ctx->frameSizeInBytes);
	ma_pcm_rb_commit_read(&ctx->buffer, framesToRead);

	(void)pInput;
}

int main(int argc, char *argv[])
{
	const std::string directory = "./";
	const std::string name = "SyncWave";
	ma_result result;

	int ret = 0;
	CPrefs prefs(argc, argv, &ret);
	if (ret != ALL_OK)
		return ret;

	SyncWaveContext ctxt{};
	result = ma_pcm_rb_init(ma_format_f32, 2, 16 * 48000, NULL, NULL, &(ctxt.buffer));
	if (result != MA_SUCCESS)
	{
		crit("Error: %s", ma_result_description(result));
		return -1;
	}
	ctxt.frameSizeInBytes = ma_get_bytes_per_frame(ma_format_f32, 2);

	ma_device pdevice;
	ma_device cdevice;
	ma_device_config pDeviceConfig = prefs.pDeviceConfig, cDeviceConfig = prefs.cDeviceConfig;
	pDeviceConfig.pUserData = &ctxt;
	cDeviceConfig.pUserData = &ctxt;

	result = ma_device_init(NULL, &pDeviceConfig, &pdevice);
	if (result != MA_SUCCESS)
	{
		crit("Error: %s", ma_result_description(result));
		return -1;
	}
	UninitDeviceOnExit playback(&pdevice);

	ma_backend backends[] = {
			ma_backend_wasapi};
	result = ma_device_init_ex(backends, sizeof(backends) / sizeof(backends[0]), NULL, &cDeviceConfig, &cdevice);
	if (result != MA_SUCCESS)
	{
		crit("Error: %s", ma_result_description(result));
		return -1;
	}
	UninitDeviceOnExit loopback(&cdevice);

	result = ma_device_start(&cdevice);
	if (result != MA_SUCCESS)
	{
		crit("Error: %s", ma_result_description(result));
		return -1;
	}

	// std::this_thread::sleep_for(std::chrono::milliseconds(100));

	result = ma_device_start(&pdevice);
	if (result != MA_SUCCESS)
	{
		crit("Error: %s", ma_result_description(result));
		return -1;
	}

	std::cout << "Press Enter to stop..." << std::endl;
	getchar();
	return 0;
}