/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsISupports.h"
#include "nsEspeakService.h"
#include "nsPrintfCString.h"
#include "nsIWeakReferenceUtils.h"
#include "SharedBuffer.h"
#include "nsISimpleEnumerator.h"

#include "mozilla/dom/nsSynthVoiceRegistry.h"
#include "mozilla/dom/nsSpeechTask.h"

#include "nsIFile.h"
#include "nsThreadUtils.h"
#include "prenv.h"

#include "mozilla/DebugOnly.h"
#include <dlfcn.h>

 // eSpeak only provides mono output.
#define ESPEAK_CHANNELS_NUM 1

// Speech rates in words per minute
#define ESPEAK_RATE_MINIMUM  80
#define ESPEAK_RATE_MAXIMUM  450
#define ESPEAK_RATE_NORMAL   175

// Default speech pitch
#define ESPEAK_PITCH_NORMAL 50

namespace mozilla {
namespace dom {

StaticRefPtr<nsEspeakService> nsEspeakService::sSingleton;

class EspeakApi
{
public:

  EspeakApi() : mInitialized(false) {}

  bool Init()
  {
    if (mInitialized) {
      return true;
    }

    void* handle = dlopen("libespeak.so", RTLD_LAZY);

    if (!handle) {
      NS_WARNING("Failed to open libespeak.so, espeak cannot run");
      return false;
    }

    espeak_Initialize =
      (int (*)(EspeakApi::espeak_AUDIO_OUTPUT, int, const char*, int))dlsym(
        handle, "espeak_Initialize");

    espeak_ListVoices =
      (EspeakApi::espeak_VOICE** (*)(EspeakApi::espeak_VOICE*))dlsym(
        handle, "espeak_ListVoices");

    espeak_SetSynthCallback =
      (void (*)(EspeakApi::t_espeak_callback*))dlsym(
        handle, "espeak_SetSynthCallback");

    espeak_SetVoiceByName =
      (EspeakApi::espeak_ERROR (*)(const char*))dlsym(
        handle, "espeak_SetVoiceByName");

    espeak_SetVoiceByProperties =
      (EspeakApi::espeak_ERROR (*)(EspeakApi::espeak_VOICE*))dlsym(
        handle, "espeak_SetVoiceByProperties");

    espeak_SetParameter =
      (EspeakApi::espeak_ERROR (*)(EspeakApi::espeak_PARAMETER, int, int))dlsym(
        handle, "espeak_SetParameter");

    espeak_Synth =
       (EspeakApi::espeak_ERROR (*)(
         const void*, size_t, unsigned int,
         EspeakApi::espeak_POSITION_TYPE, unsigned int,
         unsigned int, unsigned int*, void*))dlsym(handle, "espeak_Synth");

    mInitialized = true;
    return true;
  }

 /* Copied from speak_lib.h */
  typedef enum {
    AUDIO_OUTPUT_PLAYBACK, 
    AUDIO_OUTPUT_RETRIEVAL,
    AUDIO_OUTPUT_SYNCHRONOUS,
    AUDIO_OUTPUT_SYNCH_PLAYBACK
  } espeak_AUDIO_OUTPUT;

  typedef struct {
    const char *name;
    const char *languages;
    const char *identifier;
    unsigned char gender;
    unsigned char age;
    unsigned char variant;
    unsigned char xx1;
    int score;
    void *spare;
  } espeak_VOICE;

  typedef enum {
    espeakEVENT_LIST_TERMINATED = 0,
    espeakEVENT_WORD = 1,
    espeakEVENT_SENTENCE = 2,
    espeakEVENT_MARK = 3,
    espeakEVENT_PLAY = 4,
    espeakEVENT_END = 5,
    espeakEVENT_MSG_TERMINATED = 6,
    espeakEVENT_PHONEME = 7,
    espeakEVENT_SAMPLERATE = 8
  } espeak_EVENT_TYPE;

  typedef struct {
    espeak_EVENT_TYPE type;
    unsigned int unique_identifier;
    int text_position;
    int length;
    int audio_position;
    int sample;
    void* user_data;
    union {
      int number;
      const char *name;
      char string[8];
    } id;
  } espeak_EVENT;

  typedef enum {
    EE_OK=0,
    EE_INTERNAL_ERROR=-1,
    EE_BUFFER_FULL=1,
    EE_NOT_FOUND=2
  } espeak_ERROR;

  typedef enum {
    POS_CHARACTER = 1,
    POS_WORD,
    POS_SENTENCE
  } espeak_POSITION_TYPE;

