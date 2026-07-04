/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "rsiidletime.h"

RSIIdleTime::~RSIIdleTime()
{
    if (notif_)
        ext_idle_notification_v1_destroy(notif_);
    if (idle_notifier_)
        ext_idle_notifier_v1_destroy(idle_notifier_);
    if (seat_)
        wl_seat_destroy(seat_);
    if (registry_)
        wl_registry_destroy(registry_);
    if (display_)
        wl_display_disconnect(display_);
}

RSIIdleTime::RSIIdleTime()
{
    display_ = wl_display_connect(nullptr);
    if (!display_) {
        std::fprintf(stderr, "failed to connect to Wayland display\n");
    }

    registry_ = wl_display_get_registry(display_);
    wl_registry_add_listener(registry_, &registry_listener_, this);
    wl_display_roundtrip(display_);

    if (!idle_notifier_ || !seat_) {
        std::fprintf(stderr, "compositor lacks ext-idle-notify-v1 or wl_seat\n");
    }

    notif_ = ext_idle_notifier_v1_get_idle_notification(idle_notifier_, 0, seat_);
    ext_idle_notification_v1_add_listener(notif_, &idle_listener_, this);
    wl_display_roundtrip(display_);

    fd_ = wl_display_get_fd(display_);
}

double RSIIdleTime::getIdleTime()
{
    wl_display_flush(display_);

    pollfd pfd{fd_, POLLIN, 0};
    poll(&pfd, 1, 0); // non-blocking check

    if (pfd.revents & POLLIN)
        wl_display_dispatch(display_);
    else
        wl_display_dispatch_pending(display_);

    if (!is_idle_)
        return 0.0;
    return std::chrono::duration<double, std::milli>(Clock::now() - idle_since_).count();
}

void RSIIdleTime::registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t)
{
    auto *self = static_cast<RSIIdleTime *>(data);
    if (!strcmp(interface, ext_idle_notifier_v1_interface.name))
        self->idle_notifier_ = static_cast<ext_idle_notifier_v1 *>(wl_registry_bind(registry, name, &ext_idle_notifier_v1_interface, 1));
    else if (!strcmp(interface, wl_seat_interface.name))
        self->seat_ = static_cast<wl_seat *>(wl_registry_bind(registry, name, &wl_seat_interface, 1));
}

void RSIIdleTime::on_idled(void *data, ext_idle_notification_v1 *)
{
    auto *self = static_cast<RSIIdleTime *>(data);
    self->is_idle_ = true;
    self->idle_since_ = Clock::now();
}

void RSIIdleTime::on_resumed(void *data, ext_idle_notification_v1 *)
{
    static_cast<RSIIdleTime *>(data)->is_idle_ = false;
}

int RSIIdleTimeFake::getIdleTime() const
{
    return m_idleTime;
}

void RSIIdleTimeFake::setIdleTime(const int _idleTime)
{
    m_idleTime = _idleTime;
}
