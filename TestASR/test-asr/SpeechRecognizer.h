// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the SPEECHRECOGNIZER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// SPEECHRECOGNIZER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SPEECHRECOGNIZER_EXPORTS
#define SPEECHRECOGNIZER_API __declspec(dllexport)
#else
#define SPEECHRECOGNIZER_API __declspec(dllimport)
#endif

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <thread>
#include <functional>
#include <thread>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <mftransform.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <dmo.h>
#include <evr.h>
#include <Objbase.h>
#include <assert.h>
#include <stdint.h>
#include <wmcodecdsp.h>
#include <chrono>
#include <thread>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <queue>

#include "WWMFResampler.h"
#include "c-api.h"

using namespace std;
#pragma warning(disable: 4251)
struct Configuration {
    std::string modelDir = ""; // model folder path
    int modelSampleRate = 16000; // default 16khz
    std::string recordingDir = ""; // folder to save aac recording
    std::string decodeMethod = "modified_beam_search"; // greedy_search or modified_beam_search
    bool recordSherpaAudio = false; // turn on/off recording sherpa audio
    std::string resultMode = "tokens"; // text,tokens or json mode
    float rule1 = 2.4f; // rule1_min_trailing_silence
    float rule2 = 1.5f; // rule2_min_trailing_silence
    float rule3 = 600.0f; // rule3_min_utterance_length
    float contextScore = 1.5f; // context score
    std::string modelType = "zipformer2";
    std::string provider = "cpu";
    bool debug = true;
    float featureDim = 80;
    bool enableEndPoint = true;
    int numThreads = 2;
    int numActivePaths = 4;
    int useVulkanCompute = 0;
    std::string tokensName = "tokens.txt";
    std::string encoderName = "encoder.int8.ort";
    std::string decoderName = "decoder.int8.ort";
    std::string joinerName = "joiner.int8.ort";

    // VAD params
    std::string vadModelDir = "";
    std::string vadModelName = "silero_vad.ort";
    float vadThreshold = 0.25f;
    float vadMinSilenceDuration = 0.20f;
    float vadMinSpeechDuration = 0.15f;
    float vadMaxSpeechDuration = 5.0f;
    int vadWindowsSize = 400;
    int vadSampleRate = 16000;
    int vadNumThread = 1;
    std::string vadProvider = "cpu";

    std::string tokensPath() {
        return modelDir + tokensName;
    }

    std::string encoderPath() {
        return modelDir + encoderName;
    }

    std::string decoderPath() {
        return modelDir + decoderName;
    }

    std::string joinerPath() {
        return modelDir + joinerName;
    }

    std::string vadPath() {
        return vadModelDir + vadModelName;
    }

    friend std::ostream& operator<<(std::ostream& os, const Configuration& config) {
        os << "modelDir = " << config.modelDir << "\n";
        os << "modelSampleRate = " << config.modelSampleRate << "\n";
        os << "recordingDir = " << config.recordingDir << "\n";
        os << "decodeMethod = " << config.decodeMethod << "\n";
        os << "recordSherpaAudio = " << config.recordSherpaAudio << "\n";
        os << "resultMode = " << config.resultMode << "\n";
        os << "rule1 = " << config.rule1 << "\n";
        os << "rule2 = " << config.rule2 << "\n";
        os << "rule3 = " << config.rule3 << "\n";
        os << "contextScore = " << config.contextScore << "\n";
        os << "modelType = " << config.modelType << "\n";
        os << "provider = " << config.provider << "\n";
        os << "debug = " << config.debug << "\n";
        os << "featureDim = " << config.featureDim << "\n";
        os << "enableEndPoint = " << config.enableEndPoint << "\n";
        os << "numThreads = " << config.numThreads << "\n";
        os << "numActivePaths = " << config.numActivePaths << "\n";
        os << "useVulkanCompute = " << config.useVulkanCompute << "\n";
        os << "tokensName = " << config.tokensName << "\n";
        os << "encoderName = " << config.encoderName << "\n";
        os << "decoderName = " << config.decoderName << "\n";
        os << "joinerName = " << config.joinerName << "\n";
        os << "vadModelName = " << config.vadModelName << "\n";
        os << "vadThreshold = " << config.vadThreshold << "\n";
        os << "vadMinSilenceDuration = " << config.vadMinSilenceDuration << "\n";
        os << "vadMinSpeechDuration = " << config.vadMinSpeechDuration << "\n";
        os << "vadMaxSpeechDuration = " << config.vadMaxSpeechDuration << "\n";
        os << "vadWindowsSize = " << config.vadWindowsSize << "\n";
        os << "vadSampleRate = " << config.vadSampleRate << "\n";
        os << "vadNumThread = " << config.vadNumThread << "\n";
        os << "vadProvider = " << config.vadProvider << "\n";
        return os;
    }
};

