#pragma once

void playback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
void loopback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);