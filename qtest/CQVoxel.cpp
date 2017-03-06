#include <CQVoxel.h>
#include <CQUtil.h>
#include <CQGLControl.h>
#include <CLargestRect.h>

#include <QApplication>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>

struct CRGBAToFV {
  float fvalues[4];

  CRGBAToFV(const CRGBA &rgba) {
    fvalues[0] = rgba.getRed  ();
    fvalues[1] = rgba.getGreen();
    fvalues[2] = rgba.getBlue ();
    fvalues[3] = rgba.getAlpha();
  }
};


//------

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  //---

  std::string filename;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string opt = std::string(argv[i]).substr(1);

      std::cerr << "Invalid option '" << opt << "'" << std::endl;
    }
    else {
      filename = argv[i];
    }
  }

  //---

  CQWindow *window = new CQWindow(filename);

  window->resize(600, 600);

  window->show();

  return app.exec();
}

//------

CQWindow::
CQWindow(const std::string &filename)
{
  loadFile(filename);

  //---

  QVBoxLayout *layout = new QVBoxLayout(this);

  QTabWidget *tab = new QTabWidget;

  layout->addWidget(tab);

  //---

  QWidget *glTab = new QWidget;

  QVBoxLayout *glLayout = new QVBoxLayout(glTab);

  glCanvas_  = new CQGLCanvas(this, true);
  glControl_ = new CQGLControl(glCanvas_);

  glToolbar_ = glControl_->createToolBar();

  connect(glControl_, SIGNAL(stateChanged()), this, SLOT(controlSlot()));

  glLayout->addWidget(glToolbar_);
  glLayout->addWidget(glCanvas_);

  //---

  QWidget *sliceTab = new QWidget;

  QVBoxLayout *sliceLayout = new QVBoxLayout(sliceTab);

  sliceCanvas_ = new CSliceCanvas(this);

  sliceLayout->addWidget(sliceCanvas_);

  //---

  tab->addTab(sliceTab, "Slice");
  tab->addTab(glTab   , "GL");
}

void
CQWindow::
loadFile(const std::string &filename)
{
  valid_ = voxel_.readVox(filename);

  //------

  // add slice rects
  voxel_.toSliceRects(sliceRects_);
}

void
CQWindow::
controlSlot()
{
  glCanvas_->update();
}

//------

CQGLCanvas::
CQGLCanvas(CQWindow *window, bool slice) :
 window_(window), slice_(slice)
{
  setFocusPolicy(Qt::StrongFocus);
}

void
CQGLCanvas::
paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  CQGLControl *control = window_->control();

  control->updateGL();

  if (list_num_ < 0)
    createList();

  if (list_num_ >= 0)
    glCallList(list_num_);
}

void
CQGLCanvas::
resizeGL(int width, int height)
{
  CQGLControl *control = window_->control();

  control->handleResize(width, height);

  //----

  glEnable(GL_COLOR_MATERIAL);

  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  CRGBA ambient(0.4, 0.4, 0.4);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, CRGBAToFV(ambient).fvalues);

  glEnable(GL_NORMALIZE);

  static GLfloat lpos1[4] = { 10.0,  10.0, 10.0, 0.0};
  static GLfloat lpos2[4] = {-10.0, -10.0, 10.0, 0.0};

  glLightfv(GL_LIGHT0, GL_POSITION, lpos1);
  glLightfv(GL_LIGHT1, GL_POSITION, lpos2);

  CRGBA lcolor1(0.5, 0.5, 0.5);
  CRGBA lcolor2(0.5, 0.5, 0.5);

  glLightfv(GL_LIGHT0, GL_DIFFUSE, CRGBAToFV(lcolor1).fvalues);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, CRGBAToFV(lcolor2).fvalues);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
}

void
CQGLCanvas::
mousePressEvent(QMouseEvent *e)
{
  CQGLControl *control = window_->control();

  control->handleMousePress(e);

  update();
}

void
CQGLCanvas::
mouseReleaseEvent(QMouseEvent *e)
{
  CQGLControl *control = window_->control();

  control->handleMouseRelease(e);

  update();
}

void
CQGLCanvas::
mouseMoveEvent(QMouseEvent *e)
{
  CQGLControl *control = window_->control();

  control->handleMouseMotion(e);

  update();
}

void
CQGLCanvas::
keyPressEvent(QKeyEvent *)
{
}

