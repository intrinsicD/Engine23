//
// Created by alex on 24.11.23.
//

#ifndef ENGINE23_EVENTS_H
#define ENGINE23_EVENTS_H

namespace Bcg::Events{
    template<typename T>
    struct Startup{

    };

    template<typename T>
    struct Shutdown{

    };

    template<typename T>
    struct Update{

    };

    template<typename T>
    struct Render{

    };

    template<typename T>
    struct Begin{

    };

    template<typename T>
    struct End{

    };

    template<typename T>
    struct Notify{

    };

    template<typename T>
    struct Change{

    };

    template<typename T>
    struct Load{

    };

    template<typename T>
    struct Save{

    };

    template<typename T>
    struct Create{

    };

    template<typename T>
    struct Destroy{

    };
}

#endif //ENGINE23_EVENTS_H
