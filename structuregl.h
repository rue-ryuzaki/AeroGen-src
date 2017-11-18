#ifndef AEROGEN_STRUCTUREGL_H
#define AEROGEN_STRUCTUREGL_H

#include <QtOpenGL>
#include <QGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

#include "settingsform.h"
#include "multidla/multidla.h"
#include "osm/osm.h"
#include "dlca/dlca.h"
#include "mxdla/mxdla.h"
#include "xdla/xdla.h"

struct ShaderInfo
{
    ShaderInfo()
        : program(),
          vert(),
          frag()
    { }

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

    void        setCamera(float d);
    uint32_t    shadersStatus() const;
    void        enableShader(int32_t value);
    bool        isSupported(int32_t value) const;
    void        restruct(bool updateStr);
    bool        supportShaders() const;
    bool        isInitialized() const;

    Generator*  gen;
    int32_t     needInit;
    GLfloat     colors[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    bool        drawGL;
    bool        showAxes;
    bool        showBorders;
    ShaderParameters params;

private:
    void        initializeGL() override;
    void        paintGL() override;
    void        resizeGL(int width, int height) override;
    void        mousePressEvent(QMouseEvent* event) override;
    void        mouseMoveEvent(QMouseEvent* event) override;
    void        wheelEvent(QWheelEvent* event) override;
    void        keyPressEvent(QKeyEvent* event) override;
    bool        checkShaders();
    void        draw();
    void        make(Field* fld, bool updateStr);

    GLuint      m_strDLA;
    GLuint      m_border;
    bool        m_initialized;
    bool        m_shadersSupports;
    int32_t     m_shader;
    bool        m_loaded;
    float       m_alpha;
    float       m_theta;
    float       m_cameraDistance;
    
    QPoint      m_pressPos;
    ShaderInfo  m_info[20];
    GLfloat     m_lightPos[4] = { 10.0f, 10.0f, 10.0f, 1.0f };
    GLfloat     m_eyePos[4] = { 5.0f, 5.0f, 10.0f, 1.0f };

    StructureGL(const StructureGL&) = delete;
    StructureGL& operator =(const StructureGL&) = delete;
};

#endif // AEROGEN_STRUCTUREGL_H
