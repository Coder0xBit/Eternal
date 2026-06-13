#include "Viewer.h"

int main() {
    const auto app = Vortak::Viewer::create();
    app->run();
    return 0;
}
