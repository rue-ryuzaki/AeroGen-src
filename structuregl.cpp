﻿#include "structuregl.h"

#include <QCoreApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <qmath.h>
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif // __APPLE__

void clearList(GLuint str)
{
    // проверяем факт существования дисплейного списка с номером, хранимым в ListNom
    if (glIsList(str) == GL_TRUE) {
        // удаляем его в случае существования
        glDeleteLists(str, 1);
    }
}

namespace aerogen {
StructureGL::StructureGL(QWidget* parent)
    : QGLWidget(parent),
      QOpenGLFunctions(),
      gen(nullptr),
      needInit(0),
      drawGL(true),
      showAxes(false),
      showBorders(false),
      params(),
      m_strDLA(1),
      m_border(2),
      m_initialized(false),
      m_shadersSupports(false),
      m_shader(0),
      m_loaded(true),
      m_alpha(),
      m_theta(),
      m_cameraDistance(),
      m_pressPos(),
      m_info()
{
    QGLFormat glFormat = QGLWidget::format();
    qDebug() << QString("OpenGL Version = %1.%2")
                .arg(glFormat.majorVersion())
                .arg(glFormat.minorVersion());
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);
    if (!glFormat.sampleBuffers()) {
        qWarning() << "Could not enable sample buffers";
    }
    qDebug() << "OpenGL context valid =" << context()->isValid();
    
    m_cameraDistance = 10.0f;

    m_alpha = float(M_PI / 6.0f);
    m_theta = float(M_PI / 6.0f);
}

StructureGL::~StructureGL()
{
    makeCurrent();
    glDeleteLists(m_strDLA, 1);
    glDeleteLists(m_border, 1);
    delete gen;
    gen = nullptr;
    doneCurrent();
}

void StructureGL::setCamera(float d)
{
    if (d > 1.0f) {
        m_cameraDistance = d;
    }
    update();
}

int32_t StructureGL::shadersStatus() const
{
    return m_shader;
}

void StructureGL::enableShader(int32_t value)
{
    if (value != 0) {
        if (m_shadersSupports && isSupported(value - 1)) {
            m_shader = value;
            glUseProgram(m_info[m_shader - 1].program.programId());
        } else {
            m_shader = 0;
        }
    } else {
        if (m_shader != 0) {
            glUseProgram(0);
        }
        m_shader = 0;
    }
    update();
}

bool StructureGL::isSupported(int32_t i) const
{
    return !(i < 0 || i == 6 || i == 7 || i == 8 || i == 10 || i >= 20);
}

void StructureGL::restruct(bool updateStr)
{
    if (!m_loaded) {
        return;
    }
    m_loaded = false;
    if (gen) {
        make(gen->field(), updateStr);
        update();
    }
    m_loaded = true;
}

bool StructureGL::supportShaders() const
{
    return m_shadersSupports;
}

bool StructureGL::isInitialized() const
{
    return m_initialized;
}

void StructureGL::initializeGL()
{
    initializeOpenGLFunctions();
//    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    float ambientLight[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //static const GLfloat lightPos[4] = { 5.0f, 5.0f, 10.0f, 1.0f };
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, m_lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_shadersSupports = checkShaders();
    m_initialized = true;
    if (m_shadersSupports) {
        //initShaders();
        if (needInit != 0) {
            enableShader(needInit);
        } else {
            glUseProgram(0);
        }
    }
    qDebug("version: %s\n", glGetString(GL_VERSION));
}

void StructureGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (drawGL) {
        //glEnable(GL_CULL_FACE);
        //glClearDepth(1.0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        m_eyePos[0] = float(m_cameraDistance * cos(m_alpha) * sin(m_theta));
        m_eyePos[1] = float(m_cameraDistance * sin(m_alpha));
        m_eyePos[2] = float(m_cameraDistance * cos(m_alpha) * cos(m_theta));

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(90.0, 1.0, 0.01, height());
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //glDepthFunc(GL_LEQUAL);

        gluLookAt(m_eyePos[0], m_eyePos[1], m_eyePos[2],
                  0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0);
        glLightfv(GL_LIGHT0, GL_POSITION, m_lightPos);

        /*GLUquadricObj* quadric = gluNewQuadric();
        glColor4ub (176, 50, 153, 115);
        gluQuadricDrawStyle(quadric, (GLenum)GLU_SMOOTH);
        gluSphere(quadric, 1, 40, 40);
        glTranslatef(0, -1, 0);*/
        //drawingTeapot();

        //рисуем статичные оси координат
        //m_program->bind();
        //GLfloat colorsAxes[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        //m_program->setUniformValueArray(m_program[shader - 1].uniformLocation("inColor"), colorsAxes, 1, 4);
        if (showAxes) {
            glDisable(GL_LIGHTING);
            glBegin(GL_LINES);
            glColor3ub(0, 0, 0);
            glVertex3f(-300.0f,    0.0f,    0.0f);
            glVertex3f( 300.0f,    0.0f,    0.0f);
            glVertex3f(   0.0f, -300.0f,    0.0f);
            glVertex3f(   0.0f,  300.0f,    0.0f);
            glVertex3f(   0.0f,    0.0f, -300.0f);
            glVertex3f(   0.0f,    0.0f,  300.0f);
            glEnd();
            //подписи к ним
            glColor3ub(0, 0, 0);
            renderText(270.0,   0.0,   0.0, "X");
            renderText(  0.0, 270.0,   0.0, "Y");
            renderText(  0.0,   0.0, 270.0, "Z");
            glEnable(GL_LIGHTING);
        }

        if (m_shader == 0) {
            glCallList(m_border);
            glCallList(m_strDLA);
        } else {
            glCallList(m_border);
            QOpenGLShaderProgram& program = m_info[m_shader - 1].program;
            program.bind();
            program.setUniformValueArray(program.uniformLocation("lightPos"), m_lightPos, 1, 4);
            program.setUniformValueArray(program.uniformLocation("eyePos"),   m_eyePos, 1, 4);
            program.setUniformValueArray(program.uniformLocation("inColor"),  colors, 1, 4);
            //glUniform4fv(m_colorAttr, 1, colors);
            switch (m_shader) {
                case 1 ://lambert
                    break;
                case 2 ://wrap
                    program.setUniformValue(program.uniformLocation("factor"), params.wrap_factor);
                    break;
                case 3 ://phong
                    program.setUniformValueArray(program.uniformLocation("specColor"), params.specColor, 1, 4);
                    program.setUniformValue(program.uniformLocation("specPower"), params.specPower);
                    break;
                case 4 ://blinn
                    program.setUniformValueArray(program.uniformLocation("specColor"), params.specColor, 1, 4);
                    program.setUniformValue(program.uniformLocation("specPower"), params.specPower);
                    break;
                case 5 ://iso-ward
                    program.setUniformValueArray(program.uniformLocation("specColor"), params.specColor, 1, 4);
                    program.setUniformValue(program.uniformLocation("k"), params.iso_ward_k);
                    break;
                case 6 ://oren
                    program.setUniformValue(program.uniformLocation("a"), params.oren_a);
                    program.setUniformValue(program.uniformLocation("b"), params.oren_b);
                    break;
                case 7 ://cook
                    break;
                case 8 ://aniso
                    break;
                case 9 ://aniso-ward
                    break;
                case 10 ://minnaert
                    program.setUniformValue(program.uniformLocation("k"), params.minnaert_k);
                    break;
                case 11 ://ashikhmin
                    program.setUniformValueArray(program.uniformLocation("specColor"), params.specColor, 1, 4);
                    program.setUniformValue(program.uniformLocation("specPower"), params.specPower);
                    break;
                case 12 ://cartoon
                    program.setUniformValueArray(program.uniformLocation("specColor"), params.specColor, 1, 4);
                    program.setUniformValue(program.uniformLocation("specPower"), params.specPower);
                    program.setUniformValue(program.uniformLocation("edgePower"), params.cartoon_edgePower);
                    break;
                case 13 ://gooch
                    program.setUniformValue(program.uniformLocation("diffuseWarm"), params.gooch_diffuseWarm);
                    program.setUniformValue(program.uniformLocation("diffuseCool"), params.gooch_diffuseCool);
                    break;
                case 14 ://rim
                    program.setUniformValueArray(program.uniformLocation("specColor"), params.specColor, 1, 4);
                    program.setUniformValue(program.uniformLocation("specPower"), params.specPower);
                    program.setUniformValue(program.uniformLocation("rimPower"), params.rim_rimPower);
                    program.setUniformValue(program.uniformLocation("bias"), params.rim_bias);
                    break;
                case 15 ://subsurface
                    program.setUniformValueArray(program.uniformLocation("specColor"), params.specColor, 1, 4);
                    program.setUniformValue(program.uniformLocation("specPower"), params.specPower);
                    program.setUniformValue(program.uniformLocation("matThickness"), params.subsurface_matThickness);
                    program.setUniformValue(program.uniformLocation("rimScalar"), params.subsurface_rimScalar);
                    break;
                case 16 ://bidirect
                    program.setUniformValueArray(program.uniformLocation("color2"), params.bidirect_color2, 1, 4);
                    break;
                case 17 ://hemisphere
                    program.setUniformValueArray(program.uniformLocation("color2"), params.hemispheric_color2, 1, 4);
                    break;
                case 18 ://trilight
                    program.setUniformValueArray(program.uniformLocation("color1"), params.trilight_color1, 1, 4);
                    program.setUniformValueArray(program.uniformLocation("color2"), params.trilight_color2, 1, 4);
                    break;
                case 19 ://lommel
                    break;
                case 20 ://strauss
                    program.setUniformValue(program.uniformLocation("smooth"), params.strauss_smooth);
                    program.setUniformValue(program.uniformLocation("metal"),  params.strauss_metal);
                    program.setUniformValue(program.uniformLocation("transp"), params.strauss_transp);
                    break;
            }
            glCallList(m_strDLA);
            program.release();
        }
    }
}

