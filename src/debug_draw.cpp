#include "debug_draw.h"

void DebugDraw::init() {
  glGenBuffers(1, &vbo);
}

void DebugDraw::addLine(glm::vec3 start, glm::vec3 end, int lifetime) {
  DebugLine line;
  line.start = start;
  line.end = end;
  lines.push_back(line);

  DebugCommon common;
  common.lifeTime = lifetime;
  commonData.push_back(common);
}

void DebugDraw::draw() {
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(GLfloat)*3*2, &lines[0], GL_STREAM_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glDrawArrays(GL_LINES, 0, lines.size()*2);

  for (std::vector<DebugCommon>::iterator it = commonData.begin(); it != commonData.end(); ) {
    it->lifeTime -= 1;
    if (it->lifeTime <= 0) {
      int index = it - commonData.begin();
      it = commonData.erase(it);
      lines.erase(lines.begin() + index);
    } else {
      ++it;
    }
  }
}
