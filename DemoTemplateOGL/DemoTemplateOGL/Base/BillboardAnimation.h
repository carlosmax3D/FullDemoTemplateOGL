#ifndef _billbAnimation
#define _billbAnimation
#include "Utilities.h"
#include "Billboard.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glext.h"
#include "wglext.h"

class BillboardAnimation {
private:
	int current = 0;
    std::vector<Billboard*> frames;
public:
	BillboardAnimation(){}
	~BillboardAnimation(){
        for (int i = 0; i < frames.size(); i++){
            delete frames.at(i);
        }
    }

    void pushFrame(Billboard* frame){
        frames.emplace_back(frame);
    }

    void nextAnimation(){
        current++;
        if (current >= frames.size()){
            current = 0;
        }
    }
	void Draw(){
        if (!frames.empty()) {
            frames.at(current)->Draw();
        }
    }
};

#endif 