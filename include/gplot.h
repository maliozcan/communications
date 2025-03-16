#ifndef INCLUDE_GPLOT_HPP
#define INCLUDE_GPLOT_HPP

#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
#include <map>

class gplot {
public:
    enum class type {
        normal,
        semilogy,
    };

    gplot() : gplot(type::normal) {
    }

    explicit gplot(type t) {
        if (_gplot == nullptr) {
            fprintf(stderr, "Failed to run Gnuplot\n");
            return;
        }
        switch (t) {
            case type::normal:
                _command.append(_normal);
                break;
            case type::semilogy:
                _command.append(_semilogy);
                break;
            default:
                break;
        }
    }

    gplot(const gplot&) = delete;
    gplot& operator=(const gplot&) = delete;
    gplot(gplot&&) = delete;
    gplot& operator=(gplot&&) = delete;

    ~gplot() {
        if (_gplot) {
            pclose(_gplot);
        }
    }

    template<typename T, typename U>
    bool add_2D_data(const std::string& title, const std::vector<std::pair<T, U>>& data) {
        if (_gplot == nullptr) {
            return false;
        }

        if (!_stream.empty()) {
            _stream[title] << "e\n"; // 34.1.7 Special-filenames, http://www.gnuplot.info/docs_4.2/gnuplot.html#x1-13200034.1.7
        }
        return _write_to_file(_stream[title], data) == result::success;
    }

    bool plot() {
        if (_gplot == nullptr) {
            return false;
        }
        if (_stream.empty()) {
            return false;
        }
        int32_t counter = 0;
        for (const auto& [title, _] : _stream) {
            if (counter > 0) {
                _command += ',';
            }
            _command += " '-' with lines title \"" + title + "\"";
            ++counter;
        }
        _command += '\n';
        for (const auto& [_, data] : _stream) {
            _command += data.str();
        }
        fprintf(_gplot, "%s\n", _command.c_str());
        // retrieve command for related type
        _stream.clear();
        return true;
    }

private:
    enum class result {
        success,
        failure
    };

    template<typename T, typename U>
    static inline
    result _write_to_file(std::ostream& ostream, const std::vector<std::pair<T, U>>& data, const char* delimiter = "\n") {
        constexpr std::size_t maximum_line_size = 1024;
        std::array<char, maximum_line_size> buf;
        for (const auto& elem : data) {
            std::streamsize size = 0;
            if constexpr (std::is_same_v<T, U> && std::is_same_v<T, double>) {
                size = std::snprintf(buf.data(), buf.size(), "%f,%f%s", elem.first, elem.second, delimiter);
            } else {
                size = std::snprintf(buf.data(), buf.size(), "%d,%d%s", elem.first, elem.second, delimiter);
            }
            ostream.write(buf.data(), size);
            if (!ostream.good()) {
                return result::failure;
            }
        }
        return result::success;
    }

    static constexpr auto _normal = "\nset datafile separator ','\n"
                                      "set grid\n"
                                      "plot";

    static constexpr auto _semilogy = "\nset datafile separator ','\n"
                                      "set format y \"10^{%L}\"\n"
                                      "set logscale y\n"
                                      "set grid\n"
                                      "plot";

    FILE* _gplot{popen("gnuplot --persist", "w")};
    std::string _command{};
    std::map<std::string, std::stringstream, std::less<>> _stream{};
};

#endif // INCLUDE_GPLOT_HPP
