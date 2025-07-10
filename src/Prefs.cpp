#include "common.hpp"
#include <args.hpp>

CPrefs::CPrefs(int argc, char *argv[], int *result)
{
	*result = NOK;

	args::ArgumentParser parser = args::ArgumentParser("");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::Flag list(parser, "list-devices", "List available devices", {'l', "list-devices"});
	args::ValueFlag<int> outDevice(parser, "device", "Output device number", {'d', "device"}, -1);

	try
	{
		parser.ParseCLI(argc, argv);
	}
	catch (const args::Help &)
	{
		std::cout << parser.Help() << std::endl;
		*result = END;
		return;
	}
	catch (const args::ParseError &e)
	{
		std::cout << e.what() << std::endl;
		std::cout << parser.Help() << std::endl;
		*result = END;
		return;
	}
	catch (const args::ValidationError &e)
	{
		std::cout << e.what() << std::endl;
		std::cout << parser.Help() << std::endl;
		*result = END;
		return;
	}

	ma_result hr;
	if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS)
	{
		warn("Failed to initialize audio context.");
		return;
	}
	contextInit = true;

	hr = ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
	if (hr != MA_SUCCESS)
	{
		warn("Failed to get devices: {}", ma_result_description(hr));
		return;
	}

	if (list)
	{
		listDevices();
		*result = END;
	}
	if (outDevice.Get() != -1)
	{
		*result = initDevice(outDevice.Get());
		return;
	}
	*result = END;
}

void CPrefs::listDevices()
{
	ma_uint32 iDevice;

	std::cout << "  Playback devices: " << playbackDeviceCount << std::endl;
	for (iDevice = 0; iDevice < playbackDeviceCount; ++iDevice)
	{
		if (pPlaybackDeviceInfos[iDevice].isDefault)
			std::cout << "   " << iDevice << ": " << pPlaybackDeviceInfos[iDevice].name << "\033[32m  -> (default)\033[m";
		else
			std::cout << "   " << iDevice << ": " << pPlaybackDeviceInfos[iDevice].name;

		std::cout << std::endl;
	}
}

int CPrefs::initDevice(int device)
{
	if (device < 0 || device >= (int)playbackDeviceCount)
	{
		std::cout << "Invalid device number. Please select a valid device." << std::endl;
		std::cout << "Run with --list-devices to list available devices." << std::endl;
		return NOK;
	}

	if (playbackDeviceCount < 2)
	{
		std::cout << "Not enough playback devices. There must be more than one playback device." << std::endl;
		return NOK;
	}

	if (pPlaybackDeviceInfos[device].isDefault)
	{
		std::cout << "Cannot use default playback device. Please select another device." << std::endl;
		return NOK;
	}

	/* Loopback mode is currently only supported on WASAPI. */
	ma_backend backends[] = {
			ma_backend_wasapi};

	cDeviceConfig = ma_device_config_init(ma_device_type_loopback);
	cDeviceConfig.capture.pDeviceID = NULL;
	cDeviceConfig.capture.format = ma_format_f32;
	cDeviceConfig.capture.channels = 2;
	cDeviceConfig.sampleRate = 44100;
	cDeviceConfig.dataCallback = loopback;
	cDeviceConfig.wasapi.noAutoConvertSRC = true;

	pDeviceConfig = ma_device_config_init(ma_device_type_playback);
	pDeviceConfig.playback.pDeviceID = &(pPlaybackDeviceInfos[device].id);
	pDeviceConfig.playback.format = cDeviceConfig.capture.format;
	pDeviceConfig.playback.channels = cDeviceConfig.capture.channels;
	pDeviceConfig.sampleRate = cDeviceConfig.sampleRate;
	pDeviceConfig.dataCallback = playback;
	pDeviceConfig.wasapi.noAutoConvertSRC = true;

	return ALL_OK;
}