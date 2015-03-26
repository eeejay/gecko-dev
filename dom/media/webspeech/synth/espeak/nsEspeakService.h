/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsEspeakService_h
#define nsEspeakService_h

#include "mozilla/Mutex.h"
#include "nsAutoPtr.h"
#include "nsTArray.h"
#include "nsIObserver.h"
#include "nsIThread.h"
#include "nsISpeechService.h"
#include "nsRefPtrHashtable.h"
#include "mozilla/StaticPtr.h"
#include "mozilla/Monitor.h"

namespace mozilla {
namespace dom {

class EspeakVoice;
class EspeakCallbackRunnable;

typedef void* espeak_System;
typedef void* espeak_Resource;
typedef void* espeak_Engine;

class nsEspeakService : public nsIObserver,
                        public nsISpeechService
{
  friend class EspeakCallbackRunnable;
  friend class EspeakInitRunnable;

public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSISPEECHSERVICE
  NS_DECL_NSIOBSERVER

  nsEspeakService();

  static nsEspeakService* GetInstance();

  static already_AddRefed<nsEspeakService> GetInstanceForService();

  static void Shutdown();

private:

  virtual ~nsEspeakService();

  void Init();

  void RegisterVoices();

  EspeakVoice* CurrentVoice();

  bool mInitialized;

  int mSampleRate;

  nsCOMPtr<nsIThread> mThread;

  nsRefPtrHashtable<nsStringHashKey, EspeakVoice> mVoices;

  Monitor mVoicesMonitor;

  EspeakVoice* mCurrentVoice;

  Atomic<nsISpeechTask*> mCurrentTask;

  static StaticRefPtr<nsEspeakService> sSingleton;
};

} // namespace dom
} // namespace mozilla

#endif