void
CQGLCanvas::
createList()
{
  list_num_ = glGenLists(1);

  glNewList(list_num_, GL_COMPILE);

  //---

  const CVoxel &voxel = window_->voxel();

  double xs = voxel.xmax() + voxel.xmin();
  double ys = voxel.ymax() + voxel.ymin();
  double zs = voxel.zmax() + voxel.zmin();

  double xc = (voxel.xmin() + voxel.xmax())/2.0;
  double yc = (voxel.ymin() + voxel.ymax())/2.0;
  double zc = (voxel.zmin() + voxel.zmax())/2.0;

  //double xscale = width ()/128.0;
  //double yscale = height()/128.0;

  //double scale = std::min(xscale, yscale);
  double scale = std::max(std::max(xs, ys), zs);

  double bs = 1.0/scale;

  //---

  if (slice_) {
    const CQWindow::SliceRects &sliceRects = window_->sliceRects();

    for (const auto &sliceRect : sliceRects) {
      CRGBA c = voxel.color(sliceRect.c);

      double x1 = (sliceRect.rect.getXMin() - xc)/scale;
      double y1 = (sliceRect.rect.getYMin() - yc)/scale;
      double x2 = (sliceRect.rect.getXMax() - xc)/scale;
      double y2 = (sliceRect.rect.getYMax() - yc)/scale;
      double z1 = (sliceRect.z - zc)/scale - bs/2;
      double z2 = z1 + bs;

      drawRectangle(x1, y1, z1, x2, y2, z2, c);
    }
  }
  else {
    for (const auto &v : voxel.voxels()) {
      CRGBA c = voxel.color(v.c);

      double x = (v.x - xc)/scale;
      double y = (v.y - yc)/scale;
      double z = (v.z - zc)/scale;

      drawBox(x, y, z, bs, c);
    }
  }

  //---

  glEndList();
}

void
CQGLCanvas::
drawBox(double x, double y, double z, double s, const CRGBA &c)
{
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, CRGBAToFV(c).fvalues);

  glColor3d(c.getRed(), c.getGreen(), c.getBlue());

  glPushMatrix();

  glTranslatef(x, y, z);

  glScalef(s, s, s);

  drawUnitCube();

  glPopMatrix();
}

void
CQGLCanvas::
drawRectangle(double x1, double y1, double z1, double x2, double y2, double z2, const CRGBA &c)
{
  double xm = (x1 + x2)/2;
  double ym = (y1 + y2)/2;
  double zm = (z1 + z2)/2;

  double xs = x2 - x1;
  double ys = y2 - y1;
  double zs = z2 - z1;

  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, CRGBAToFV(c).fvalues);

  glColor3d(c.getRed(), c.getGreen(), c.getBlue());

  glPushMatrix();

  glTranslatef(xm, ym, zm);

  glScalef(xs, ys, zs);

  drawUnitCube();

  glPopMatrix();
}

void
CQGLCanvas::
drawUnitCube()
{
  static GLfloat cube_normal[6][3] = {
    {-1.0,  0.0,  0.0},
    { 0.0,  1.0,  0.0},
    { 1.0,  0.0,  0.0},
    { 0.0, -1.0,  0.0},
    { 0.0,  0.0,  1.0},
    { 0.0,  0.0, -1.0}
  };

  static GLint cube_faces[6][4] = {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };

  GLfloat v[8][3];

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -0.5;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  0.5;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -0.5;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  0.5;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -0.5;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] =  0.5;

  for (GLint i = 5; i >= 0; i--) {
    glBegin(GL_POLYGON);

    glNormal3fv(&cube_normal[i][0]);

    glTexCoord2d(0.0, 0.0); glVertex3fv(&v[cube_faces[i][0]][0]);
    glTexCoord2d(1.0, 0.0); glVertex3fv(&v[cube_faces[i][1]][0]);
    glTexCoord2d(1.0, 1.0); glVertex3fv(&v[cube_faces[i][2]][0]);
    glTexCoord2d(0.0, 1.0); glVertex3fv(&v[cube_faces[i][3]][0]);

    glEnd();
  }
}

//------

CSliceCanvas::
CSliceCanvas(CQWindow *window) :
 window_(window)
{
  setFocusPolicy(Qt::StrongFocus);
}

