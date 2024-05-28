#include "common.h"
#include <args.hpp>

CPrefs::CPrefs(int argc, char* argv[], int* result) {
	*result = NOK;

	args::ArgumentParser parser = args::ArgumentParser("");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::Flag list(parser, "list-devices", "List available devices", {'l', "list-devices"});
	args::ValueFlag<int> outDevice(parser, "device", "Output device number", {'d', "device"}, -1);

	try {
		parser.ParseCLI(argc, argv);
	}
	catch (const args::Help&) {
		LOG(INFO) << parser;
		*result = END;
		return;
	}
	catch (const args::ParseError& e) {
		LOG(WARNING) << e.what();
		LOG(INFO) << "Run with --help to view available options.";
		*result = END;
		return;
	}
	catch (const args::ValidationError& e) {
		LOG(WARNING) << e.what();
		LOG(INFO) << "Run with --help to view available options.";
		*result = END;
		return;
	}


	ma_result hr;
	if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
		LOG(WARNING) << "Failed to initialize context.";
		return;
	}
	contextInit = true;


	hr = ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
	if (hr != MA_SUCCESS) {
		LOG(WARNING) << ("Failed to retrieve device information.");
		return;
	}

	if(list) {
		listDevices();
		*result = END;
	}
	if (outDevice.Get() != -1) {
		*result = initDevice(outDevice.Get());
		return;
	}
	*result = END;
}

void CPrefs::listDevices() {
	ma_uint32 iDevice;

	LOG(INFO) << ("Available Devices");
	for (iDevice = 0; iDevice < playbackDeviceCount; ++iDevice) {
		if(pPlaybackDeviceInfos[iDevice].isDefault)
			LOG(INFO) << "   " << iDevice << ": " << pPlaybackDeviceInfos[iDevice].name << "\033[32m  -> (default)\033[m";
		else LOG(INFO) << "   " << iDevice << ": " << pPlaybackDeviceInfos[iDevice].name;
	}
}

int CPrefs::initDevice(int device) {
	ma_result result;

	if (device < 0 || device >= (int)playbackDeviceCount) {
		LOG(WARNING) << ("Invalid device number");
		LOG(INFO) << ("Run with --list-devices to list available devices.");
		return NOK;
	}

	if (playbackDeviceCount < 2) {
		LOG(WARNING) << ("Not enough playback devices. \nThere must be more than one playback device");
		return NOK;
	}

	if (pPlaybackDeviceInfos[device].isDefault) {
		LOG(WARNING) << ("Cannot use default playback device");
		LOG(INFO) << ("Please select another device.");
		return NOK;
	}

	/* Loopback mode is currently only supported on WASAPI. */
	ma_backend backends[] = {
		ma_backend_wasapi
	};


	cDeviceConfig = ma_device_config_init(ma_device_type_loopback);
	cDeviceConfig.capture.pDeviceID = NULL;
	cDeviceConfig.capture.format = ma_format_f32;
	cDeviceConfig.capture.channels = 2;
	cDeviceConfig.sampleRate = 44100;
	cDeviceConfig.dataCallback = loopback;


	pDeviceConfig = ma_device_config_init(ma_device_type_playback);
	pDeviceConfig.playback.pDeviceID = &(pPlaybackDeviceInfos[device].id);
	pDeviceConfig.playback.format = cDeviceConfig.capture.format;
	pDeviceConfig.playback.channels = cDeviceConfig.capture.channels;
	pDeviceConfig.sampleRate = cDeviceConfig.sampleRate;
	pDeviceConfig.dataCallback = playback;

	input = malloc(1000 * ma_get_bytes_per_frame(cDeviceConfig.capture.format, cDeviceConfig.capture.channels));
	pDeviceConfig.pUserData = input;
	cDeviceConfig.pUserData = input;

	return ALL_OK;
}