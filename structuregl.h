#ifndef STRUCTUREGL_H
#define	STRUCTUREGL_H

#include <QtOpenGL>
#include <QGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

#include "settingsform.h"
#include "multidla/multidla.h"
#include "osm/osm.h"
#include "dlca/dlca.h"
#include "xdla/xdla.h"

struct ShaderInfo
{
    QOpenGLShaderProgram    program;
    QString                 vert;
    QString                 frag;
};

class StructureGL : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit StructureGL(QWidget* parent = 0);
    ~StructureGL();

    void    setCamera(int d);
    int     shadersStatus() const;
    void    enableShader(int value);
    void    restruct();
    bool    supportShaders() const;
    bool    isInitialized() const;

    Generator* gen;
    int     needInit;
    GLfloat colors[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    bool    drawGL;
    bool    showAxes;
    bool    showBorders;
    ShaderParameters params;

signals:

protected:
    void    initializeGL();
    void    paintGL();
    void    resizeGL(int width, int height);
    void    mousePressEvent(QMouseEvent* event);
    void    mouseMoveEvent(QMouseEvent* event);
    void    wheelEvent(QWheelEvent* event);
    void    keyPressEvent(QKeyEvent* event);
    
private slots:

private:
    bool    checkShaders();
    void    draw();
    void    make(Field* fld);
    
    void    clearList(GLuint str);
    /* for QOpenGLWidget
    void renderText(double x, double y, double z, const QString& str,
            const QFont& font = QFont());
    
    inline GLint project(GLdouble objx, GLdouble objy, GLdouble objz,
        const GLdouble model[16], const GLdouble proj[16], const GLint viewport[4],
        GLdouble* winx, GLdouble* winy, GLdouble* winz);
    inline void transformPoint(GLdouble out[4], const GLdouble m[16], const GLdouble in[4]);*/
    GLuint  m_strDLA;
    bool    m_initialized;
    bool    m_shadersSupports;
    int     m_shader;
    bool    m_loaded;
    float   m_alpha;
    float   m_theta;
    float   m_cameraDistance;
    
    QPoint  m_pressPos;
    ShaderInfo m_info[20];
    GLfloat m_lightPos[4] = { 10.0f, 10.0f, 10.0f, 1.0f };
    GLfloat m_eyePos[4] = { 5.0f, 5.0f, 10.0f, 1.0f };
};

#endif	// STRUCTUREGL_H
