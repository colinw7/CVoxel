#ifndef CVoxel_H
#define CVoxel_H

#include <CIBBox2D.h>
#include <CRGBA.h>
#include <map>
#include <set>
#include <vector>
#include <string>

typedef unsigned char uchar;

class CVoxel {
 public:
  enum class Dir {
    X, Y, Z
  };

  struct Size {
    int x { 0 }, y { 0 }, z { 0 };

    Size(int x1=0, int y1=0, int z1=0) :
     x(x1), y(y1), z(z1) {
    }
  };

  //---

  struct Voxel {
    uchar x { 0 }, y { 0 }, z { 0 };
    uchar c { 0 };

    Voxel(uchar x1=0, uchar y1=0, uchar z1=0, uchar c1=0) :
     x(x1), y(y1), z(z1), c(c1) {
    }
  };

  typedef std::vector<Voxel> Voxels;

  //---

  struct Color {
    uchar r { 0 }, g { 0 }, b { 0 }, a { 0 };

    Color(uchar r1=0, uchar g1=0, uchar b1=0, uchar a1=0) :
     r(r1), g(g1), b(b1), a(a1) {
    }
  };

  typedef std::vector<Color> Colors;

  //---

  class SlicePoints {
   public:
    typedef std::map<int,int>     YColors;
    typedef std::map<int,YColors> XYColors;
    typedef std::set<int>         Colors;

   public:
    SlicePoints() { }

    void clear() { xyColors_.clear(); colors_.clear(); }

    void setValue(int x, int y, int c) {
      xyColors_[x][y] = c;

      colors_.insert(c);
    }

    int getValue(int x, int y) const {
      auto px = xyColors_.find(x);

      if (px == xyColors_.end())
        return -1;

      auto py = px->second.find(y);

      if (py == px->second.end())
        return -1;

      return py->second;
    }

    const Colors &colors() const { return colors_; }

   private:
    XYColors xyColors_;
    Colors   colors_;
  };

  //---

  struct SliceRect {
    SliceRect(const CIBBox2D &rect1, int z1, int c1) :
     rect(rect1), z(z1), c(c1) {
    }

    CIBBox2D rect;
    int      z { 0 };
    int      c { -1 };
  };

  typedef std::vector<SliceRect> SliceRects;

  //---

 public:
  CVoxel() { }

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  bool readVox(const std::string &filename);

  const Voxels &voxels() const { return voxels_; }

  int xmin() const { return xmin_; }
  int ymin() const { return ymin_; }
  int zmin() const { return zmin_; }

  int xmax() const { return xmax_; }
  int ymax() const { return ymax_; }
  int zmax() const { return zmax_; }

  void toSliceRects(SliceRects &rects) const;

  void slicePlane(Dir dir, int value, SliceRects &sliceRects) const;

  void outputRaytrace() const;

  CRGBA color(uchar c) const;

 private:
  Size   size_;
  Voxels voxels_;
  Colors colors_;
  int    xmin_ { 0 }, ymin_ { 0 }, zmin_ { 0 };
  int    xmax_ { 0 }, ymax_ { 0 }, zmax_ { 0 };
  bool   debug_ { false };
};

#endif
