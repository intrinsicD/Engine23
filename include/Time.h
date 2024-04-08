//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_TIME_H
#define ENGINE23_TIME_H

#include <chrono>
#include <string>

namespace Bcg {
    template<typename T>
    struct DurationTypeToString;

    struct Time {
        struct Unit {
            using nano = std::nano;
            using micro = std::micro;
            using milli = std::milli;
            using seconds = std::ratio<1>;
            using minutes = std::ratio<60>;
            using hours = std::ratio<3600>;
        };

        struct Point {
            static Point Now() {
                return {std::chrono::high_resolution_clock::now()};
            }

            template<typename Duration = Unit::milli>
            double duration(const Point &other) {
                return std::chrono::duration<double, Duration>(value - other.value).count();
            }

            template<typename Duration = Unit::milli>
            std::string unit_string() const {
                return DurationTypeToString<Duration>::value();
            }

            std::chrono::time_point<std::chrono::high_resolution_clock> value;
        };

        Point engine_run_start;
        Point engine_run_end;
        Point engine_constructor_start;
        Point engine_constructor_end;

        struct Mainloop {
            Point current;
            Point last;
            double avg_duration = 0;
            double duration = 0;
            size_t iter_counter = 0;
            int fps = 0;
            int avg_fps = 0;
        } mainloop;

        struct SimulationLoop {
            Point start;
            double avg_duration = 0;
            double duration = 0;
            size_t iter_counter = 0;
            double min_step_size = 1.0 / 30000;
            double accumulator = 0;
        } simulationloop;
    };

    template<>
    struct DurationTypeToString<Time::Unit::nano> {
        static std::string value() { return " ns"; }
    };

    template<>
    struct DurationTypeToString<Time::Unit::micro> {
        static std::string value() { return " µs"; }
    };

    template<>
    struct DurationTypeToString<Time::Unit::milli> {
        static std::string value() { return " ms"; }
    };

    template<>
    struct DurationTypeToString<Time::Unit::seconds> {
        static std::string value() { return "  s"; }
    };

    template<>
    struct DurationTypeToString<Time::Unit::minutes> {
        static std::string value() { return "min"; }
    };

    template<>
    struct DurationTypeToString<Time::Unit::hours> {
        static std::string value() { return "  h"; }
    };
}
#endif //ENGINE23_TIME_H
