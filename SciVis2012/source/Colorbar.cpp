#include "Colorbar.h"

Colorbar::Colorbar()
{
    colorMode = Visualization::Rainbow;
    texture = 0;
    title = "Colorbar";
}

Colorbar::~Colorbar()
{
    if (texture)
        glDeleteTextures(1, &texture);
}

void Colorbar::setup()
{
    if (texture)
        glDeleteTextures(1, &texture);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    const int color_width = 256;
    GLubyte data[color_width][3];
    fillData(data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, color_width, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void Colorbar::render()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_1D, texture);
    glColor4f(1, 1, 1, 0.8f);
    glTranslatef(700.0f, 100.0f, 0.0f);
    GLfloat px = 0;
    GLfloat py = 0;
    GLfloat sx = 10;
    GLfloat sy = 100;
    glBegin(GL_POLYGON);
        glTexCoord2f(1, 0); glVertex2f(px, py);
        glTexCoord2f(1, 0); glVertex2f(px+sx, py);
        glTexCoord2f(0, 0); glVertex2f(px+sx, py-sy);
        glTexCoord2f(0, 0); glVertex2f(px, py-sy);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Colorbar::fillData(GLubyte (&data)[256][3])
{    
    for (size_t i = 0; i != 256; i++)
    {
        data[i][0] = i;
        data[i][1] = i;
        data[i][2] = i;
    }
}