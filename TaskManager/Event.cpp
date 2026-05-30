#pragma once
#include "Event.h"

    // Subscribe a listener to the event
    void Event::Subscribe(std::shared_ptr<Listener> listener) {
        listeners_.emplace_back(std::move(listener));  // Add the listener to the list
    }

    // Set a callback to be triggered when the event is notified
    void Event::SetCallback(std::function<void()> callback) {
        callback_ = std::move(callback);
    }

    // Notify all listeners and trigger the callback if defined
    void Event::Notify() const {
        // Create a local copy of the listener pool to avoid modification during iteration
       // auto listeners_copy = listeners_.get_pool();  // This now works with a const reference

        for (const auto& listener : listeners_) {
            listener.get()->on_event_triggered();  // Notify each listener
        }

        // If a callback is defined, Execute it
        if (callback_) {
            callback_();  // Execute the callback function
        }
    }
