/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef RSIBREAK_RSIIDLETIME_H
#define RSIBREAK_RSIIDLETIME_H

#include "wayland-ext-idle-notify-v1-client-protocol.h"
#include <wayland-client.h>

#include <chrono>
#include <cstdio>
#include <cstring>
#include <poll.h>

class RSIIdleTime
{
public:
    ~RSIIdleTime();
    RSIIdleTime();

    double getIdleTime();

private:
    using Clock = std::chrono::steady_clock;

    static void registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
    static void on_idled(void *data, ext_idle_notification_v1 *notif);
    static void on_resumed(void *data, ext_idle_notification_v1 *notif);

    wl_display *display_ = nullptr;
    wl_registry *registry_ = nullptr;
    wl_seat *seat_ = nullptr;
    ext_idle_notifier_v1 *idle_notifier_ = nullptr;
    ext_idle_notification_v1 *notif_ = nullptr;
    int fd_ = -1;

    bool is_idle_ = false;
    Clock::time_point idle_since_;

    const wl_registry_listener registry_listener_ = {
        .global = registry_global,
        .global_remove = [](void *, wl_registry *, uint32_t) { },
    };
    const ext_idle_notification_v1_listener idle_listener_ = {
        .idled = on_idled,
        .resumed = on_resumed,
    };
};

class RSIIdleTimeFake : public RSIIdleTime
{
private:
    int m_idleTime = 0;

public:
    ~RSIIdleTimeFake() = default;
    int getIdleTime() const;
    void setIdleTime(const int _idleTime);
};

#endif // RSIBREAK_RSIIDLETIME_H
