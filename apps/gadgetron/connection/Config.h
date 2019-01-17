#pragma once

#include <string>
#include <unordered_map>
#include <boost/variant.hpp>
#include <boost/optional.hpp>

namespace Gadgetron::Server::Connection {

    struct Config {

        struct Gadget;
        struct Parallel;
        struct Distributed;
        using Node = boost::variant<Gadget, Parallel, Distributed>;

        template<class CONFIG>
        static std::string name(CONFIG config) {
            return config.name.empty() ? config.classname : config.name;
        }

        struct Reader {
            std::string dll, classname;
            boost::optional<uint16_t> slot;
        };

        struct Writer {
            std::string dll, classname;
        };

        struct Stream {
            std::string name;
            std::vector<Node> nodes;
        };

        struct RepeatedStream : Stream {
        };

        struct Gadget {
            std::string name, dll, classname;
            std::unordered_map<std::string, std::string> properties;
        };

        struct Branch : Gadget {};
        struct Merge : Gadget {};

        struct Parallel {
            Branch branch;
            Merge merge;
            std::vector<Stream> streams;
        };

        struct Discovery {};

        struct Distributed {
            Discovery discovery;
            std::vector<Reader> readers;
            std::vector<Writer> writers;
            std::vector<Stream> streams;
            std::vector<RepeatedStream> repeated_streams;

        };

        std::vector<Reader> readers;
        std::vector<Writer> writers;
        Stream stream;
    };

    Config parse_config(std::istream &stream);
}