/* Copyright (C) 2014 Daniel Dressler and contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#pragma once

#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/error.h>
#include <avahi-common/thread-watch.h>

typedef struct dnssd_s {
  AvahiThreadedPoll *DNSSDMaster;
  AvahiClient       *DNSSDClient;
  AvahiEntryGroup   *ipp_ref;
} dnssd_t;

int dnssd_init();
void dnssd_shutdown();
int dnssd_register();
void dnssd_unregister();
