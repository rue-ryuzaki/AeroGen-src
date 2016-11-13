#include "structuregl.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QCoreApplication>
#include <qmath.h>
#include <GL/glu.h>
#include <QDebug>

#include "settingsform.h"

StructureGL::StructureGL(QWidget *parent) : QGLWidget(parent), shaderParams()
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
    
    cameraDistance = 10;

    alpha = M_PI / 6;
    theta = M_PI / 6;
}

StructureGL::~StructureGL()
{
    makeCurrent();
    glDeleteLists(strDLA, 1);
    delete gen;
    doneCurrent();
}

void StructureGL::initializeGL()
{
    initializeOpenGLFunctions();
    float ambientLight[] = { 0.2, 0.2, 0.2, 1.0 };
    float specularLight[] = { 1.0, 1.0, 1.0, 1.0 };
    //connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    //static const GLfloat lightPos[4] = { 5.0f, 5.0f, 10.0f, 1.0f };
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    shadersSupports = checkShaders();
    initialized = true;
    if (shadersSupports) {
        //initShaders();
        if (needInit != 0) {
            EnableShaders(needInit);
        } else {
            glUseProgram(0);
        }
    }
    qDebug("version: %s\n", glGetString(GL_VERSION));
}

bool StructureGL::checkShaders()
{
    svert[0] = ":/shader-lambert.vs"; sfrag[0] = ":/shader-lambert.fs";
    svert[1] = ":/shader-wrap.vs"; sfrag[1] = ":/shader-wrap.fs";
    svert[2] = ":/shader-phong.vs"; sfrag[2] = ":/shader-phong.fs";
    svert[3] = ":/shader-blinn.vs"; sfrag[3] = ":/shader-blinn.fs";
    svert[4] = ":/shader-iso-ward.vs"; sfrag[4] = ":/shader-iso-ward.fs";
    svert[5] = ":/shader-oren.vs"; sfrag[5] = ":/shader-oren.fs";
    svert[6] = ":/shader-cook.vs"; sfrag[6] = ":/shader-cook.fs";//
    svert[7] = ":/shader-aniso.vs"; sfrag[7] = ":/shader-aniso.fs";//
    svert[8] = ":/shader-aniso-ward.vs"; sfrag[8] = ":/shader-aniso-ward.fs";//
    svert[9] = ":/shader-minnaert.vs"; sfrag[9] = ":/shader-minnaert.fs";
    svert[10] = ":/shader-ashikhmin.vs"; sfrag[10] = ":/shader-ashikhmin.fs";// +\-
    svert[11] = ":/shader-cartoon.vs"; sfrag[11] = ":/shader-cartoon.fs";
    svert[12] = ":/shader-gooch.vs"; sfrag[12] = ":/shader-gooch.fs";
    svert[13] = ":/shader-rim.vs"; sfrag[13] = ":/shader-rim.fs";
    svert[14] = ":/shader-subsurface.vs"; sfrag[14] = ":/shader-subsurface.fs";//
    svert[15] = ":/shader-bidirect.vs"; sfrag[15] = ":/shader-bidirect.fs";
    svert[16] = ":/shader-hemisphere.vs"; sfrag[16] = ":/shader-hemisphere.fs";
    svert[17] = ":/shader-trilight.vs"; sfrag[17] = ":/shader-trilight.fs";
    svert[18] = ":/shader-lommel.vs"; sfrag[18] = ":/shader-lommel.fs";
    svert[19] = ":/shader-strauss.vs"; sfrag[19] = ":/shader-strauss.fs";
    for (int i = 0; i < 20; ++i) {
        if (i == 6 || i == 7 || i == 8 || i == 10) {
            // skip !
            continue;
        }
        try {
            if (!m_program[i].addShaderFromSourceFile(QOpenGLShader::Vertex, svert[i])) {
                std::cout << "error load vertex shader" << std::endl;
                m_program[i].removeAllShaders();
                return false;
            }
            // Compile fragment shader
            if (!m_program[i].addShaderFromSourceFile(QOpenGLShader::Fragment, sfrag[i])) {
                std::cout << "error load fragment shader" << std::endl;
                m_program[i].removeAllShaders();
                return false;
            }
            QString log = m_program[i].log();
            if (log != "") {
                qDebug() << log;
            }
            // Link shader pipeline
            if (!m_program[i].link()) {
                std::cout << "error link program" << std::endl;
                m_program[i].removeAllShaders();
                return false;
            }
            // Bind shader pipeline for use
            if (!m_program[i].bind()) {
                std::cout << "error bind program" << std::endl;
                m_program[i].removeAllShaders();
                return false;
            }
            m_program[i].release();
        } catch (...) {
            std::cout << "error - some catch" << std::endl;
            m_program[i].removeAllShaders();
            return false;
        }
        //m_program[i].removeAllShaders();
    }
    return true;
}

