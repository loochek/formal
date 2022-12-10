#include <string>
#include <libformal/utils.hpp>

namespace formal {
    std::string_view strip(const std::string_view& str) {
        if (str.empty()) {
            return str;
        }

        auto start_it = str.begin();
        auto end_it = str.rbegin();

        while (std::isspace(*start_it)) {
            start_it++;
            if (start_it == str.end()) {
                break;
            }
        }

        while (std::isspace(*end_it)) {
            end_it++;
            if (end_it == str.rend()) {
                break;
            }
        }

        size_t length = end_it.base() - start_it;
        return length > 0 ? std::string_view(start_it, length) : std::string_view(str.data(), 0);
    }
}
