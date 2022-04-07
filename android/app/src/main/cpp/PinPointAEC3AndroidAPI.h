//
//  PinPointAECAndroidAPI.hpp
//  PinPointAECAndroid
//
//

#ifndef PinPointAEC3AndroidAPI
#define PinPointAEC3AndroidAPI

#ifdef __cplusplus
extern "C"
{
#endif

__attribute__(( visibility("default") ))  void* PinPointAEC3Create(int sampleRate, bool linearOutput);
__attribute__(( visibility("default") ))  void PinPointAEC3Destroy(void *object);
__attribute__(( visibility("default") ))  void PinPointAEC3ProcessCapture(void* object, float* audioBuffer, int numberOfFrames);
__attribute__(( visibility("default") ))  void PinPointAEC3ProcessCaptureLinear(void* object, float* audioBuffer, float* linearBuffer, int numberOfFrames);
__attribute__((visibility("default"))) void PinPointAEC3ProcessCaptureMVM(void* object, float* audioBuffer, float* linearAudioBuffer, int numberOfFrames);
__attribute__(( visibility("default") ))  void PinPointAEC3AnalyzeRender(void* object, float* audioBuffer, int numberOfFrames);

#ifdef __cplusplus
}
#endif

#endif