struct WavHeader {
    char RIFF[4];
    DWORD bytes;
    char WAVE[4];
    char fmt[4];
    int siz_wf;
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
    WORD wBitsPerSample;
    char data[4];
    DWORD pcmbytes;
};

enum SpeechRecognizerStatus {
    SpeechRecognizerStart,
    SpeechRecognizerNormal,
    SpeechRecognizerListen,
    SpeechRecognizerStopListen,
    SpeechRecognizerMute,
    SpeechRecognizerRelease
};


/// <summary>
/// Speech recognizer to manage recognizing and recording microphone
///  This class is exported from the dll
/// </summary>
class SPEECHRECOGNIZER_API SpeechRecognizer
{
public:
    // Constructor & Destructor
    SpeechRecognizer();
    SpeechRecognizer(const Configuration& config);
    SpeechRecognizer(const Configuration& config, std::string speechText, std::string recordingId);
    ~SpeechRecognizer();

    /// <summary>
    /// Start to listen mic audio
    /// </summary>
    /// <returns></returns>
    HRESULT listen();

    /// <summary>
    /// Stop to listen mic audio
    /// </summary>
    /// <returns></returns>
    HRESULT stopListening();

    /// <summary>
    /// Pause listen
    /// </summary>
    /// <returns></returns>
    HRESULT mute();

    /// <summary>
    /// Resume listen
    /// </summary>
    /// <returns></returns>
    HRESULT unmute();

    /// <summary>
    /// Setup microphone and ASR model
    /// </summary>
    /// <param name="recordingId"></param>
    /// <param name="recordingPath_s"></param>
    /// <returns></returns>
    HRESULT initialize(std::string recordingId, std::string recordingPath_s); // set recordingId property

    /// <summary>
    /// Release all resources
    /// </summary>
    void release();

    /// <summary>
    /// Reset ASR speech
    /// </summary>
    void resetSpeech();

    /// <summary>
    /// flush speech
    /// </summary>
    /// <param name="speechText"></param>
    void flushSpeech(std::string speechText); // set speech text property

    /// <summary>
    /// Add a callback (text, isEndpoint, isReset, isVoiceActive, isNoSpeech) to receive value from ASR
    /// </summary>
    /// <param name="listener"></param>
    void addListener(const std::function<void(const std::string&, bool, bool, bool, bool)>& listener);

    /// <summary>
    /// Remove a callback (text, isEndpoint, isReset, isVoiceActive, isNoSpeech) ASR
    /// </summary>
    /// <param name="listener"></param>
    void removeListener(const std::function<void(const std::string&, bool, bool, bool, bool)>& listener);

    /// <summary>
    /// Clear all listeners
    /// </summary>
    void removeAllListeners();

    /// <summary>
    /// Add a callback to receive volume level
    /// </summary>
    /// <param name="listener"></param>
    void addLevelListener(const std::function<void(float)>& listener);

    /// <summary>
    /// Remove a callback level
    /// </summary>
    /// <param name="listener"></param>
    void removeLevelListener(const std::function<void(float)>& listener);

    /// <summary>
    /// Clear all levels listeners
    /// </summary>
    void removeAllLevelListeners();

    /// <summary>
    /// Recognize from file
    /// </summary>
    /// <param name="audio_path"></param>
    /// <param name="output_path"></param>
    void recognizeAudio(std::string audio_path, std::string output_path);

