#pragma once

#include "Globals.h"

using namespace event_management;

class event_manager
{
private:
    class Channel
    {
    private:
        std::mutex mutex;//locks everything

        std::unordered_map<Event_type, std::vector<Event_receiver_weak>> subscribers;

        std::queue<std::unique_ptr<Event>> event_queue;
        std::unique_ptr<Event> immediate_event;

        std::thread worker;

        std::condition_variable signal;
        bool running = true;

        void run() {
            while (running) {
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    signal.wait(lock, [&]
                    {
                        return  !running || immediate_event != nullptr || !event_queue.empty();
                    });

                    if (!running)
                        break;


                    //TODO: dont forget to check if receiver is alive before directing event!
                    //TODO: also delete it from subscribed list.

                    if (immediate_event != nullptr )
                    {
                        //TODO: direct the event

                        //Don't forget to move the event if needed before reset, or it will be destroyed!
                        immediate_event.reset();
                    }
                    else
                    {
                        while (!event_queue.empty())
                        {
                            std::unique_ptr<Event> event = std::move(event_queue.front());
                            event_queue.pop();

                            //TODO: direct the event
                        }
                    }

                }

            }
        }

    public:

        Channel() {
            worker = std::thread(&Channel::run, this);
        }

        ~Channel() {
            {
                std::lock_guard<std::mutex> lock(mutex);
                running = false;
            }
            signal.notify_one();
            worker.join();
        }

        // Subscribe an event_receiver to an event ID
        void subscribe(const Event_type event_type,const Event_receiver_shared& receiver)
        {
            std::lock_guard<std::mutex> lock(mutex);
            subscribers[event_type].push_back(receiver);
        }

        void throw_event(std::unique_ptr<Event> event)
        {
            std::unique_lock<std::mutex> lock(mutex);
            if(event->timing == Event_timing::Immediate)
            {
                immediate_event = std::move(event);
                lock.unlock();//need to unlock so thread can lock it and work

                signal.notify_one();
            }
            else
            {
                event_queue.push(std::move(event));
            }
        }

        void tick()
        {
            signal.notify_one();
        }

    };

    std::vector<Channel> channels;
public:

};