void StructureGL::EnableShaders(int value)
{
    if (value != 0) {
        if (shadersSupports) {
            shader = value;
            glUseProgram(m_program[shader - 1].programId());
        } else {
            shader = 0;
        }
    } else {
        if (shader != 0) {
            glUseProgram(0);
        }
        shader = 0;
    }
    update();
}

void StructureGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (drawGL) {
        draw();
    }
}

void StructureGL::draw()
{
    //glEnable(GL_CULL_FACE);
    //glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    eyePos[0] = cameraDistance * cos(alpha) * sin(theta);
    eyePos[1] = cameraDistance * sin(alpha);
    eyePos[2] = cameraDistance * cos(alpha) * cos(theta);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, 1,
                   0.01,
                   height());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //glDepthFunc(GL_LEQUAL);

    gluLookAt(eyePos[0], eyePos[1], eyePos[2],
              0, 0, 0,
              0, 1, 0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    /*GLUquadricObj *quadric = gluNewQuadric();
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
        glVertex3f(-300, 0, 0);
        glVertex3f(300, 0, 0);
        glVertex3f(0, -300, 0);
        glVertex3f(0, 300, 0);
        glVertex3f(0, 0, -300);
        glVertex3f(0, 0, 300);
        glEnd();
        //подписи к ним
        glColor3ub(0, 0, 0);
        renderText(270, 0, 0, "X");
        renderText(0, 270, 0, "Y");
        renderText(0, 0, 270, "Z");
        glEnable(GL_LIGHTING);
    }
    
    if (shader == 0) {
        glCallList(strDLA);
    } else {
        m_program[shader - 1].bind();
        m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("lightPos"), lightPos, 1, 4);
        m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("eyePos"), eyePos, 1, 4);
        m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("inColor"), colors, 1, 4);
        //glUniform4fv(m_colorAttr, 1, colors);
        switch (shader) {
            case 1://lambert
                break;
            case 2://wrap
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("factor"), shaderParams.wrap_factor);
                break;
            case 3://phong
                m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("specColor"), shaderParams.specColor, 1, 4);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("specPower"), shaderParams.specPower);
                break;
            case 4://blinn
                m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("specColor"), shaderParams.specColor, 1, 4);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("specPower"), shaderParams.specPower);
                break;
            case 5://iso-ward
                m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("specColor"), shaderParams.specColor, 1, 4);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("k"), shaderParams.iso_ward_k);
                break;
            case 6://oren
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("a"), shaderParams.oren_a);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("b"), shaderParams.oren_b);
                break;
            case 7://cook
                break;
            case 8://aniso
                break;
            case 9://aniso-ward
                break;
            case 10://minnaert
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("k"), shaderParams.minnaert_k);
                break;
            case 11://ashikhmin
                m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("specColor"), shaderParams.specColor, 1, 4);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("specPower"), shaderParams.specPower);
                break;
            case 12://cartoon
                m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("specColor"), shaderParams.specColor, 1, 4);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("specPower"), shaderParams.specPower);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("edgePower"), shaderParams.cartoon_edgePower);
                break;
            case 13://gooch
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("diffuseWarm"), shaderParams.gooch_diffuseWarm);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("diffuseCool"), shaderParams.gooch_diffuseCool);
                break;
            case 14://rim
                m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("specColor"), shaderParams.specColor, 1, 4);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("specPower"), shaderParams.specPower);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("rimPower"), shaderParams.rim_rimPower);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("bias"), shaderParams.rim_bias);
                break;
            case 15://subsurface
                m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("specColor"), shaderParams.specColor, 1, 4);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("specPower"), shaderParams.specPower);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("matThickness"), shaderParams.subsurface_matThickness);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("rimScalar"), shaderParams.subsurface_rimScalar);
                break;
            case 16://bidirect
                m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("color2"), shaderParams.bidirect_color2, 1, 4);
                break;
            case 17://hemisphere
                m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("color2"), shaderParams.hemispheric_color2, 1, 4);
                break;
            case 18://trilight
                m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("color1"), shaderParams.trilight_color1, 1, 4);
                m_program[shader - 1].setUniformValueArray(m_program[shader - 1].uniformLocation("color2"), shaderParams.trilight_color2, 1, 4);
                break;
            case 19://lommel
                break;
            case 20://strauss
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("smooth"), shaderParams.strauss_smooth);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("metal") , shaderParams.strauss_metal);
                m_program[shader - 1].setUniformValue(m_program[shader - 1].uniformLocation("transp"), shaderParams.strauss_transp);
                break;
        }
        glCallList(strDLA);
        m_program[shader - 1].release();
        //glFlush();
    }
}

void StructureGL::resizeGL(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width / 2, width / 2, -height / 2, height / 2, -width, width);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void StructureGL::mousePressEvent(QMouseEvent *event)
{
    pressPos = event->pos();
}

void StructureGL::mouseMoveEvent(QMouseEvent *event)
{
    if (pressPos.y() - event->y() > 0) { if (alpha + 0.01 < M_PI / 2) { alpha += 0.01; } }
    else if (pressPos.y() - event->y() < 0) { if (-M_PI / 2 < alpha - 0.01) { alpha -= 0.01; } }
    if (pressPos.x() - event->x() > 0) { theta += 0.01; }
    else if (pressPos.x() - event->x() < 0) { theta -= 0.01; }
    
    eyePos[0] = cameraDistance * cos(alpha) * sin(theta);
    eyePos[1] = cameraDistance * sin(alpha);
    eyePos[2] = cameraDistance * cos(alpha) * cos(theta);
    
    pressPos = event->pos();
    update();
}

void StructureGL::wheelEvent(QWheelEvent *event)
{
    int d = cameraDistance - (event->delta()) / 120;
    SetCamera(d);
}

void StructureGL::keyPressEvent(QKeyEvent* event)
{
    
}

void StructureGL::SetCamera(int d)
{
    if (d > 1) {
        cameraDistance = d;
    }
    update();
}

void StructureGL::Restruct()
{
    if (!loaded) return;
    loaded = false;
    if (gen != nullptr) {
        make(gen->GetField());
        update();
    }
    loaded = true;
}

void StructureGL::make(Field* fld)
{
    if (fld == nullptr) {
        return;
    }
    clearList(strDLA);
    glNewList(strDLA, GL_COMPILE);
    int sx = fld->getSizes().x;
    int sy = fld->getSizes().y;
    int sz = fld->getSizes().z;
    lightPos[0] = sx;
    lightPos[1] = sy;
    lightPos[2] = sz;
    if (showBorders) {
        glColor3ub(0, 0, 0);
        glBegin(GL_LINE_LOOP);
        glVertex3d(-sx/2, -sy/2, -sz/2);
        glVertex3d( sx/2, -sy/2, -sz/2);
        glVertex3d( sx/2,  sy/2, -sz/2);
        glVertex3d(-sx/2,  sy/2, -sz/2);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glVertex3d(-sx/2, -sy/2, sz/2);
        glVertex3d( sx/2, -sy/2, sz/2);
        glVertex3d( sx/2,  sy/2, sz/2);
        glVertex3d(-sx/2,  sy/2, sz/2);
        glEnd();
        glBegin(GL_LINES);
        glVertex3d(-sx/2, -sy/2, -sz/2);
        glVertex3d(-sx/2, -sy/2,  sz/2);
        glEnd();
        glBegin(GL_LINES);
        glVertex3d(sx/2, -sy/2, -sz/2);
        glVertex3d(sx/2, -sy/2,  sz/2);
        glEnd();
        glBegin(GL_LINES);
        glVertex3d(-sx/2, sy/2, -sz/2);
        glVertex3d(-sx/2, sy/2,  sz/2);
        glEnd();
        glBegin(GL_LINES);
        glVertex3d(sx/2, sy/2, -sz/2);
        glVertex3d(sx/2, sy/2,  sz/2);
        glEnd();
    }
    GLUquadricObj * quadObj = gluNewQuadric();
    // spheres
    for (const Cell & cell : fld->getCells()) {
        double ix = cell.getCoord().x;
        double iy = cell.getCoord().y;
        double iz = cell.getCoord().z;
        double dr = cell.getFigure()->getRadius();
        double h;
        FigureType type = cell.getFigure()->getType();
        switch (type) {
            case fig_sphere:
                glPushMatrix();
                glMaterialfv(GL_FRONT, GL_DIFFUSE, colors);
                glTranslated(ix - double(sx) / 2, iy - double(sy) / 2, iz - double(sz) / 2);
                gluSphere(quadObj, dr, 10, 10);
                glPopMatrix();
                break;
            case fig_cylinder:
                glPushMatrix();
                glMaterialfv(GL_FRONT, GL_DIFFUSE, colors);
                h = static_cast<FCylinder*>(cell.getFigure())->getHeight();
                glTranslated(ix - double(sx) / 2, iy - double(sy) / 2, iz - double(sz) / 2);
                glRotated(cell.getRotate().x, 1.0, 0.0, 0.0);
                glRotated(cell.getRotate().y, 0.0, 1.0, 0.0);
                //glRotated(cell.getRotate().z, 0.0, 0.0, 1.0); // not need - OZ AXE
                glTranslated(0, 0, - 0.5 * h);
                gluCylinder(gluNewQuadric(), dr, dr, h, 10, 10);
                glTranslated(0, 0,  h);
                gluDisk(gluNewQuadric(), 0, dr, 10, 10);
                glTranslated(0, 0, -h);
                glRotated(180.0, 0.0, 1.0, 0.0);
                gluDisk(gluNewQuadric(), 0, dr, 10, 10);
                glPopMatrix();
                break;
            case fig_cube:

                // points
                //glPushMatrix();
                //glMaterialfv(GL_FRONT, GL_DIFFUSE, colors);
                //glTranslated(ix - (double)sx / 2, iy - (double)sy / 2, iz - (double)sz / 2);
                //glColor4f(c[0], c[1], c[2], c[3]);
                //glPointSize(2);
                //glBegin(GL_POINTS);
                //glVertex3d(ix - (double)sx / 2, iy - (double)sy / 2, iz - (double)sz / 2);
                //glEnd();
                //glPopMatrix();

                glBegin(GL_QUADS);
                glPushMatrix();
                //glTranslated(ix - (double)sx / 2, iy - (double)sy / 2, iz - (double)sz / 2);
                double xm = (ix - double(sx) / 2 - dr);
                double xp = (ix - double(sx) / 2 + dr);
                double ym = (iy - double(sy) / 2 - dr);
                double yp = (iy - double(sy) / 2 + dr);
                double zm = (iz - double(sz) / 2 - dr);
                double zp = (iz - double(sz) / 2 + dr);
                //cube

                //front
                glNormal3d(0.0, 0.0, 1.0);
                glVertex3f(xp, yp, zp);
                glVertex3f(xm, yp, zp);
                glVertex3f(xm, ym, zp);
                glVertex3f(xp, ym, zp);
                //back
                glNormal3d(0.0, 0.0, -1.0);
                glVertex3f(xp, ym, zm);
                glVertex3f(xm, ym, zm);
                glVertex3f(xm, yp, zm);
                glVertex3f(xp, yp, zm);
                //top
                glNormal3d(0.0, 1.0, 0.0);
                glVertex3f(xm, yp, zp);
                glVertex3f(xp, yp, zp);
                glVertex3f(xp, yp, zm);
                glVertex3f(xm, yp, zm);
                //bottom
                glNormal3d(0.0, -1.0, 0.0);
                glVertex3f(xm, ym, zp);
                glVertex3f(xm, ym, zm);
                glVertex3f(xp, ym, zm);
                glVertex3f(xp, ym, zp);
                //right
                glNormal3d(1.0, 0.0, 0.0);
                glVertex3f(xp, yp, zp);
                glVertex3f(xp, ym, zp);
                glVertex3f(xp, ym, zm);
                glVertex3f(xp, yp, zm);
                //left
                glNormal3d(-1.0, 0.0, 0.0);
                glVertex3f(xm, yp, zm);
                glVertex3f(xm, ym, zm);
                glVertex3f(xm, ym, zp);
                glVertex3f(xm, yp, zp);
                glPopMatrix();
                glEnd();
                //glutSolidCube(1);
                break;
        }
    }
    gluDeleteQuadric(quadObj);
    glEndList();
}
        
void StructureGL::clearList(GLuint str)
{
    // проверяем факт существования дисплейного списка с номером, хранимым в ListNom 
    if (glIsList(str) == GL_TRUE) {
        // удаляем его в случае существования 
        glDeleteLists(str, 1);
    }
}
