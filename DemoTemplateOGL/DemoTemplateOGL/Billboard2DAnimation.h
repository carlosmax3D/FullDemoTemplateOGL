#ifndef _billb2DAnimation
#define _billb2DAnimation
#include "Base/Utilities.h"
#include "Billboard2D.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Base/glext.h"
#include "Base/wglext.h"

class Billboard2DAnimation {
private:
    int current = 0;
    std::vector<Billboard2D*> frames;
public:
    Billboard2DAnimation() {}
    ~Billboard2DAnimation() {
        for (Billboard2D* frame : frames) {
            delete frame;
        }
    }

    void pushFrame(Billboard2D* frame) {
        frames.emplace_back(frame);
    }

    void nextAnimation() {
        current++;
        if (current >= frames.size()) {
            current = 0; // Loop back to the first frame  
        }
    }
    void Draw() {
        if (!frames.empty()) {
            frames.at(current)->Draw();
        }
    }
};
#endif