    /// <summary>
    /// Convert wav to aac 
    /// </summary>
    /// <param name="inputFilePath"></param>
    /// <param name="outputFilePath"></param>
    /// <returns></returns>
    HRESULT ConvertWavToAac(LPCWSTR inputFilePath, LPCWSTR outputFilePath);

    /// <summary>
    /// Set Context biasing 
    /// </summary>
    /// <param name="hotWords"></param>
    /// <param name="destroyStream"></param>
    void setContextBiasing(std::string hotWords, bool destroyStream);

    /// <summary>
    /// Set measure listener
    /// </summary>
    /// <param name="listener"></param>
    void setMeasureListener(const std::function<void(string, long)>& listener);

private:
    /// <summary>
    /// Configuation for ASR and VAD
    /// </summary>
    Configuration configuration;

    /// <summary>
    /// Text to recognize
    /// </summary>
    std::string speechText;

    /// <summary>
    /// Recording id for audio file
    /// </summary>
    std::string recordingId;

    /// <summary>
    /// Recording audio path
    /// </summary>
    std::string recordingPath;

    /// <summary>
    /// Speech status
    /// </summary>
    SpeechRecognizerStatus recognizerStatus;


    /// <summary>
    /// Flag to indicate speech initialized
    /// </summary>
    bool isInitialized;

    /// <summary>
    /// A callback (text, isEndpoint, isReset, isVoiceActive, isNoSpeech) list
    /// </summary>
    vector<std::function<void(const std::string&, bool, bool, bool, bool)>> recogCallbackList;

    std::function<void(string, long)> measureListener;

    /// <summary>
    /// A level callback list
    /// </summary>
    vector<std::function<void(float)>> levelCallbackList;

    /// <summary>
    /// List of tasks to execute into the same ASR thread
    /// </summary>
    queue<std::function<void()>> threadCallbackList;

    /// <summary>
    /// VAD
    /// </summary>
    SherpaOnnxVoiceActivityDetector* vad = nullptr;

private:
    // resample variables and functions
    WWMFResampler iResampler;
    IMFMediaType* pInputType;
    IMFMediaType* pOutputType;
    HRESULT InitializeResample();
    HRESULT FinalizeResample();

    // recognition variables and functions
    std::atomic<SherpaOnnxOnlineRecognizer*> sherpaRecognizer;
    std::atomic<SherpaOnnxOnlineStream*> sherpaStream;

    HRESULT InitializeRecognition();
    HRESULT FinializeRecognition();

    std::thread* recogThread;
    void ProcessResampleRecogThread();
    bool IsFileExist(const std::string fileName);
    std::atomic<bool> isRecording;

public:
    SherpaOnnxOnlineRecognizerConfig config;
    SherpaOnnxVadModelConfig vadConfig;
    int curRecogBockIndex;
    HWAVEIN hWaveIn;
    list < WAVEHDR*> WaveHdrList;
    list < WAVEHDR*> WaveHdrSherpaList;
    SpeechRecognizerStatus getRecognizerStatus();
    bool isSerpaRecording();

    /// <summary>
    /// Resample audio buffer
    /// </summary>
    /// <param name="Block"></param>
    /// <param name="nBytes"></param>
    /// <param name="SampleBlock"></param>
    /// <param name="nSampleBytes"></param>
    /// <returns></returns>
    HRESULT Resample(BYTE* Block, int nBytes, BYTE* SampleBlock, int* nSampleBytes);

    /// <summary>
    /// Recognize mic buffer
    /// </summary>
    /// <param name="sampledBytes"></param>
    /// <param name="nBytes"></param>
    /// <param name="index"></param>
    /// <returns></returns>
    HRESULT Recognize(int8_t* sampledBytes, int nBytes, int index);

    std::list <WAVEHDR* >::iterator recogIt;
};


extern SPEECHRECOGNIZER_API int nSpeechRecognizer;

SPEECHRECOGNIZER_API int fnSpeechRecognizer(void);
