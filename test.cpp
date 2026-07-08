#include <socket_helper.hpp>

using namespace socket_helper;

int main(void) {
    std::cout << socket_helper::communication_style_to_string_map[communication_style::stream];
    return 0;
}
