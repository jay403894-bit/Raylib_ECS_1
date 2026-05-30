#pragma once
#include <memory>
#include <functional>
#include <vector>
#include <string>
/// <Listener>
/// a listener for Event
/// </Listener>
class Listener {
public:
    virtual ~Listener() = default;
    virtual void on_event_triggered() = 0;
};

class Event {
public:
    Event() = default;  // Constructor

    virtual Event& operator=(const Event& other) = delete; // Prevent copying
    Event(const Event& other) = delete; // Prevent copying

    virtual ~Event() = default;  // Virtual destructor

    // Subscribe a listener to the event
    virtual void Subscribe(std::shared_ptr<Listener> listener);

    // Set a callback to be triggered when the event is notified
    virtual void SetCallback(std::function<void()> callback);

    // Notify all listeners and trigger the callback if defined
    void Notify() const;

    // Abstract method for getting the event type (each event will provide its own type)
    virtual std::string get_event_type() const = 0;

private:
    std::vector<std::shared_ptr<Listener>> listeners_;  // Pool of listeners
    std::function<void()> callback_;  // A callback function to be executed on event trigger
};