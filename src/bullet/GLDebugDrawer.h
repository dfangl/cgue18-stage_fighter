//
// Created by Raphael on 25.03.2018.
//

#ifndef STAGE_FIGHTER_GLDEBUGDRAWER_H
#define STAGE_FIGHTER_GLDEBUGDRAWER_H

#include <string>
#include <vector>

#include <glm/glm.hpp>


#include "LinearMath/btIDebugDraw.h"
#include "../helper/Logger.h"
#include "../object3d/Object3D.h"
#include "../manager/ShaderManager.h"

class GLDebugDrawer : public btIDebugDraw, public Logger, public Object3D {

private:
    int debugMode;
    GLuint colorVBO;

    struct Line {
        glm::vec3 from;
        glm::vec3 to;

        Line(glm::vec3 _from, glm::vec3 _to) {
            from = _from;
            to = _to;
        }
    };

    std::vector<Line> lines;
    std::vector<glm::vec3> colors;

public:
    GLDebugDrawer();
    virtual ~GLDebugDrawer();

    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;

    void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime,
                          const btVector3 &color) override;

    void reportErrorWarning(const char *warningString) override;

    void draw3dText(const btVector3 &location, const char *textString) override;

    void setDebugMode(int debugMode) override;

    int getDebugMode() const override;

    void draw() override;

};


#endif //STAGE_FIGHTER_GLDEBUGDRAWER_H