void StructureGL::resizeGL(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width / 2.0, width / 2.0, -height / 2.0, height / 2.0, -width, width);
//    glOrtho(-width / 2.0, width / 2.0, -height / 2.0, height / 2.0, -width, width);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void StructureGL::mousePressEvent(QMouseEvent* event)
{
    m_pressPos = event->pos();
}

void StructureGL::mouseMoveEvent(QMouseEvent* event)
{
    if (m_pressPos.y() < event->y() && (m_alpha + 0.01f < M_PI_2)) {
        m_alpha += 0.01f;
    } else if (m_pressPos.y() > event->y() && (-M_PI_2 < m_alpha - 0.01f)) {
        m_alpha -= 0.01f;
    }
    if (m_pressPos.x() > event->x()) {
        m_theta += 0.01f;
    } else if (m_pressPos.x() < event->x()) {
        m_theta -= 0.01f;
    }
    
    m_eyePos[0] = float(m_cameraDistance * cos(m_alpha) * sin(m_theta));
    m_eyePos[1] = float(m_cameraDistance * sin(m_alpha));
    m_eyePos[2] = float(m_cameraDistance * cos(m_alpha) * cos(m_theta));
    
    m_pressPos = event->pos();
    update();
}

void StructureGL::wheelEvent(QWheelEvent* event)
{
    float d = m_cameraDistance - float(event->delta()) / 120.0f;
    setCamera(d);
}

void StructureGL::keyPressEvent(QKeyEvent* /*event*/)
{
}

bool StructureGL::checkShaders()
{
    m_info[0].vert = ":/shader-lambert.vs";       m_info[0].frag = ":/shader-lambert.fs";
    m_info[1].vert = ":/shader-wrap.vs";          m_info[1].frag = ":/shader-wrap.fs";
    m_info[2].vert = ":/shader-phong.vs";         m_info[2].frag = ":/shader-phong.fs";
    m_info[3].vert = ":/shader-blinn.vs";         m_info[3].frag = ":/shader-blinn.fs";
    m_info[4].vert = ":/shader-iso-ward.vs";      m_info[4].frag = ":/shader-iso-ward.fs";
    m_info[5].vert = ":/shader-oren.vs";          m_info[5].frag = ":/shader-oren.fs";
    m_info[6].vert = ":/shader-cook.vs";          m_info[6].frag = ":/shader-cook.fs";//
    m_info[7].vert = ":/shader-aniso.vs";         m_info[7].frag = ":/shader-aniso.fs";//
    m_info[8].vert = ":/shader-aniso-ward.vs";    m_info[8].frag = ":/shader-aniso-ward.fs";//
    m_info[9].vert = ":/shader-minnaert.vs";      m_info[9].frag = ":/shader-minnaert.fs";
    m_info[10].vert = ":/shader-ashikhmin.vs";    m_info[10].frag = ":/shader-ashikhmin.fs";// +\-
    m_info[11].vert = ":/shader-cartoon.vs";      m_info[11].frag = ":/shader-cartoon.fs";
    m_info[12].vert = ":/shader-gooch.vs";        m_info[12].frag = ":/shader-gooch.fs";
    m_info[13].vert = ":/shader-rim.vs";          m_info[13].frag = ":/shader-rim.fs";
    m_info[14].vert = ":/shader-subsurface.vs";   m_info[14].frag = ":/shader-subsurface.fs";//
    m_info[15].vert = ":/shader-bidirect.vs";     m_info[15].frag = ":/shader-bidirect.fs";
    m_info[16].vert = ":/shader-hemisphere.vs";   m_info[16].frag = ":/shader-hemisphere.fs";
    m_info[17].vert = ":/shader-trilight.vs";     m_info[17].frag = ":/shader-trilight.fs";
    m_info[18].vert = ":/shader-lommel.vs";       m_info[18].frag = ":/shader-lommel.fs";
    m_info[19].vert = ":/shader-strauss.vs";      m_info[19].frag = ":/shader-strauss.fs";
    for (int32_t i = 0; i < 20; ++i) {
        if (!isSupported(i)) {
            // skip !
            continue;
        }
        ShaderInfo& info = m_info[i];
        QOpenGLShaderProgram& program = info.program;
        try {
            if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, info.vert)) {
                std::cout << "error load vertex shader" << std::endl;
                program.removeAllShaders();
                return false;
            }
            // Compile fragment shader
            if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, info.frag)) {
                std::cout << "error load fragment shader" << std::endl;
                program.removeAllShaders();
                return false;
            }
            QString log = program.log();
            if (!log.isEmpty()) {
                qDebug() << log;
            }
            // Link shader pipeline
            if (!program.link()) {
                std::cout << "error link program" << std::endl;
                program.removeAllShaders();
                return false;
            }
            // Bind shader pipeline for use
            if (!program.bind()) {
                std::cout << "error bind program" << std::endl;
                program.removeAllShaders();
                return false;
            }
            program.release();
        } catch (...) {
            std::cout << "error - some catch" << std::endl;
            program.removeAllShaders();
            return false;
        }
        //program.removeAllShaders();
    }
    return true;
}

