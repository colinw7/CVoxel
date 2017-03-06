#ifndef CQVoxel_H
#define CQVoxel_H

#include <QGLWidget>
#include <CVoxel.h>
#include <map>
#include <set>

class CQGLCanvas;
class CQGLControl;
class CQGLControlToolBar;
class CSliceCanvas;

class CQWindow : public QWidget {
  Q_OBJECT

 public:
  typedef CVoxel::SliceRects SliceRects;

 public:
  CQWindow(const std::string &filename);

  void loadFile(const std::string &filename);

  bool isValid() const { return valid_; }

  const CVoxel &voxel() const { return voxel_; }

  CQGLControl *control() const { return glControl_; }

  const SliceRects &sliceRects() const { return sliceRects_; }

 private slots:
  void controlSlot();

 private:
  bool                valid_       { false };
  CVoxel              voxel_;
  CQGLCanvas*         glCanvas_    { nullptr };
  CQGLControl*        glControl_   { nullptr };
  CQGLControlToolBar* glToolbar_   { nullptr };
  CSliceCanvas*       sliceCanvas_ { nullptr };
  SliceRects          sliceRects_;
};

//---

class CQGLCanvas : public QGLWidget {
 public:
  CQGLCanvas(CQWindow *window, bool slice);

  void createList();

  void drawBox(double x, double y, double z, double s, const CRGBA &c);

  void drawRectangle(double x1, double y1, double z1,
                     double x2, double y2, double z2, const CRGBA &c);

  void drawUnitCube();

 private:
  void paintGL();

  void resizeGL(int, int);

  void mousePressEvent  (QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent   (QMouseEvent *event);

  void keyPressEvent(QKeyEvent *event);

 private:
  CQWindow* window_   { nullptr };
  bool      slice_    { true };
  int       list_num_ { -1 };
};

//---

class CSliceCanvas : public QWidget {
 public:
  typedef CVoxel::Dir Dir;

 public:
  CSliceCanvas(CQWindow *window);

  void paintEvent(QPaintEvent *);

  void keyPressEvent(QKeyEvent *e);

  void mapPoint(int x, int y, int &x1, int &y1) {
    x1 = (x - xm_)*s_ + xc_;
    y1 = (y - ym_)*s_ + yc_;
  }

 private:
  CQWindow* window_ { nullptr };
  Dir       dir_    { Dir::Z };
  int       value_  { 1 };
  int       xc_ { 0 };
  int       yc_ { 0 };
  int       xm_ { 0 };
  int       ym_ { 0 };
  int       s_  { 1 };
};

#endif
