/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/ModuleUtils.h"
#include "nsIClassInfoImpl.h"

#ifdef MOZ_WEBRTC

#include "nsEspeakService.h"

using namespace mozilla::dom;

#define ESPEAKSERVICE_CID \
  {0x28dc56cb, 0xbbee, 0x4e88, {0xbe, 0xae, 0x98, 0xd2, 0xe3, 0xfd, 0xe8, 0x0e}}

#define ESPEAKSERVICE_CONTRACTID "@mozilla.org/synthespeak;1"

// Defines nsEspeakServiceConstructor
NS_GENERIC_FACTORY_SINGLETON_CONSTRUCTOR(nsEspeakService,
                                         nsEspeakService::GetInstanceForService)

// Defines kESPEAKSERVICE_CID
NS_DEFINE_NAMED_CID(ESPEAKSERVICE_CID);

static const mozilla::Module::CIDEntry kCIDs[] = {
  { &kESPEAKSERVICE_CID, true, nullptr, nsEspeakServiceConstructor },
  { nullptr }
};

static const mozilla::Module::ContractIDEntry kContracts[] = {
  { ESPEAKSERVICE_CONTRACTID, &kESPEAKSERVICE_CID },
  { nullptr }
};

static const mozilla::Module::CategoryEntry kCategories[] = {
  { "profile-after-change", "Espeak Speech Synth", ESPEAKSERVICE_CONTRACTID },
  { nullptr }
};

static void
UnloadEspeakModule()
{
  nsEspeakService::Shutdown();
}

static const mozilla::Module kModule = {
  mozilla::Module::kVersion,
  kCIDs,
  kContracts,
  kCategories,
  nullptr,
  nullptr,
  UnloadEspeakModule
};

NSMODULE_DEFN(synthespeak) = &kModule;
#endif