  typedef enum {
    espeakSILENCE=0, /* internal use */
    espeakRATE=1,
    espeakVOLUME=2,
    espeakPITCH=3,
    espeakRANGE=4,
    espeakPUNCTUATION=5,
    espeakCAPITALS=6,
    espeakWORDGAP=7,
    espeakOPTIONS=8,   // reserved for misc. options.  not yet used
    espeakINTONATION=9,

    espeakRESERVED1=10,
    espeakRESERVED2=11,
    espeakEMPHASIS,   /* internal use */
    espeakLINELENGTH, /* internal use */
    espeakVOICETYPE,  // internal, 1=mbrola
    N_SPEECH_PARAM    /* last enum */
  } espeak_PARAMETER;

  typedef int (t_espeak_callback)(short*, int, espeak_EVENT*);

  int (* espeak_Initialize)(espeak_AUDIO_OUTPUT, int, const char*, int);

  espeak_VOICE** (* espeak_ListVoices)(espeak_VOICE*);

  void (* espeak_SetSynthCallback)(t_espeak_callback*);

  espeak_ERROR (* espeak_SetVoiceByName)(const char*);

  espeak_ERROR (* espeak_SetVoiceByProperties)(espeak_VOICE*);

  espeak_ERROR (* espeak_SetParameter)(espeak_PARAMETER, int, int);

  espeak_ERROR (* espeak_Synth)(const void*, size_t, unsigned int,
                                espeak_POSITION_TYPE, unsigned int,
                                unsigned int, unsigned int*, void*);

private:
  bool mInitialized;

} sEspeakApi;

class EspeakVoice
{
public:
  EspeakVoice(const nsAutoString& aName,
              const nsAutoString& aLanguage,
              const nsAutoString& aIdentifier)
    : mName(aName)
    , mLanguage(aLanguage)
    , mIdentifier(aIdentifier) {}

  NS_INLINE_DECL_REFCOUNTING(EspeakVoice)

  nsString mName;

  nsString mLanguage;

