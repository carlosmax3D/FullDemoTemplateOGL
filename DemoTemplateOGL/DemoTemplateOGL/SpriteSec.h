#ifndef _animacionsp
#define _animacionsp
#include "Base/Utilities.h"
#include "Base/Billboard.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Base/glext.h"
#include "Base/wglext.h"

class animacionsp {
private:
    int current = 0;
    std::vector<Billboard*> frames;
public:
    animacionsp() {}
    ~animacionsp() {
        for (int i = 0; i < frames.size(); i++) {
            delete frames.at(i);
        }
    }

    void pushFrame(Billboard* frame) {
        frames.emplace_back(frame);
    }

    void nextAnimation() {
        current++;
        if (current >= frames.size()) {
            current = 0;
        }
    }
    void Draw() {
        if (!frames.empty()) {
            frames.at(current)->Draw();
        }
    }
};

#endif 