void
CSliceCanvas::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.fillRect(rect(), QColor(255,255,255));

  //---

  // map constants
  const CVoxel &voxel = window_->voxel();

  xc_ = width ()/2;
  yc_ = height()/2;

  if      (dir_ == Dir::X) {
    xm_ = (voxel.ymin() + voxel.ymax())/2.0;
    ym_ = (voxel.zmin() + voxel.zmax())/2.0;
  }
  else if (dir_ == Dir::Y) {
    xm_ = (voxel.xmin() + voxel.xmax())/2.0;
    ym_ = (voxel.zmin() + voxel.zmax())/2.0;
  }
  else if (dir_ == Dir::Z) {
    xm_ = (voxel.xmin() + voxel.xmax())/2.0;
    ym_ = (voxel.ymin() + voxel.ymax())/2.0;
  }

  //---

  // draw rectangles
  s_ = 4;

  for (const auto &v : voxel.voxels()) {
    int vx, vy, vz;

    if      (dir_ == Dir::X) {
      vx = v.y; vy = v.z; vz = v.x;
    }
    else if (dir_ == Dir::Y) {
      vx = v.x; vy = v.z; vz = v.y;
    }
    else if (dir_ == Dir::Z) {
      vx = v.x; vy = v.y; vz = v.z;
    }

    if (vz != value_) continue;

    //---

    int x, y;

    mapPoint(vx, vy, x, y);

    CRGBA c = voxel.color(v.c);

    QRect rect(x - s_/2, y - s_/2, s_, s_);

    p.fillRect(rect, CQUtil::rgbaToColor(c));
  }

  //------

  // draw largest rectangles
  CVoxel::SliceRects sliceRects;

  voxel.slicePlane(dir_, value_, sliceRects);

  for (const auto &sliceRect : sliceRects) {
    int x, y;

    mapPoint(sliceRect.rect.getXMin(), sliceRect.rect.getYMin(), x, y);

    int w = sliceRect.rect.getWidth ()*s_;
    int h = sliceRect.rect.getHeight()*s_;

    QRect qrect(x - s_/2, y - s_/2, w, h);

    p.setPen(QColor(0,0,0));

    p.drawRect(qrect);
  }

  //------

  p.setPen(QColor(0,0,0));

  QFontMetrics fm(font());

  QString text;

  if      (dir_ == Dir::X) {
    text = QString("X: value %1 (min %2, max %3)").
            arg(value_).arg(voxel.xmin()).arg(voxel.xmax());
  }
  else if (dir_ == Dir::Y) {
    text = QString("Y: value %1 (min %2, max %3)").
            arg(value_).arg(voxel.ymin()).arg(voxel.ymax());
  }
  else if (dir_ == Dir::Z) {
    text = QString("Z: value %1 (min %2, max %3)").
            arg(value_).arg(voxel.zmin()).arg(voxel.zmax());
  }

  int tw = fm.width(text);
  int th = fm.height();

  p.drawText(width()/2 - tw/2, height() - th - 4, text);
}

void
CSliceCanvas::
keyPressEvent(QKeyEvent *e)
{
  const CVoxel &voxel = window_->voxel();

  if      (e->key() == Qt::Key_Up) {
    if (dir_ == Dir::X) {
      if (value_ < voxel.xmax()) { ++value_; update(); }
    }
    else if (dir_ == Dir::Y) {
      if (value_ < voxel.ymax()) { ++value_; update(); }
    }
    else if (dir_ == Dir::Z) {
      if (value_ < voxel.zmax()) { ++value_; update(); }
    }
  }
  else if (e->key() == Qt::Key_Down) {
    if (dir_ == Dir::X) {
      if (value_ > voxel.xmin()) { --value_; update(); }
    }
    else if (dir_ == Dir::Y) {
      if (value_ > voxel.ymin()) { --value_; update(); }
    }
    else if (dir_ == Dir::Z) {
      if (value_ > voxel.zmin()) { --value_; update(); }
   }
  }
  else if (e->key() == Qt::Key_Left) {
    if      (dir_ == Dir::Z) { dir_ = Dir::Y; update(); }
    else if (dir_ == Dir::Y) { dir_ = Dir::X; update(); }
  }
  else if (e->key() == Qt::Key_Right) {
    if      (dir_ == Dir::X) { dir_ = Dir::Y; update(); }
    else if (dir_ == Dir::Y) { dir_ = Dir::Z; update(); }
  }
}