  nsString mIdentifier;

private:
  ~EspeakVoice() {}
};


class EspeakCallbackRunnable : public nsRunnable,
                               public nsISpeechTaskCallback
{
  friend class EspeakSynthDataRunnable;

public:
  EspeakCallbackRunnable(const nsAString& aText, EspeakVoice* aVoice,
                         float aRate, float aPitch, int aSampleRate,
                         nsISpeechTask* aTask, nsEspeakService* aService)
    : mText(NS_ConvertUTF16toUTF8(aText))
    , mRate(aRate)
    , mPitch(aPitch)
    , mSampleRate(aSampleRate)
    , mTask(aTask)
    , mVoice(aVoice)
    , mService(aService)
    , mCanceled(false) { }

  NS_DECL_ISUPPORTS_INHERITED
  NS_DECL_NSISPEECHTASKCALLBACK

  NS_IMETHOD Run() override;

  void DispatchSynthDataRunnable(int sSampleRate,
                                 already_AddRefed<SharedBuffer>&& aBuffer,
                                 size_t aBufferSize);

  bool IsCanceled() { return mCanceled; }

private:
  ~EspeakCallbackRunnable() { }

  nsCString mText;

  float mRate;

  float mPitch;

  int mSampleRate;

  // We use this pointer to compare it with the current service task.
  // If they differ, this runnable should stop.
  nsISpeechTask* mTask;

  // We hold a strong reference to the service, which in turn holds
  // a strong reference to this voice.
  EspeakVoice* mVoice;

  // By holding a strong reference to the service we guarantee that it won't be
  // destroyed before this runnable.
  nsRefPtr<nsEspeakService> mService;

  bool mCanceled;
};

NS_IMPL_ISUPPORTS_INHERITED(EspeakCallbackRunnable, nsRunnable, nsISpeechTaskCallback)

static int
espeak_cb(short *wav, int numsamples, EspeakApi::espeak_EVENT *events)
{
  // espeak_cb is called synchronously from EspeakCallbackRunnable::Run, so
  // it is guaranteed to still exist.
  EspeakCallbackRunnable* runnable =
    static_cast<EspeakCallbackRunnable*>(events->user_data);

  if (runnable->IsCanceled()) {
    return 1;
  }

  if (numsamples) {
    nsRefPtr<SharedBuffer> buffer = SharedBuffer::Create(numsamples * sizeof(short));
    memcpy(buffer->Data(), wav, numsamples * sizeof(short));

    runnable->DispatchSynthDataRunnable(0, buffer.forget(), numsamples);
  }


  return 0;
}

// nsRunnable

NS_IMETHODIMP
EspeakCallbackRunnable::Run()
{
  MOZ_ASSERT(!NS_IsMainThread());

  EspeakApi::espeak_VOICE props = { 0 };
  props.name = NS_ConvertUTF16toUTF8(mVoice->mName).get();
  props.identifier = NS_ConvertUTF16toUTF8(mVoice->mIdentifier).get();
  props.languages = NS_ConvertUTF16toUTF8(mVoice->mLanguage).get();

  sEspeakApi.espeak_SetVoiceByProperties(&props);
  sEspeakApi.espeak_SetSynthCallback(espeak_cb);

  int rate = (int)(mRate * ESPEAK_RATE_NORMAL);
  rate = (rate > ESPEAK_RATE_MAXIMUM) ? ESPEAK_RATE_MAXIMUM : rate;
  rate = (rate < ESPEAK_RATE_MINIMUM) ? ESPEAK_RATE_MINIMUM : rate;
  sEspeakApi.espeak_SetParameter(EspeakApi::espeakRATE, rate, 0);

  int pitch = (int)(mPitch * ESPEAK_PITCH_NORMAL);
  sEspeakApi.espeak_SetParameter(EspeakApi::espeakPITCH, pitch, 0);

  DispatchSynthDataRunnable(mSampleRate, already_AddRefed<SharedBuffer>(), 0);

  unsigned int id = 0;
  sEspeakApi.espeak_Synth(mText.get(), mText.Length() + 1, 0,
                          EspeakApi::POS_CHARACTER, 0, 0, &id, this);

  DispatchSynthDataRunnable(0, already_AddRefed<SharedBuffer>(), 0);

  return NS_OK;
}

void
EspeakCallbackRunnable::DispatchSynthDataRunnable(
  int aSampleRate, already_AddRefed<SharedBuffer>&& aBuffer, size_t aBufferSize)
{
  class EspeakSynthDataRunnable final : public nsRunnable
  {
  public:
    EspeakSynthDataRunnable(int aSampleRate,
                            already_AddRefed<SharedBuffer>& aBuffer,
                            size_t aBufferSize, EspeakCallbackRunnable* aCallback)
      : mSampleRate(aSampleRate)
      , mBuffer(aBuffer)
      , mBufferSize(aBufferSize)
      , mCallback(aCallback) {
    }

    NS_IMETHOD Run()
    {
      MOZ_ASSERT(NS_IsMainThread());

      nsISpeechTask* task = mCallback->mTask;

      if (mSampleRate) {
        return task->Setup(mCallback, ESPEAK_CHANNELS_NUM, mSampleRate, 2);
      }

      return task->SendAudioNative(
        mBufferSize ? static_cast<short*>(mBuffer->Data()) : nullptr, mBufferSize);
    }

  private:
    int mSampleRate;

    nsRefPtr<SharedBuffer> mBuffer;

    size_t mBufferSize;

    nsRefPtr<EspeakCallbackRunnable> mCallback;
  };

  nsCOMPtr<nsIRunnable> sendEvent =
    new EspeakSynthDataRunnable(aSampleRate, aBuffer, aBufferSize, this);
  NS_DispatchToMainThread(sendEvent);
}

// nsISpeechTaskCallback

NS_IMETHODIMP
EspeakCallbackRunnable::OnPause()
{
  return NS_OK;
}

NS_IMETHODIMP
EspeakCallbackRunnable::OnResume()
{
  return NS_OK;
}

NS_IMETHODIMP
EspeakCallbackRunnable::OnCancel()
{
  mCanceled = true;
  return NS_OK;
}

NS_IMETHODIMP
EspeakCallbackRunnable::OnVolumeChanged(float aVolume)
{
  return NS_OK;
}


NS_INTERFACE_MAP_BEGIN(nsEspeakService)
  NS_INTERFACE_MAP_ENTRY(nsISpeechService)
  NS_INTERFACE_MAP_ENTRY(nsIObserver)
  NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIObserver)
NS_INTERFACE_MAP_END

NS_IMPL_ADDREF(nsEspeakService)
NS_IMPL_RELEASE(nsEspeakService)

nsEspeakService::nsEspeakService()
  : mInitialized(false)
  , mVoicesMonitor("nsEspeakService::mVoices")
{
}

nsEspeakService::~nsEspeakService()
{
  // We don't worry about removing the voices because this gets
  // destructed at shutdown along with the voice registry.
  MonitorAutoLock autoLock(mVoicesMonitor);
  mVoices.Clear();

  if (mThread) {
    mThread->Shutdown();
  }
}

// nsIObserver

NS_IMETHODIMP
nsEspeakService::Observe(nsISupports* aSubject, const char* aTopic,
                       const char16_t* aData)
{
  MOZ_ASSERT(NS_IsMainThread());
  NS_ENSURE_TRUE(!strcmp(aTopic, "profile-after-change"), NS_ERROR_UNEXPECTED);

  DebugOnly<nsresult> rv = NS_NewNamedThread("Espeak Worker", getter_AddRefs(mThread));
  MOZ_ASSERT(NS_SUCCEEDED(rv));
  return mThread->Dispatch(
    NS_NewRunnableMethod(this, &nsEspeakService::Init), NS_DISPATCH_NORMAL);
}
// nsISpeechService

NS_IMETHODIMP
nsEspeakService::Speak(const nsAString& aText, const nsAString& aUri,
                       float aVolume, float aRate, float aPitch,
                       nsISpeechTask* aTask)
{
  NS_ENSURE_TRUE(mInitialized, NS_ERROR_NOT_AVAILABLE);

  MonitorAutoLock autoLock(mVoicesMonitor);
  bool found = false;
  EspeakVoice* voice = mVoices.GetWeak(aUri, &found);
  NS_ENSURE_TRUE(found, NS_ERROR_NOT_AVAILABLE);

  nsRefPtr<EspeakCallbackRunnable> cb = new EspeakCallbackRunnable(
    aText, voice, aRate, aPitch, mSampleRate, aTask, this);
  return mThread->Dispatch(cb, NS_DISPATCH_NORMAL);
}

NS_IMETHODIMP
nsEspeakService::GetServiceType(SpeechServiceType* aServiceType)
{
  *aServiceType = nsISpeechService::SERVICETYPE_DIRECT_AUDIO;
  return NS_OK;
}

struct VoiceTraverserData
{
  nsEspeakService* mService;
  nsSynthVoiceRegistry* mRegistry;
};

// private methods

static PLDHashOperator
EspeakAddVoiceTraverser(const nsAString& aUri,
                        nsRefPtr<EspeakVoice>& aVoice,
                        void* aUserArg)
{
  VoiceTraverserData* data = static_cast<VoiceTraverserData*>(aUserArg);

  nsAutoString name;
  name.AssignLiteral("eSpeak ");
  name.Append(aVoice->mName);

  DebugOnly<nsresult> rv =
    data->mRegistry->AddVoice(
      data->mService, aUri, name, aVoice->mLanguage, true, false);
  NS_WARN_IF_FALSE(NS_SUCCEEDED(rv), "Failed to add voice");

  return PL_DHASH_NEXT;
}

void
nsEspeakService::Init()
{
  MOZ_ASSERT(!NS_IsMainThread());
  MOZ_ASSERT(!mInitialized);

  sEspeakApi.Init();

  mSampleRate = sEspeakApi.espeak_Initialize(
    EspeakApi::AUDIO_OUTPUT_SYNCHRONOUS, 512, nullptr, 0);

  MonitorAutoLock autoLock(mVoicesMonitor);

  mVoices.Clear();
  EspeakApi::espeak_VOICE** voices = sEspeakApi.espeak_ListVoices(nullptr);
  const EspeakApi::espeak_VOICE *v;

  for(int i=0; (v = voices[i]) != nullptr; i++)
  {
    NS_ConvertUTF8toUTF16 voiceName(v->name);
    NS_ConvertUTF8toUTF16 voiceId(v->identifier);

    const char *p = v->languages;
    while(*p != 0)
    {
      int len = strlen(p+1);
      NS_ConvertUTF8toUTF16 voiceLang(p+1);

      nsAutoString uri;
      uri.AssignLiteral("urn:moz-tts:espeak:");
      uri.Append(voiceId);
      uri.AppendLiteral("?");
      uri.Append(voiceLang);

      nsRefPtr<EspeakVoice> voice =
        new EspeakVoice(voiceName, voiceLang, voiceId);

      mVoices.Put(uri, voice);
      p += len+2;
    }
  }

  NS_DispatchToMainThread(NS_NewRunnableMethod(this, &nsEspeakService::RegisterVoices));
}

void
nsEspeakService::RegisterVoices()
{
  VoiceTraverserData data = { this, nsSynthVoiceRegistry::GetInstance() };
  mVoices.Enumerate(EspeakAddVoiceTraverser, &data);

  mInitialized = true;
}

// static methods

nsEspeakService*
nsEspeakService::GetInstance()
{
  MOZ_ASSERT(NS_IsMainThread());
  if (XRE_GetProcessType() != GeckoProcessType_Default) {
    MOZ_ASSERT(false, "nsEspeakService can only be started on main gecko process");
    return nullptr;
  }

  if (!sSingleton) {
    sSingleton = new nsEspeakService();
  }

  return sSingleton;
}

already_AddRefed<nsEspeakService>
nsEspeakService::GetInstanceForService()
{
  nsRefPtr<nsEspeakService> espeakService = GetInstance();
  return espeakService.forget();
}

void
nsEspeakService::Shutdown()
{
  if (!sSingleton) {
    return;
  }

  sSingleton = nullptr;
}

} // namespace dom
} // namespace mozilla
