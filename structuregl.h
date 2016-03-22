#ifndef GLWIDGET_H
#define	GLWIDGET_H

#include <QtOpenGL>
#include <QGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include "settingsform.h"
#include "multidla/multidla.h"
#include "osm/osm.h"
#include "dlca/dlca.h"
#include "xdla/xdla.h"

class StructureGL : public QGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    StructureGL(QWidget *parent = 0);
    ~StructureGL();

    void SetCamera(int d);
    int ShadersStatus() const { return shader; }
    void EnableShaders(int value);
    void Restruct();
    bool SupportShaders() const { return shadersSupports; }
    bool isInitialized() const { return initialized; }

    Generator * gen = nullptr;
    int needInit = 0;
    GLfloat colors[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    bool drawGL = true;
    bool showAxes = false;
    bool showBorders = false;
    ShaderParameters shaderParams;

signals:

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent* event);
    
private slots:

private:
    bool checkShaders();
    void draw();
    void make(Field * fld);
    
    void clearList(GLuint str);
    /* for QOpenGLWidget
    void renderText(double x, double y, double z, const QString &str,
            const QFont & font = QFont());
    
    inline GLint project(GLdouble objx, GLdouble objy, GLdouble objz,
        const GLdouble model[16], const GLdouble proj[16], const GLint viewport[4],
        GLdouble * winx, GLdouble * winy, GLdouble * winz);
    inline void transformPoint(GLdouble out[4], const GLdouble m[16], const GLdouble in[4]);*/
    GLuint strDLA = 1;
    bool initialized = false;
    bool shadersSupports = false;
    int shader = 0;
    bool loaded = true;
    float alpha;
    float theta;

    float cameraDistance;
    
    QPoint pressPos;
    QOpenGLShaderProgram m_program[20];
    QString svert[20];
    QString sfrag[20];
    GLfloat lightPos[4] = { 10.0f, 10.0f, 10.0f, 1.0f };
    GLfloat eyePos[4] = { 5.0f, 5.0f, 10.0f, 1.0f };
};

#endif	/* GLWIDGET_H */