void StructureGL::make(Field* fld, bool updateStr)
{
    if (!fld) {
        return;
    }
    int32_t sx = int32_t(fld->sizes().x);
    int32_t sy = int32_t(fld->sizes().y);
    int32_t sz = int32_t(fld->sizes().z);
    m_lightPos[0] = float(sx);
    m_lightPos[1] = float(sy);
    m_lightPos[2] = float(sz);
    clearList(m_border);
    glNewList(m_border, GL_COMPILE);
    if (showBorders) {
        glColor3ub(0, 0, 0);
        glBegin(GL_LINE_LOOP);
        glVertex3i(-(sx >> 1), -(sy >> 1), -(sz >> 1));
        glVertex3i(  sx >> 1,  -(sy >> 1), -(sz >> 1));
        glVertex3i(  sx >> 1,    sy >> 1,  -(sz >> 1));
        glVertex3i(-(sx >> 1),   sy >> 1,  -(sz >> 1));
        glEnd();
        glBegin(GL_LINE_LOOP);
        glVertex3i(-(sx >> 1), -(sy >> 1), sz >> 1);
        glVertex3i(  sx >> 1,  -(sy >> 1), sz >> 1);
        glVertex3i(  sx >> 1,    sy >> 1,  sz >> 1);
        glVertex3i(-(sx >> 1),   sy >> 1,  sz >> 1);
        glEnd();
        glBegin(GL_LINES);
        glVertex3i(-(sx >> 1), -(sy >> 1), -(sz >> 1));
        glVertex3i(-(sx >> 1), -(sy >> 1),   sz >> 1);
        glEnd();
        glBegin(GL_LINES);
        glVertex3i(sx >> 1, -(sy >> 1), -(sz >> 1));
        glVertex3i(sx >> 1, -(sy >> 1),   sz >> 1);
        glEnd();
        glBegin(GL_LINES);
        glVertex3i(-(sx >> 1), sy >> 1, -(sz >> 1));
        glVertex3i(-(sx >> 1), sy >> 1,   sz >> 1);
        glEnd();
        glBegin(GL_LINES);
        glVertex3i(sx >> 1, sy >> 1, -(sz >> 1));
        glVertex3i(sx >> 1, sy >> 1,   sz >> 1);
        glEnd();
    }
    glEndList();
    if (!updateStr) {
        return;
    }
    clearList(m_strDLA);
    glNewList(m_strDLA, GL_COMPILE);
    GLUquadricObj* quadObj = gluNewQuadric();
    // spheres
    for (const auto& cell : fld->cells()) {
        double ix = cell.coord().x;
        double iy = cell.coord().y;
        double iz = cell.coord().z;
        double dr = cell.figure()->radius();
        double h;
        FigureType type = cell.figure()->type();
        switch (type) {
            case fig_figure :
                break;
            case fig_sphere :
                glPushMatrix();
                glMaterialfv(GL_FRONT, GL_DIFFUSE, colors);
                glTranslated(ix - double(sx >> 1), iy - double(sy >> 1), iz - double(sz >> 1));
                gluSphere(quadObj, dr, 8, 8);
                glPopMatrix();
                break;
            case fig_cylinder :
                glPushMatrix();
                glMaterialfv(GL_FRONT, GL_DIFFUSE, colors);
                h = static_cast<FCylinder*>(cell.figure())->height();
                glTranslated(ix - double(sx >> 1), iy - double(sy >> 1), iz - double(sz >> 1));
//                glTranslated(ix - double(sx) / 2.0, iy - double(sy) / 2.0, iz - double(sz) / 2.0);
                glRotated(cell.rotate().x, 1.0, 0.0, 0.0);
                glRotated(cell.rotate().y, 0.0, 1.0, 0.0);
                //glRotated(cell.getRotate().z, 0.0, 0.0, 1.0); // not need - OZ AXE
                glTranslated(0.0, 0.0, -0.5 * h);
                gluCylinder(gluNewQuadric(), dr, dr, h, 10, 10);
                glTranslated(0.0, 0.0,  h);
                gluDisk(gluNewQuadric(), 0.0, dr, 10, 10);
                glTranslated(0.0, 0.0, -h);
                glRotated(180.0, 0.0, 1.0, 0.0);
                gluDisk(gluNewQuadric(), 0.0, dr, 10, 10);
                glPopMatrix();
                break;
            case fig_cube :
                // points
                //glPushMatrix();
                //glMaterialfv(GL_FRONT, GL_DIFFUSE, colors);
                //glTranslated(ix - double(sx) / 2.0, iy - double(sy) / 2.0, iz - double(sz) / 2.0);
                //glColor4f(c[0], c[1], c[2], c[3]);
                //glPointSize(2);
                //glBegin(GL_POINTS);
                //glVertex3d(ix - double(sx) / 2.0, iy - double(sy) / 2.0, iz - double(sz) / 2.0);
                //glEnd();
                //glPopMatrix();

                glBegin(GL_QUADS);
                glPushMatrix();
                //glTranslated(ix - double(sx) / 2.0, iy - double(sy) / 2.0, iz - double(sz) / 2.0);
                double xm = double(ix - double(sx >> 1) - dr);
                double xp = double(ix - double(sx >> 1) + dr);
                double ym = double(iy - double(sy >> 1) - dr);
                double yp = double(iy - double(sy >> 1) + dr);
                double zm = double(iz - double(sz >> 1) - dr);
                double zp = double(iz - double(sz >> 1) + dr);
                //cube

                //front
                glNormal3d(0.0, 0.0, 1.0);
                glVertex3d(xp, yp, zp);
                glVertex3d(xm, yp, zp);
                glVertex3d(xm, ym, zp);
                glVertex3d(xp, ym, zp);
                //back
                glNormal3d(0.0, 0.0, -1.0);
                glVertex3d(xp, ym, zm);
                glVertex3d(xm, ym, zm);
                glVertex3d(xm, yp, zm);
                glVertex3d(xp, yp, zm);
                //top
                glNormal3d(0.0, 1.0, 0.0);
                glVertex3d(xm, yp, zp);
                glVertex3d(xp, yp, zp);
                glVertex3d(xp, yp, zm);
                glVertex3d(xm, yp, zm);
                //bottom
                glNormal3d(0.0, -1.0, 0.0);
                glVertex3d(xm, ym, zp);
                glVertex3d(xm, ym, zm);
                glVertex3d(xp, ym, zm);
                glVertex3d(xp, ym, zp);
                //right
                glNormal3d(1.0, 0.0, 0.0);
                glVertex3d(xp, yp, zp);
                glVertex3d(xp, ym, zp);
                glVertex3d(xp, ym, zm);
                glVertex3d(xp, yp, zm);
                //left
                glNormal3d(-1.0, 0.0, 0.0);
                glVertex3d(xm, yp, zm);
                glVertex3d(xm, ym, zm);
                glVertex3d(xm, ym, zp);
                glVertex3d(xm, yp, zp);
                glPopMatrix();
                glEnd();
                //glutSolidCube(1);
                break;
        }
    }
    gluDeleteQuadric(quadObj);
    glEndList();
}
} // aerogen
